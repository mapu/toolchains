//===------ UCPSFixupHwLoop.cpp - Fixup HW loop too short from LOOP. ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// The loop start address in the LOOPn instruction is encoded as a distance
// from the LOOPn instruction itself.  If the start address is too far from
// the LOOPn instruction, the loop needs to be set up manually, i.e. via
// direct transfers to SAn and LCn.
// This pass will identify and convert such LOOPn instructions to a proper
// form.
//===----------------------------------------------------------------------===//


#include "llvm/ADT/DenseMap.h"
#include "UCPS.h"
#include "UCPSTargetMachine.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/PassSupport.h"
#include "llvm/Target/TargetInstrInfo.h"

using namespace llvm;

namespace llvm {
  void initializeUCPSFixupHwLoopPass(PassRegistry&);
}

namespace {
  struct UCPSFixupHwLoop : public MachineFunctionPass {
  public:
    static char ID;

    UCPSFixupHwLoop() : MachineFunctionPass(ID) {
      initializeUCPSFixupHwLoopPass(*PassRegistry::getPassRegistry());
    }

    bool runOnMachineFunction(MachineFunction &MF) override;

    const char *getPassName() const override {
      return "UCPS Hardware Loop Fixup";
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesCFG();
      MachineFunctionPass::getAnalysisUsage(AU);
    }

  private:

    static const int MIN_LOOP_DISTANCE = 28;

    bool fixupLoopInstrs(MachineFunction &MF);

  };

  char UCPSFixupHwLoop::ID = 0;
}

INITIALIZE_PASS(UCPSFixupHwLoop, "hwloopfixup",
                "UCPS Hardware Loop Fixup", false, false)

FunctionPass *llvm::createUCPSFixupHwLoop() {
  return new UCPSFixupHwLoop();
}


/// \brief Returns true if the instruction is a hardware loop instruction.
static bool isHardwareLoop(const MachineInstr *MI) {
  return MI->getOpcode() == UCPSInst::LoopL0;
}


bool UCPSFixupHwLoop::runOnMachineFunction(MachineFunction &MF) {
  bool Changed = fixupLoopInstrs(MF);
  return Changed;
}


bool UCPSFixupHwLoop::fixupLoopInstrs(MachineFunction &MF) {
  const UCPSTargetMachine *TM =
    static_cast<const UCPSTargetMachine*>(&MF.getTarget());
  const UCPSInstrInfo *TII = static_cast<const UCPSInstrInfo *>(
    TM->getSubtargetImpl(*MF.getFunction())->getInstrInfo());

  // Offset of the current instruction from the start.
  unsigned InstOffset = 0;
  // Map for each basic block to it's first instruction.
  DenseMap<const BasicBlock*, unsigned> BlockToInstOffset;
  DenseMap<const BasicBlock*, MachineBasicBlock*> BBtoMBB;

  // First pass - compute the offset of each basic block.
  for (MachineFunction::iterator MBB = MF.begin(), MBBe = MF.end();
       MBB != MBBe; ++MBB) {
    BlockToInstOffset[MBB->getBasicBlock()] = InstOffset;
    BBtoMBB[MBB->getBasicBlock()] = &*MBB;
    InstOffset += MBB->size();
  }

  // Second pass - check each loop instruction to see if it needs to
  // be converted.
  InstOffset = 0;
  bool Changed = false;

  // Loop over all the basic blocks.
  for (MachineFunction::iterator MBB = MF.begin(), MBBe = MF.end();
       MBB != MBBe; ++MBB) {
    InstOffset = BlockToInstOffset[MBB->getBasicBlock()];

    // Loop over all the instructions.
    MachineBasicBlock::iterator MII = MBB->begin();
    MachineBasicBlock *MIB;
    while (MII != MBB->end()) {
      if (isHardwareLoop(MII)) {
        assert(MII->getOperand(0).isBlockAddress() &&
               "Expect a block address as loop operand");
        const BlockAddress *BA = MII->getOperand(0).getBlockAddress();
        if (BlockToInstOffset.count(BA->getBasicBlock()) == 0)
          llvm_unreachable("Hardware loop block is not found.\n");
        int Dist = BlockToInstOffset[BA->getBasicBlock()] - InstOffset;
        if (Dist < 0) {
          llvm_unreachable("Backward hardware loop is not supported.\n");
        }
        MIB = BBtoMBB[BA->getBasicBlock()];
        if (Dist < MIN_LOOP_DISTANCE) {
          for (int i = 0; i < MIN_LOOP_DISTANCE - Dist; i++)
            BuildMI(MIB->getPrevNode(),
                    MIB->getPrevNode()->back().getDebugLoc(),
                    TII->get(UCPSInst::NOP));

          //MII = MBB->erase(MII);
        }
        MIB->setAlignment(4);
        MF.ensureAlignment(MIB->getAlignment());
        Changed = true;
      }
      ++MII;
      InstOffset++;
    }
  }

  return Changed;
}

