//===----- MSPUPacketizer.cpp - vliw packetizer ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This implements a simple VLIW packetizer using DFA. The packetizer works on
// machine basic blocks. For each instruction I in BB, the packetizer consults
// the DFA to see if machine resources are available to execute I. If so, the
// packetizer checks if I depends on any instruction J in the current packet.
// If no dependency is found, I is added to current packet and machine resource
// is marked as taken. If any dependency is found, a target API call is made to
// prune the dependence.
//
//===----------------------------------------------------------------------===//
#define DEBUG_TYPE "packets"
#include "llvm/CodeGen/DFAPacketizer.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/CodeGen/LatencyPriorityQueue.h"
#include "llvm/CodeGen/SchedulerRegistry.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineFunctionAnalysis.h"
#include "llvm/CodeGen/MachineBranchProbabilityInfo.h"
#include "llvm/CodeGen/ScheduleHazardRecognizer.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "MSPU.h"
#include "MSPUTargetMachine.h"
#include "MSPURegisterInfo.h"
#include "MSPUSubtarget.h"
#include "MSPUMachineFunctionInfo.h"

#include <map>
#include <vector>

using namespace llvm;

static void MSPUCheckImmFlag(MachineInstr *MI, unsigned &flag)
{
  #define AGUIMM 1
  #define SEQIMM 2
  switch(MI->getDesc().getOpcode()) {
    case MSPUInst::LoadSI8JI:
    case MSPUInst::LoadUI8JI:
    case MSPUInst::LoadSI16JI:
    case MSPUInst::LoadUI16JI:
    case MSPUInst::LoadI32JI:
    case MSPUInst::LoadF32JI:
    case MSPUInst::LoadF64JI:
    case MSPUInst::LoadPtrJI:
    case MSPUInst::LoadPtrOffsetJI:

    case MSPUInst::StoreI8JI:
    case MSPUInst::StoreI16JI:
    case MSPUInst::StoreI32JI:
    case MSPUInst::StoreF32JI:
    case MSPUInst::StoreF64JI:
    case MSPUInst::StorePtrJI:

    case MSPUInst::AssignRRegImm11:
    case MSPUInst::AssignJRegImm11:

    case MSPUInst::AssignI32:
    case MSPUInst::AssignPtr:
    case MSPUInst::AssignF32:
      flag |= AGUIMM;
      return;

    case MSPUInst::JumpImm:
    case MSPUInst::JumpBasicBlock:
    case MSPUInst::JumpImmCond:
    case MSPUInst::JumpBasicBlockCond:

    case MSPUInst::CallImm:
    case MSPUInst::CallImmCond:

    case MSPUInst::LoopL0:
    case MSPUInst::LoopL1:
      flag |= SEQIMM;
      return;

    default:
      return;
  }
}

static cl::opt<bool> PacketizeVolatiles(
      "mspu-packetize-volatiles",
      cl::ZeroOrMore,
      cl::Hidden, cl::init(true),
      cl::desc("Allow non-solo packetization of volatile memory references"));

namespace llvm
{
  void
  initializeMSPUPacketizerPass(PassRegistry&);
}

namespace
{
  class MSPUPacketizer: public MachineFunctionPass
  {

    public:
      static char ID;

      MSPUPacketizer() : MachineFunctionPass(ID)
      {
        initializeMSPUPacketizerPass(*PassRegistry::getPassRegistry());
      }

      void
      getAnalysisUsage(AnalysisUsage &AU) const
      {
        AU.setPreservesCFG();
        AU.addRequired<MachineDominatorTree>();
        AU.addRequired<MachineBranchProbabilityInfo>();
        AU.addPreserved<MachineDominatorTree>();
        AU.addRequired<MachineLoopInfo>();
        AU.addPreserved<MachineLoopInfo>();
        MachineFunctionPass::getAnalysisUsage(AU);
      }

      const char *
      getPassName() const
      {
        return "MSPU Packetizer";
      }

      bool
      runOnMachineFunction(MachineFunction &Fn);
  };

  char MSPUPacketizer::ID = 0;

  class MSPUPacketizerList: public VLIWPacketizerList
  {

