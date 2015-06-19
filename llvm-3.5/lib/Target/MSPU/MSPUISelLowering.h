//===-- MSPUISelLowering.h - MSPU DAG Lowering Interface ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that MSPU uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef MSPU_ISELLOWERING_H
#define MSPU_ISELLOWERING_H

#include "MSPU.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/CodeGen/CallingConvLower.h"

namespace llvm {
namespace MSPU_ISD {    // MSPU Instruction Selection DAG
enum {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  CMPICC,       // Compare two GPR operands, set icc.
  CMPFCC,       // Compare two FP operands, set fcc.
  BRICC,        // Branch to dest on icc condition
  BRFCC,        // Branch to dest on fcc condition
  SELECT_ICC,    // Select between two values using the current ICC flags.
  SELECT_FCC,    // Select between two values using the current FCC flags.

  Hi,
  Lo,           // Hi/Lo operations, typically on a global address.

  FTOI,         // FP to Int within a FP register.
  ITOF,         // Int to FP within a FP register.

  CALL,         // A call instruction.
  RET,    	    // Return.
  GLOBAL_BASE_REG,    // Global base reg for PIC
  FLUSHW,       // FLUSH register windows to stack
  XferAddr,
  Recip,
  ADJDYNALLOC    // pseudo opcode for alloca
};
}

class MSPUTargetLowering: public TargetLowering {
private:

public:
  MSPUTargetLowering(TargetMachine &TM);

  virtual SDValue
  LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  /// ReplaceNodeResults - This callback is invoked when a node result type is
  /// illegal for the target, and the operation was registered to use 'custom'
  /// lowering for that result type.  The target places new result values for
  /// the node in Results (their number and types must exactly match those of
  /// the original return values of the node), or leaves Results empty, which
  /// indicates that the node is not to be custom lowered after all.
  virtual void ReplaceNodeResults(SDNode * N,
                                  SmallVectorImpl<SDValue> &Results,
                                  SelectionDAG &DAG) const override;

  /// LowerOperationWrapper - This callback is invoked by the type legalizer
  /// to legalize nodes with an illegal operand type but legal result types.
  /// It replaces the LowerOperation callback in the type Legalizer.
  /*virtual void LowerOperationWrapper(SDNode *N,
   SmallVectorImpl<SDValue> &Results,
   SelectionDAG &DAG) const;*/

  /// computeKnownBitsForTargetNode - Determine which of the bits specified
  /// in Mask are known to be either zero or one and return them in the
  /// KnownZero/KnownOne bitsets.
  virtual void
  computeKnownBitsForTargetNode(const SDValue Op,
                                APInt &KnownZero,
                                APInt &KnownOne,
                                const SelectionDAG &DAG,
                                unsigned Depth = 0) const override;

  virtual MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr *MI, MachineBasicBlock *MBB) const
    override;

  virtual const char *
  getTargetNodeName(unsigned Opcode) const override;

  ConstraintType
  getConstraintType(const std::string &Constraint) const override;

  virtual std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const std::string &Constraint, MVT VT) const
    override;

  virtual bool
  isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;

  virtual SDValue
  LowerFormalArguments(SDValue Chain,
                       CallingConv::ID CallConv,
                       bool isVarArg,
                       const SmallVectorImpl<ISD::InputArg> &Ins,
                       SDLoc DL,
                       SelectionDAG &DAG,
                       SmallVectorImpl<SDValue> &InVals) const override;

  virtual SDValue
  LowerCall(CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const
    override;

  virtual SDValue
  LowerReturn(SDValue Chain,
              CallingConv::ID CallConv,
              bool IsVarArg,
              const SmallVectorImpl<ISD::OutputArg> &Outs,
              const SmallVectorImpl<SDValue> &OutVals,
              SDLoc DL,
              SelectionDAG &DAG) const override;

  SDValue
  LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerConstantPool(SDValue Op, SelectionDAG &DAG) const;

  // lower ConstantFP to ConstantPool, and let
  // LLVM lower call LowerConstantPool() to finish remaining lowerings.
  SDValue
  LowerConstantFP(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerFrameIndex(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerFNeg(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerFDiv(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerSelect(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerVAARG(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerVASTART(SDValue Op, SelectionDAG &DAG) const;

  SDValue
  LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;

  unsigned
  getSRetSize(const Function *CalleeFn) const;
};
}    // end namespace llvm

#endif    // MSPU_ISELLOWERING_H