    private:

      // Has the instruction been promoted to a dot-new instruction.
      bool PromotedToDotNew;

      // Has the instruction been glued to allocframe.
      bool GlueAllocframeStore;

      // Has the feeder instruction been glued to new value jump.
      bool GlueToNewValueJump;

      // Check if there is a dependence between some instruction already in this
      // packet and this instruction.
      bool Dependence;

      // Only check for dependence if there are resources available to
      // schedule this instruction.
      bool FoundSequentialDependence;

      /// \brief A handle to the branch probability pass.
      const MachineBranchProbabilityInfo *MBPI;

      // Track MIs with ignored dependece.
      std::vector<MachineInstr*> IgnoreDepMIs;

    public:
      // Ctor.
      MSPUPacketizerList(MachineFunction &MF, MachineLoopInfo &MLI,
                         const MachineBranchProbabilityInfo *MBPI);

      // initPacketizerState - initialize some internal flags.
      void
      initPacketizerState();

      // ignorePseudoInstruction - Ignore bundling of pseudo instructions.
      bool
      ignorePseudoInstruction(MachineInstr *MI, MachineBasicBlock *MBB);

      // isSoloInstruction - return true if instruction MI can not be packetized
      // with any other instruction, which means that MI itself is a packet.
      bool
      isSoloInstruction(MachineInstr *MI);

      // isLegalToPacketizeTogether - Is it legal to packetize SUI and SUJ
      // together.
      bool
      isLegalToPacketizeTogether(SUnit *SUI, SUnit *SUJ);

    private:
      bool
      IsCallDependent(MachineInstr* MI, SDep::Kind DepType, unsigned DepReg);
      bool
      tryAllocateResourcesForConstExt(MachineInstr* MI);
      bool
      canReserveResourcesForConstExt(MachineInstr *MI);
      void
      reserveResourcesForConstExt(MachineInstr* MI);

  };
}

INITIALIZE_PASS_BEGIN(MSPUPacketizer, "packets", "MSPU Packetizer", false, false)
INITIALIZE_PASS_DEPENDENCY(MachineDominatorTree)
INITIALIZE_PASS_DEPENDENCY(MachineBranchProbabilityInfo)
INITIALIZE_PASS_DEPENDENCY(MachineLoopInfo)
INITIALIZE_AG_DEPENDENCY(AliasAnalysis)
INITIALIZE_PASS_END(MSPUPacketizer, "packets", "MSPU Packetizer", false, false)

// MSPUPacketizerList Ctor.
MSPUPacketizerList::MSPUPacketizerList(MachineFunction &MF,
                                       MachineLoopInfo &MLI,
                                       const MachineBranchProbabilityInfo *MBPI)
      : VLIWPacketizerList(MF, MLI, true)
{
  this->MBPI = MBPI;
}

bool
MSPUPacketizer::runOnMachineFunction(MachineFunction &Fn)
{
  const TargetInstrInfo *TII = Fn.getSubtarget().getInstrInfo();
  MachineLoopInfo &MLI = getAnalysis<MachineLoopInfo>();
  const MachineBranchProbabilityInfo *MBPI = &getAnalysis<MachineBranchProbabilityInfo>();
  // Instantiate the packetizer.
  MSPUPacketizerList PacketizerList(Fn, MLI, MBPI);

  // DFA state table should not be empty.
  assert(PacketizerList.getResourceTracker() && "Empty DFA table!");

  //
  // Loop over all basic blocks and remove KILL pseudo-instructions
  // These instructions confuse the dependence analysis. Consider:
  // D0 = ...   (Insn 0)
  // R0 = KILL R0, D0 (Insn 1)
  // R0 = ... (Insn 2)
  // Here, Insn 1 will result in the dependence graph not emitting an output
  // dependence between Insn 0 and Insn 2. This can lead to incorrect
  // packetization
  //
  for(MachineFunction::iterator MBB = Fn.begin(), MBBe = Fn.end();
        MBB != MBBe; ++MBB) {
    MachineBasicBlock::iterator End = MBB->end();
    MachineBasicBlock::iterator MI = MBB->begin();
    while(MI != End) {
      if(MI->isKill()) {
        MachineBasicBlock::iterator DeleteMI = MI;
        ++MI;
        MBB->erase(DeleteMI);
        End = MBB->end();
        continue;
      }
      ++MI;
    }
  }

  // Loop over all of the basic blocks.
  for(MachineFunction::iterator MBB = Fn.begin(), MBBe = Fn.end();
        MBB != MBBe; ++MBB) {
    // Find scheduling regions and schedule / packetize each region.
    unsigned RemainingCount = MBB->size();
    for(MachineBasicBlock::iterator RegionEnd = MBB->end();
          RegionEnd != MBB->begin();) {
      // The next region starts above the previous region. Look backward in the
      // instruction stream until we find the nearest boundary.
      MachineBasicBlock::iterator I = RegionEnd;
      for(; I != MBB->begin(); --I, --RemainingCount) {
        if(TII->isSchedulingBoundary(std::prev(I), MBB, Fn))
          break;
      }
      I = MBB->begin();

      // Skip empty scheduling regions.
      if(I == RegionEnd) {
        RegionEnd = std::prev(RegionEnd);
        --RemainingCount;
        continue;
      }
      // Skip regions with one instruction.
      if(I == std::prev(RegionEnd)) {
        RegionEnd = std::prev(RegionEnd);
        continue;
      }

      PacketizerList.PacketizeMIs(MBB, I, RegionEnd);
      RegionEnd = I;
    }
  }

  return true;
}

static bool
IsRegDependence(const SDep::Kind DepType)
{
  return (DepType == SDep::Data || DepType == SDep::Anti ||
        DepType == SDep::Output);
}

static bool
IsControlFlow(MachineInstr* MI)
{
  return (MI->getDesc().isTerminator() || MI->getDesc().isCall());
}

/// DoesModifyCalleeSavedReg - Returns true if the instruction modifies a
/// callee-saved register.
static bool
DoesModifyCalleeSavedReg(MachineInstr *MI,
                         const TargetRegisterInfo *TRI)
{
  for(const uint16_t *CSR = TRI->getCalleeSavedRegs(); *CSR; ++CSR) {
    unsigned CalleeSavedReg = *CSR;
    if(MI->modifiesRegister(CalleeSavedReg, TRI))
      return true;
  }
  return false;
}

// initPacketizerState - Initialize packetizer flags
void
MSPUPacketizerList::initPacketizerState()
{

  Dependence = false;
  PromotedToDotNew = false;
  GlueToNewValueJump = false;
  GlueAllocframeStore = false;
  FoundSequentialDependence = false;

  return;
}

// ignorePseudoInstruction - Ignore bundling of pseudo instructions.
bool
MSPUPacketizerList::ignorePseudoInstruction(MachineInstr *MI,
                                            MachineBasicBlock *MBB)
{
  if(MI->isDebugValue())
    return true;

  // We must print out inline assembly
  if(MI->isInlineAsm())
    return false;

  // We check if MI has any functional units mapped to it.
  // If it doesn't, we ignore the instruction.
  const MCInstrDesc& TID = MI->getDesc();
  unsigned SchedClass = TID.getSchedClass();
  const InstrStage* IS = ResourceTracker->getInstrItins()->beginStage(SchedClass);
  unsigned FuncUnits = IS->getUnits();
  return !FuncUnits;
}

// isSoloInstruction: - Returns true for instructions that must be
// scheduled in their own packet.
bool
MSPUPacketizerList::isSoloInstruction(MachineInstr *MI)
{

  if(MI->isInlineAsm())
    return true;

  if(MI->isEHLabel())
    return true;

  // From MSPU V4 Programmer's Reference Manual 3.4.4 Grouping constraints:
  // trap, pause, barrier, icinva, isync, and syncht are solo instructions.
  // They must not be grouped with other instructions in a packet.
  /*if(IsSchedBarrier(MI))
    return true;*/

  return false;
}

// isLegalToPacketizeTogether:
// SUI is the current instruction that is out side of the current packet.
// SUJ is the current instruction inside the current packet against which that
// SUI will be packetized.
bool
MSPUPacketizerList::isLegalToPacketizeTogether(SUnit *SUI, SUnit *SUJ)
{
  MachineInstr *I = SUI->getInstr();
  MachineInstr *J = SUJ->getInstr();
  assert(I && J && "Unable to packetize null instruction!");

  unsigned flag = 0;
  MSPUCheckImmFlag(I,flag);
  MSPUCheckImmFlag(J,flag);
  if(flag == 3) return false; // disallow coexistance of AGU-IMM and SEQ-IMM

  const MCInstrDesc &MCIDI = I->getDesc();
  const MCInstrDesc &MCIDJ = J->getDesc();

  MachineBasicBlock::iterator II = I;

  const unsigned FrameSize = MF.getFrameInfo()->getStackSize();
  const MSPURegisterInfo* RegInfo =
    (const MSPURegisterInfo *)MF.getSubtarget().getRegisterInfo();
  const MSPUInstrInfo *InstrInfo = (const MSPUInstrInfo *) TII;

  // Inline asm cannot go in the packet.
  if(I->getOpcode() == TargetOpcode::INLINEASM)
    llvm_unreachable("Should not meet inline asm here!");

  if(isSoloInstruction(I))
    llvm_unreachable("Should not meet solo instr here!");

  if(SUJ->isSucc(SUI)) {
    for(unsigned i = 0; (i < SUJ->Succs.size()) && !FoundSequentialDependence; ++i)
    {

      if(SUJ->Succs[i].getSUnit() != SUI) {
        continue;
      }

      SDep::Kind DepType = SUJ->Succs[i].getKind();

      // For direct calls:
      // Ignore register dependences for call instructions for
      // packetization purposes except for those due to r31 and
      // predicate registers.
      //
      // For indirect calls:
      // Same as direct calls + check for true dependences to the register
      // used in the indirect call.
      //
      // We completely ignore Order dependences for call instructions
      //
      // For returns:
      // Ignore register dependences for return instructions like jumpr,
      // dealloc return unless we have dependencies on the explicit uses
      // of the registers used by jumpr (like r31) or dealloc return
      // (like r29 or r30).
      //
      // TODO: Currently, jumpr is handling only return of r31. So, the
      // following logic (specificaly IsCallDependent) is working fine.
      // We need to enable jumpr for register other than r31 and then,
      // we need to rework the last part, where it handles indirect call
      // of that (IsCallDependent) function. Bug 6216 is opened for this.
      //
      unsigned DepReg = 0;
      const TargetRegisterClass* RC = NULL;
      if(DepType == SDep::Data) {
        DepReg = SUJ->Succs[i].getReg();
        RC = RegInfo->getMinimalPhysRegClass(DepReg);
      }

      // Ignore output dependences due to superregs. We can
      // write to two different subregisters of R1:0 for instance
      // in the same cycle
      //

      //
      // Let the
      // If neither I nor J defines DepReg, then this is a
      // superfluous output dependence. The dependence must be of the
      // form:
      //  R0 = ...
      //  R1 = ...
      // and there is an output dependence between the two instructions
      // with
      // DepReg = D0
      // We want to ignore these dependences.
      // Ideally, the dependence constructor should annotate such
      // dependences. We can then avoid this relatively expensive check.
      //
      if(DepType == SDep::Output) {
        // DepReg is the register that's responsible for the dependence.
        unsigned DepReg = SUJ->Succs[i].getReg();

        // Check if I and J really defines DepReg.
        if(I->definesRegister(DepReg) ||
              J->definesRegister(DepReg)) {
          FoundSequentialDependence = true;
          break;
        }
      }

      //
      // Skip over anti-dependences. Two instructions that are
      // anti-dependent can share a packet
      //
      else if(DepType != SDep::Anti) {
        FoundSequentialDependence = true;
        break;
      }
    }

    if(FoundSequentialDependence) {
      Dependence = true;
      return false;
    }
  }

  return true;
}

//===----------------------------------------------------------------------===//
//                         Public Constructor Functions
//===----------------------------------------------------------------------===//

FunctionPass *
llvm::createMSPUPacketizer()
{
  return new MSPUPacketizer();
}
