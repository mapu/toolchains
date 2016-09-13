//===-- UCPSISelLowering.cpp - UCPS DAG Lowering Implementation ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that UCPS uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "UCPSISelLowering.h"
#include "UCPSTargetMachine.h"
#include "UCPSMachineFunctionInfo.h"
#include "UCPSRegisterInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

static unsigned UCPS_NumFixedArgs = 0; // assume that callee has no arg
/*
static bool
CC_UCPS_Assign_SRet(	unsigned &ValNo,
						MVT &ValVT,
						MVT &LocVT,
						CCValAssign::LocInfo &LocInfo,
						ISD::ArgFlagsTy &ArgFlags,
						CCState &State)
{
	assert(ArgFlags.isSRet());

	//Assign SRet argument
	State.addLoc(CCValAssign::getCustomMem(ValNo, ValVT, 0, LocVT, LocInfo));
	return true;
}*/

#include "UCPSGenCallingConv.inc"

static bool
UCPSPassArg (unsigned ValNo, MVT ValVT,
                        MVT LocVT, CCValAssign::LocInfo LocInfo,
                        ISD::ArgFlagsTy ArgFlags, CCState &State)
{
  // promote meta integer types
  if (LocVT == MVT::i1 ||
      LocVT == MVT::i8 ||
      LocVT == MVT::i16) {
    LocVT = MVT::i32;
    if (ArgFlags.isSExt())
        LocInfo = CCValAssign::SExt;
    else if (ArgFlags.isZExt())
        LocInfo = CCValAssign::ZExt;
    else
        LocInfo = CCValAssign::AExt;
  }

  if(State.isVarArg() && 0 < UCPS_NumFixedArgs && UCPS_NumFixedArgs <= ValNo) {
    // these conditons hold only when we are handling a real variable arg of a function call,
    // not in its definition.
    return UCPSPassVarArg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State);
  }

  return UCPSPassFixedArg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State);
}

/// LowerReturn - This hook must be implemented to lower outgoing
/// return values, described by the Outs (? OutVals ?) array, into the specified
/// DAG. The implementation should return the resulting token chain
/// value.

// This function analyzes C "return" statements in 2 ways:
// 1. it prepares return value SDNode.
// 2. it inserts a SDNode for return to caller.
// Note that epilogue is inserted between them.
// Outs is a physical register array, obtained by AnalyzeReturn()???
SDValue
UCPSTargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                SDLoc DL, SelectionDAG &DAG) const
{
  MachineFunction &MF = DAG.getMachineFunction();

  // CCValAssign - represent the assignment of the return value to locations.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  // Build an instance and initialize its members.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                 RVLocs, *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, UCPSPassArg);
  // now RVLocs obtain info about which registers to use for which return values.


  // If this is the first return lowered for this function, add
  // the regs to the liveout set for the function.
  // Attention that liveness can not be set multiple times.
  /*if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }*/

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain); // fill in the first element

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i]; // hold resolved return values.
    assert(VA.isRegLoc() && "Can only return in registers!");
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);
    // Guarantee that all emitted copies are stuck together with flags.
    Glue = Chain.getValue(1); // output of getCopyToReg()

    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // If the function returns a struct
  if (MF.getFunction()->hasStructRetAttr()) {
    UCPSMachineFunctionInfo *FInfo = MF.getInfo<UCPSMachineFunctionInfo>();

    unsigned SRetReg = FInfo->getSRetReg();
    if (SRetReg == 0)
      llvm_unreachable("sret virtual register should be created in LowerFormalArguments()");

    /*Chain = DAG.getMemcpy(Chain, DL, SRetReg, srcArg, size, argAlign,
                          false,        //isVolatile,
                          (argSize < 8), //AlwaysInline if size <= 32
                          MachinePointerInfo::getFixedStack(dstOnStack),
                          MachinePointerInfo());*/

    SDValue Val = DAG.getCopyFromReg(Chain, DL, SRetReg, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));
    Chain = Val.getValue(1);
    Chain = DAG.getCopyToReg(Chain, DL, UCPSReg::R0, Val, Glue);

    Glue = Chain.getValue(1); // output of getCopyToReg()

    RetOps.push_back(DAG.getRegister(UCPSReg::R0, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout())));
  }

  RetOps[0] = Chain;  // Update chain.

  // Add the Glue if we have it.
  // This will fail for a void-return function.
  if (Glue.getNode())
    RetOps.push_back(Glue);

  return DAG.getNode(UCPS_ISD::RET, DL, MVT::Other, RetOps);
}

/// LowerFormalArguments - This hook must be implemented to lower the
/// incoming (formal) arguments, described by the 'Ins' array, into the
/// specified DAG. The implementation should fill in the 'InVals' array
/// with legal-type argument values, and return the resulting token-chain value.
SDValue
UCPSTargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv, bool isVarArg,
                                         const SmallVectorImpl<ISD::InputArg> &Ins,
                                         SDLoc DL, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals) const
{
	MachineFunction &MF = DAG.getMachineFunction();
	MachineRegisterInfo &RegInfo = MF.getRegInfo();
	UCPSMachineFunctionInfo *FuncInfo = MF.getInfo<UCPSMachineFunctionInfo>();

	// Assign locations to all of the incoming arguments.
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
	               ArgLocs, *DAG.getContext());

	// refer to generated *UCPSCallConv*
	CCInfo.AnalyzeFormalArguments(Ins, UCPSPassArg);

	unsigned idx = 0;

  // step 1: if this function returne a struct, then pass it like a pointer as if this pointer is
  // the first formal argument.
  // Note that returning a struct will change function signature implicitly.
  if(MF.getFunction()->hasStructRetAttr()) {
    assert(Ins.size()>0 && "expect at least one formal arg for struct return");
    assert(Ins[0].Flags.isSRet() && "the first arg should be for struct return");

    unsigned SRetReg = FuncInfo->getSRetReg();
    if(SRetReg == 0) {
      SRetReg = MF.getRegInfo().createVirtualRegister(&UCPSReg::I32RegRegClass);
      MF.getRegInfo().addLiveIn(ArgLocs[0].getLocReg(), SRetReg);

      SDValue SRetVal = DAG.getCopyFromReg(Chain, DL, SRetReg, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));
      InVals.push_back(SRetVal);

      //Chain = SRetVal.getValue(1);

      FuncInfo->setSRetReg(SRetReg);
      idx = 1; // skip struct return arg.
    }

    else llvm_unreachable("SRetReg can not be set anywhere else");
  } // end step 1

	// step 2: handling all formal arguments, one by one
	for(unsigned end = ArgLocs.size(); idx < end; ++idx) {
	  CCValAssign &ArgLoc = ArgLocs[idx];
	  //ISD::ArgFlagsTy Flags = Ins[idx].Flags;

    if(ArgLoc.needsCustom()) { // formal arg has wide type.
      continue;
    }

		// if the arg fits into some register.
		// This is the most common situation.
		if(ArgLoc.isRegLoc()) {
			unsigned VReg;
			SDValue ArgVal;
			MVT LocType = ArgLoc.getLocVT();

			if(LocType == MVT::i32) {
				VReg = RegInfo.createVirtualRegister(&UCPSReg::I32RegRegClass);
			}
			else if(LocType == MVT::f32) {
				VReg = RegInfo.createVirtualRegister(&UCPSReg::F32RegRegClass);
			}
			else if(LocType == MVT::f64) {
				VReg = RegInfo.createVirtualRegister(&UCPSReg::F64RegRegClass);
			}
			else
				llvm_unreachable("invalid formal argument type to be passed in register!");

			MF.getRegInfo().addLiveIn(ArgLoc.getLocReg(), VReg);

			ArgVal = DAG.getCopyFromReg(Chain, DL, VReg, LocType);
			InVals.push_back(ArgVal);

			// This incurs a severe problem.
			//Chain = ArgVal.getValue(1);

			continue;
		}

		// if the arg should reside in memory.
    // For example, if the number of args is more than 6 for UCPS.
		// In this case, the InVal holds its memory address.
    if(ArgLoc.isMemLoc()) {
      unsigned Offset = ArgLoc.getLocMemOffset();
      unsigned size = (ArgLoc.getLocVT().getSizeInBits() + 7) / 8;

      int FrameObj = MF.getFrameInfo()->CreateFixedObject(size, Offset, true);
      SDValue FI = DAG.getFrameIndex(FrameObj, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));

      /*if(Flags.isByVal()) { // if it is a struct instance, this is not happen in SPU C
        // If it's a pass-by-value aggregate, then do not dereference the stack
        // location. Instead, we should generate a reference to the stack
        // location.
        InVals.push_back(FI);
      }
      else*/ { // when this is a pointer to its memory address.
        SDValue Load = DAG.getLoad(ArgLoc.getLocVT(), DL, Chain, FI,
                                   MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), Offset),
                                   false, false, false, 0);

        InVals.push_back(Load);

        //Chain = Load.getValue(1);
      }

      continue;
    }

    llvm_unreachable("invalid location for arg passing");
	} // end step 2

	// step 3: Store remaining ArgReg to the stack if this is a varargs function.
	if(isVarArg) {
    // This will point to the start address of the first var arg.
    int FrameIndex = MF.getFrameInfo()->CreateFixedObject(8 /* any size??? */,
                                            CCInfo.getNextStackOffset(),
                                            true);

    FuncInfo->setVarArgsFrameIndex(FrameIndex);

	} // end step 3

	// return the original Chain.
	return Chain;
}

SDValue
UCPSTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                  SmallVectorImpl<SDValue> &InVals) const
{
  SelectionDAG &DAG                       = CLI.DAG;
  SDLoc &DL                               = CLI.DL;
  SmallVector<ISD::OutputArg, 32> &OutTys = CLI.Outs;    // init info on how to pass each arg
  SmallVector<SDValue, 32> &OutVals       = CLI.OutVals; // init real arguments passed to callee
  SmallVector<ISD::InputArg, 32> &Ins     = CLI.Ins;     // retruned value by callee
  bool &isTailCall                        = CLI.IsTailCall;

  SDValue Chain                           = CLI.Chain;
  SDValue Callee                          = CLI.Callee;
  CallingConv::ID CallConv                = CLI.CallConv;
  bool isVarArg                           = CLI.IsVarArg;

  MachineFrameInfo *FrameInfo = DAG.getMachineFunction().getFrameInfo();

  // At the very beginning, we assume callee has no arg.
  UCPS_NumFixedArgs = 0;

  // does not yet support tail call optimization.
  isTailCall = false;

  SmallVector<CCValAssign, 16> ArgLocs; // info of how each arg is passed
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 ArgLocs, *DAG.getContext());

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  GlobalAddressSDNode *GA = dyn_cast<GlobalAddressSDNode>(Callee);
  ExternalSymbolSDNode *ES = dyn_cast<ExternalSymbolSDNode>(Callee);

  bool hasSRet = false;
  unsigned SRetSize = 0;

  if (GA)
  {
    Callee = DAG.getTargetGlobalAddress(GA->getGlobal(), DL, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));

    const Function* CalleeFn = dyn_cast<Function>(GA->getGlobal());
    if (CalleeFn)
    {
      hasSRet = CalleeFn->hasStructRetAttr();
      SRetSize = getSRetSize(CalleeFn, DAG);

      // It is disallowed that a function has zero args and is a vararg function
      // so it must be an undeclared function. Do not assume
      // varargs if the callee is undefined.
      UCPS_NumFixedArgs = CalleeFn->getFunctionType()->getNumParams();
    }
  }
  else if (ES)
    Callee = DAG.getTargetExternalSymbol(ES->getSymbol(), DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));

  // assign locations for each operand according to its value type.
  // UCPSCallConv is a function pointer and is invoked within AnalyzeCallOperands().
  // CCState::Locs is filled in, which is *ArgLocs* here.
  // And now whether an arg is in-mem or in-reg is decided.
  // Whether an arg is by-val or SRet is determined much earlier.
  CCInfo.AnalyzeCallOperands(OutTys, UCPSPassArg);

  // Get the size of the outgoing arguments stack space requirement.
  // CCState::StackOffset records memory size needed for arg passing, so
  // if we want to pass an arg in-mem, we should perform this in AnalyzeCallOperands()
  // via UCPSCallConv().
  unsigned MemArgSize = CCInfo.getNextStackOffset();

  // Create local copies for by-value args.
  SmallVector<SDValue, 8> MemArgs;
  SmallVector<SDValue, 8> MemArgChains;

  for (unsigned i = 0,  e = OutTys.size(); i != e; ++i) {
    ISD::ArgFlagsTy argFlags = OutTys[i].Flags;

    if(argFlags.isByVal() && i < UCPS_NumFixedArgs) {

      // the original address of the by-value arg which is usually a struct object.
      SDValue SrcAddr = OutVals[i]; // its address

      unsigned Size = argFlags.getByValSize();
      unsigned Align = argFlags.getByValAlign();

      int FIndx = FrameInfo->CreateStackObject(Size, Align, false);
      SDValue DstAddr = DAG.getFrameIndex(FIndx, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));
      SDValue SizeVal = DAG.getConstant(Size, DL, MVT::i32);

      // call memcpy() to move this arg to dst on stack
      SDValue CopyChain = DAG.getMemcpy(Chain, DL, DstAddr, SrcAddr, SizeVal, Align,
                                        false, //isVolatile
                                        (Size < 8), //AlwaysInline if size <= 32
                                        /*isTailCall=*/false,
                                        MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FIndx),
                                        MachinePointerInfo());

      MemArgChains.push_back(CopyChain);
      MemArgs.push_back(DstAddr);
    }
  }

  // Emit all stores, make sure this occurs before any copies into physregs.
  if (!MemArgChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemArgChains);

  /*---------------------- call start ----------------------------*/
  Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(MemArgSize, DL, true), DL);

  // This occurs a critical bug.
  /* void llvm::ScheduleDAGTopologicalSort::InitDAGTopologicalSorting():
   * Assertion `Node2Index[SU->NodeNum] > Node2Index[I->getSUnit()->NodeNum]
   *  && "Wrong topological sorting"' failed */
  SDValue Glue /*= Chain.getValue(1)*/;

  // two ways to pass args: register and memory
  // which is determined in *CCInfo.AnalyzeCallOperands*
  SmallVector<std::pair<unsigned, SDValue>, 8> RegLocs;
  SmallVector<SDValue, 8> MemLocs;

  // Walk the register/memloc assignments, inserting copies/loads.
  // Note: in SPU backend, CCState::ArgLocs holds just the real-args passed directly.
  for (unsigned i = 0, byValIdx = 0, end = OutTys.size();
       i != end; ++i)
  {
    CCValAssign ArgLoc = ArgLocs[i]; // the iterator
    SDValue Arg = OutVals[i];

    ISD::ArgFlagsTy Flags = OutTys[i].Flags;

    // if it is a fixed and by-val arg we pass its copy address.
    if (Flags.isByVal() && i < UCPS_NumFixedArgs)
      Arg = MemArgs[byValIdx++];

    // Promote the value if needed.
    switch(ArgLoc.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");

      case CCValAssign::Full: break;

      case CCValAssign::SExt:
        Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, ArgLoc.getLocVT(), Arg);
        break;
      case CCValAssign::ZExt:
        Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, ArgLoc.getLocVT(), Arg);
        break;
      case CCValAssign::AExt:
        Arg = DAG.getNode(ISD::ANY_EXTEND, DL, ArgLoc.getLocVT(), Arg);
        break;
      case CCValAssign::BCvt:
        Arg = DAG.getNode(ISD::BITCAST, DL, ArgLoc.getLocVT(), Arg);
        break;
    }

    // reg or mem
    if (ArgLoc.isRegLoc()) {
      RegLocs.push_back(std::make_pair(ArgLoc.getLocReg(), Arg));
    }
    else if(ArgLoc.isMemLoc()) {
      unsigned Offset = ArgLoc.getLocMemOffset();
      unsigned SPReg = UCPSReg::J29;
      SDValue SP;
      SDValue DstAddr;

      if(Flags.isByVal() && UCPS_NumFixedArgs <= i) {
        SP = DAG.getCopyFromReg(Chain, DL, SPReg, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));
        DstAddr = DAG.getNode(ISD::ADD, DL, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()), SP, DAG.getIntPtrConstant(Offset, DL));

        unsigned Size = Flags.getByValSize();
        unsigned Align = Flags.getByValAlign();
        SDValue SizeVal = DAG.getConstant(Size, DL, MVT::i32);
        SDValue CopyChain = DAG.getMemcpy(Chain, DL, DstAddr, Arg, SizeVal, Align,
                                          false, //isVolatile
                                          (Size < 8), //AlwaysInline if size <= 32
                                          /*isTailCall=*/false,
                                          MachinePointerInfo::getStack(DAG.getMachineFunction(), Offset),
                                          MachinePointerInfo());

        MemLocs.push_back(CopyChain);
      }
      else {
        // Create a store off the stack pointer for this argument.
        SP = DAG.getRegister(SPReg, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));
        DstAddr = DAG.getNode(ISD::ADD, DL, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()), SP, DAG.getIntPtrConstant(Offset, DL));
        SDValue StoreChain = DAG.getStore(Chain, DL, Arg, DstAddr,
                                          MachinePointerInfo::getStack(DAG.getMachineFunction(), Offset),
                                          false, false, 0);

        MemLocs.push_back(StoreChain);
      }
    }

    else llvm_unreachable("unsupported argument location!");
  }

  // Emit all stores, make sure this occurs before any copies into physregs.
  if (!MemLocs.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemLocs);

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag is necessary since all emitted instructions must be
  // stuck together.
  for (unsigned i = 0, e = RegLocs.size(); i != e; ++i) {
    unsigned Reg = RegLocs[i].first;
    Chain = DAG.getCopyToReg(Chain, DL, Reg, RegLocs[i].second, Glue);
    Glue = Chain.getValue(1);
  }

  // Returns a chain & a glue for retval copy to use
  SDVTList VTList = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(CLI.DAG.getMachineFunction(), CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (hasSRet)
    Ops.push_back(DAG.getTargetConstant(SRetSize, DL, MVT::i32));

  // do not collect *StackLocs* here.
  for (unsigned i = 0, e = RegLocs.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegLocs[i].first,
                                  RegLocs[i].second.getValueType()));

  // Glue is invalid if no arg is passed.
  if(Glue.getNode())  Ops.push_back(Glue);

  Chain = DAG.getNode(UCPS_ISD::CALL, DL, VTList, Ops);
  Glue = Chain.getValue(1);

  /*---------------------- call end ----------------------------*/
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(MemArgSize, DL, true),
                             DAG.getIntPtrConstant(0, DL, true), Glue, DL);

  Glue = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetState(CallConv, isVarArg, DAG.getMachineFunction(),
                   RVLocs, *DAG.getContext());

  RetState.AnalyzeCallResult(Ins, UCPSPassArg);

  // Note: we do not need to handle returned struct value here, since
  // compiler treat this as void return type.

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, DL, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), Glue).getValue(1);
    Glue = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

unsigned
UCPSTargetLowering::getSRetSize(const Function* CalleeFn, SelectionDAG &DAG) const
                                {

  if(CalleeFn && CalleeFn->hasStructRetAttr()) {
    PointerType *Ty = cast<PointerType>(CalleeFn->arg_begin()->getType());
    Type *ElementTy = Ty->getElementType();
    return DAG.getDataLayout().getTypeAllocSize(ElementTy);
  }
  else
    return 0;
}

//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

/// IntCondCCodeToICC - Convert a DAG integer condition code to a UCPS ICC
/// condition.
static UCPS_CC::CondCodes
IntCondCodeToICC(ISD::CondCode CC)
{
	switch(CC) {
		default:
			llvm_unreachable("Unknown integer condition code!");
		case ISD::SETEQ:
			return UCPS_CC::ICC_E;
		case ISD::SETNE:
			return UCPS_CC::ICC_NE;
		case ISD::SETLT:
			return UCPS_CC::ICC_L;
		case ISD::SETGT:
			return UCPS_CC::ICC_G;
		case ISD::SETLE:
			return UCPS_CC::ICC_LE;
		case ISD::SETGE:
			return UCPS_CC::ICC_GE;
		case ISD::SETULT:
			return UCPS_CC::ICC_CS;
		case ISD::SETULE:
			return UCPS_CC::ICC_LEU;
		case ISD::SETUGT:
			return UCPS_CC::ICC_GU;
		case ISD::SETUGE:
			return UCPS_CC::ICC_CC;
	}
}

/// FPCondCodeToFCC - Convert a DAG floating-point condition code to a UCPS
/// FCC condition.
static UCPS_CC::CondCodes
FPCondCodeToFCC(ISD::CondCode CC)
{
	switch(CC) {
		default:
			llvm_unreachable("Unknown fp condition code!");
		case ISD::SETEQ:
		case ISD::SETOEQ:
			return UCPS_CC::FCC_E;
		case ISD::SETNE:
		case ISD::SETUNE:
			return UCPS_CC::FCC_NE;
		case ISD::SETLT:
		case ISD::SETOLT:
			return UCPS_CC::FCC_L;
		case ISD::SETGT:
		case ISD::SETOGT:
			return UCPS_CC::FCC_G;
		case ISD::SETLE:
		case ISD::SETOLE:
			return UCPS_CC::FCC_LE;
		case ISD::SETGE:
		case ISD::SETOGE:
			return UCPS_CC::FCC_GE;
		case ISD::SETULT:
			return UCPS_CC::FCC_UL;
		case ISD::SETULE:
			return UCPS_CC::FCC_ULE;
		case ISD::SETUGT:
			return UCPS_CC::FCC_UG;
		case ISD::SETUGE:
			return UCPS_CC::FCC_UGE;
		case ISD::SETUO:
			return UCPS_CC::FCC_U;
		case ISD::SETO:
			return UCPS_CC::FCC_O;
		case ISD::SETONE:
			return UCPS_CC::FCC_LG;
		case ISD::SETUEQ:
			return UCPS_CC::FCC_UE;
	}
}

UCPSTargetLowering::UCPSTargetLowering(const TargetMachine &TM, const UCPSSubtarget &STI)
				: TargetLowering(TM), Subtarget(STI)
{
	// Set up the register classes.
	addRegisterClass(MVT::i32, &UCPSReg::I32RegRegClass);
//addRegisterClass(MVT::i64, &UCPSReg::PtrRegRegClass);

	addRegisterClass(MVT::f32, &UCPSReg::F32RegRegClass);
	addRegisterClass(MVT::f64, &UCPSReg::F64RegRegClass);
  for (MVT VT : MVT::integer_valuetypes()) {
	setLoadExtAction( ISD::EXTLOAD, VT, MVT::i1, Custom);
  setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i1, Custom);
  setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i1, Custom);

	setLoadExtAction(ISD::EXTLOAD, VT, MVT::i8, Custom);
	setLoadExtAction(ISD::EXTLOAD, VT, MVT::i16, Custom);
	setLoadExtAction(ISD::EXTLOAD, VT, MVT::i64, Expand);
  }
  for (MVT VT : MVT::fp_valuetypes()) {
	setLoadExtAction(ISD::EXTLOAD, VT, MVT::f32, Expand);
	setLoadExtAction(ISD::EXTLOAD, VT, MVT::f64, Expand);
  }

	setTruncStoreAction(MVT::f64, MVT::f32, Expand);

  // VASTART needs to be custom lowered to use the VarArgsFrameIndex.
  setOperationAction(ISD::VASTART, MVT::Other, Custom);

  setOperationAction(ISD::VAEND, MVT::Other, Expand);

  // VAARG is defined at ABIInfo::EmitVAArg()
	setOperationAction(ISD::VAARG, MVT::Other, Expand);
	setOperationAction(ISD::VACOPY, MVT::Other, Expand);

	setOperationAction(ISD::BRIND, MVT::Other, Expand);
	setOperationAction(ISD::BRCOND, MVT::i64, Custom);

	// see DAGCombiner::visitBRCOND() in DAGCombiner.cpp
	// we have to set as follows to prevent further combining
	// setcc + BRCOND into br_cc.
  setOperationAction(ISD::BR_CC, MVT::i1, Expand);
  setOperationAction(ISD::BR_CC, MVT::i8, Expand);
  setOperationAction(ISD::BR_CC, MVT::i16, Expand);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::BR_CC, MVT::f32, Expand);
  setOperationAction(ISD::BR_CC, MVT::f64, Expand);
  setOperationAction(ISD::BR_CC, MVT::Other, Expand);

  // expand br to a jump table
  setOperationAction(ISD::BR_JT, MVT::Other, Expand);

  setCondCodeAction(ISD::SETO, MVT::f32, Custom);
  setCondCodeAction(ISD::SETUO, MVT::f32, Custom);

  setCondCodeAction(ISD::SETO, MVT::f64, Custom);
  setCondCodeAction(ISD::SETUO, MVT::f64, Custom);

  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::f32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::f64, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::Other, Expand);

  // libcall
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);

  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);


  setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);

  setOperationAction(ISD::BITCAST, MVT::f32, Expand);
  setOperationAction(ISD::BITCAST, MVT::i32, Expand);

  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ, MVT::i32, Expand);
  setOperationAction(ISD::CTLZ, MVT::i32, Expand);
  setOperationAction(ISD::CTPOP, MVT::i32, Expand);

  setOperationAction(ISD::CTLZ_ZERO_UNDEF, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ_ZERO_UNDEF, MVT::i32, Expand);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);

  setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);
  setOperationAction(ISD::FCOPYSIGN, MVT::f64, Expand);
  setOperationAction(ISD::FCOPYSIGN, MVT::i32, Expand);

  //setOperationAction(ISD::SELECT, MVT::i32, Custom);
  //setOperationAction(ISD::SELECT, MVT::f32, Custom);
  //setOperationAction(ISD::SELECT, MVT::f64, Custom);

  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
  setOperationAction(ISD::BlockAddress, MVT::i32, Custom);

  // constantfp  ->  ConstantPool  ->  TargetConstantPool
  setOperationAction(ISD::ConstantFP, MVT::f32, Custom);
  setOperationAction(ISD::ConstantFP, MVT::f64, Custom);

  setOperationAction(ISD::ConstantPool, MVT::f32, Custom);
  setOperationAction(ISD::ConstantPool, MVT::f64, Custom);
  setOperationAction(ISD::ConstantPool, MVT::i32, Custom);

  setOperationAction(ISD::FNEG, MVT::f32, Custom);
  setOperationAction(ISD::FNEG, MVT::f64, Custom);
  setOperationAction(ISD::FDIV, MVT::f32, Custom);
  setOperationAction(ISD::FDIV, MVT::f64, Custom);

  setOperationAction(ISD::MULHS, MVT::i32, Expand);
  setOperationAction(ISD::MULHU, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);

  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32, Custom);
  setOperationAction(ISD::ATOMIC_LOAD_ADD, MVT::i32, Expand);

  setOperationAction(ISD::ROTL, MVT::i32, Expand);
  setOperationAction(ISD::ROTR, MVT::i32, Expand);

	setStackPointerRegisterToSaveRestore(UCPSReg::J29);

	setMinFunctionAlignment(0); // 1 byte align

	setSchedulingPreference(Sched::VLIW /*Sched::Source*/);

	computeRegisterProperties(Subtarget.getRegisterInfo());
}

const char *
UCPSTargetLowering::getTargetNodeName(unsigned Opcode) const
{
	switch(Opcode) {
		default:
			return 0;
		case UCPS_ISD::CMPICC:
			return "UCPS_ISD::CMPICC";
		case UCPS_ISD::CMPFCC:
			return "UCPS_ISD::CMPFCC";
		case UCPS_ISD::BRICC:
			return "UCPS_ISD::BRICC";
		case UCPS_ISD::BRFCC:
			return "UCPS_ISD::BRFCC";
		case UCPS_ISD::SELECT_ICC:
			return "UCPS_ISD::SELECT_ICC";
		case UCPS_ISD::SELECT_FCC:
			return "UCPS_ISD::SELECT_FCC";
		case UCPS_ISD::Hi:
			return "UCPS_ISD::Hi";
		case UCPS_ISD::Lo:
			return "UCPS_ISD::Lo";
		case UCPS_ISD::FTOI:
			return "UCPS_ISD::FTOI";
		case UCPS_ISD::ITOF:
			return "UCPS_ISD::ITOF";
		case UCPS_ISD::CALL:
			return "UCPS_ISD::CALL";
		case UCPS_ISD::RET:
			return "UCPS_ISD::RET";
		case UCPS_ISD::GLOBAL_BASE_REG:
			return "UCPS_ISD::GLOBAL_BASE_REG";
		case UCPS_ISD::FLUSHW:
			return "UCPS_ISD::FLUSHW";
		case UCPS_ISD::XferAddr:
			return "UCPS_ISD::XferAddr";
    case UCPS_ISD::Recip:
      return "UCPS_ISD::Recip";
    case UCPS_ISD::ADJDYNALLOC:
      return "UCPS_ISD::ADJDYNALLOC";
	}
}

/// isMaskedValueZeroForTargetNode - Return true if 'Op & Mask' is known to
/// be zero. Op is expected to be a target specific node. Used by DAG
/// combiner.
void
UCPSTargetLowering::computeKnownBitsForTargetNode(const SDValue Op,
													APInt &KnownZero,
													APInt &KnownOne,
													const SelectionDAG &DAG,
													unsigned Depth) const
{

}

// Look at LHS/RHS/CC and see if they are a lowered setcc instruction.  If so
// set LHS/RHS and UCPS_CC to the LHS/RHS of the setcc and UCPS_CC to the condition.
static void
LookThroughSetCC(SDValue &LHS, SDValue &RHS, ISD::CondCode CC, unsigned &UCPS_CC)
{
	if(isa<ConstantSDNode>(RHS) && cast<ConstantSDNode>(RHS)->isNullValue() && CC == ISD::SETNE
					&& ((LHS.getOpcode() == UCPS_ISD::SELECT_ICC && LHS.getOperand(3).getOpcode() == UCPS_ISD::CMPICC)
									|| (LHS.getOpcode() == UCPS_ISD::SELECT_FCC
													&& LHS.getOperand(3).getOpcode() == UCPS_ISD::CMPFCC))
					&& isa<ConstantSDNode>(LHS.getOperand(0)) && isa<ConstantSDNode>(LHS.getOperand(1))
					&& cast<ConstantSDNode>(LHS.getOperand(0))->isOne()
					&& cast<ConstantSDNode>(LHS.getOperand(1))->isNullValue()) {
		SDValue CMPCC = LHS.getOperand(3);
		UCPS_CC = cast<ConstantSDNode>(LHS.getOperand(2))->getZExtValue();
		LHS = CMPCC.getOperand(0);
		RHS = CMPCC.getOperand(1);
	}
}

SDValue
UCPSTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
	  GlobalAddressSDNode * node = cast<GlobalAddressSDNode>(Op) ;
	  const GlobalValue *GV =  node->getGlobal();
	  int64_t Offset = node  ->getOffset();
	  SDLoc DL(node);
	  SDValue v= DAG.getTargetGlobalAddress(GV, DL, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()), Offset);
	  return DAG.getNode(UCPS_ISD::XferAddr, DL, DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()), v);
}

SDValue UCPSTargetLowering::LowerBlockAddress(SDValue Op,
                                             SelectionDAG &DAG) const {
  //MachineFunction &MF = DAG.getMachineFunction();
  SDLoc DL(Op);
  EVT PtrVT = DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout());
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
  int64_t Offset = cast<BlockAddressSDNode>(Op)->getOffset();
  //Reloc::Model RelocM = getTargetMachine().getRelocationModel();
  SDValue CPAddr;
  CPAddr = DAG.getTargetBlockAddress(BA, PtrVT, 4, Offset);
  SDValue Result = DAG.getNode(UCPS_ISD::XferAddr, DL, PtrVT, CPAddr);
  return Result;
}

SDValue
UCPSTargetLowering::LowerFNeg(SDValue Op, SelectionDAG &DAG) const
{
  EVT ValType   = Op.getValueType();
  APInt a;
  Type* t;
  unsigned align;
  LLVMContext &Context = *DAG.getContext();

  if(ValType == MVT::f32) {
    t = Type::getFloatTy(Context);
    a = APInt::floatToBits(float(0.0));
    align = 4;
  }
  else if(ValType == MVT::f64) {
    t = Type::getDoubleTy(Context);
    a = APInt::doubleToBits(double(0.0));
    align = 8;
  }
  else return Op;

  Constant * zero = Constant::getIntegerValue(t, a);
  EVT PtrType = DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout());
  SDValue ConstPool  = DAG.getTargetConstantPool(zero, PtrType, align);

  SDLoc dl (Op.getNode());

  ConstPool = DAG.getNode(UCPS_ISD::XferAddr, dl, PtrType, ConstPool);

  SDValue ConstValue = DAG.getLoad(ValType, dl, DAG.getEntryNode(), ConstPool,
                                   MachinePointerInfo::getConstantPool(DAG.getMachineFunction()), false,
                                   false, true,  align);

  return DAG.getNode(ISD::FSUB, dl, ValType, ConstValue, Op.getNode()->getOperand(0));
}

SDValue
UCPSTargetLowering::LowerFDiv(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op.getNode());
  EVT ValType   = Op.getValueType();
  SDValue op1 = DAG.getNode(UCPS_ISD::Recip, dl, ValType, Op.getOperand(1));

  return DAG.getNode(ISD::FMUL, dl, ValType, Op.getOperand(0), op1);
}

SDValue
UCPSTargetLowering::LowerConstantPool(SDValue Op, SelectionDAG &DAG) const
{
	ConstantPoolSDNode *CPNode = cast<ConstantPoolSDNode>(Op.getNode());
	SDLoc dl(Op.getNode());

  EVT PtrVT = CPNode->getValueType(0);;

  SDValue Pool;
  if (CPNode->isMachineConstantPoolEntry())
    Pool = DAG.getTargetConstantPool(CPNode->getMachineCPVal(), PtrVT,
               CPNode->getAlignment(), CPNode->getOffset());
  else
    Pool = DAG.getTargetConstantPool(CPNode->getConstVal(), PtrVT,
               CPNode->getAlignment(), CPNode->getOffset());

  // Use LARL to load the address of the constant pool entry.
  Pool = DAG.getNode(UCPS_ISD::XferAddr, dl, PtrVT, Pool);

  return Pool;
}

SDValue
UCPSTargetLowering::LowerConstantFP(SDValue Op, SelectionDAG &DAG) const
{
  EVT ValType = Op.getValueType();
  //PointerType* type;
  unsigned align;
  //LLVMContext &Context = *DAG.getContext();
  if(ValType == MVT::f32) {
    //type = Type::getFloatPtrTy(Context);
    align = 4;
  }
  else if(ValType == MVT::f64) {
    //type = Type::getDoublePtrTy(Context);
    align = 8;
  }
  else return Op;

  ConstantFPSDNode *src = cast<ConstantFPSDNode>(Op.getNode());
  const ConstantFP * fp = src->getConstantFPValue();
  SDLoc dl(Op.getNode());

  EVT ptrType = DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout());

  SDValue pool = DAG.getConstantPool(fp, ptrType, align);
  pool = DAG.getLoad(ValType, dl, DAG.getEntryNode(), pool,
                     MachinePointerInfo::getConstantPool(DAG.getMachineFunction()),
                     false,false,true, align);

  return pool;
}

static SDValue
LowerFP_TO_SINT(SDValue Op, SelectionDAG &DAG)
{
  SDLoc dl(Op.getNode());
	// Convert the fp value to integer in an FP register.
	assert(Op.getValueType() == MVT::i32);
	Op = DAG.getNode(UCPS_ISD::FTOI, dl, MVT::f32, Op.getOperand(0));
	return DAG.getNode(ISD::BITCAST, dl, MVT::i32, Op);
}

static SDValue
LowerI64Mul(SDValue Op, SelectionDAG &DAG)
{
  SDLoc dl(Op.getNode());
  // Convert the fp value to integer in an FP register.
	assert(Op.getValueType() == MVT::i64 && Op.getOpcode()==ISD::MUL);

	SDValue operand0 = Op.getOperand(0);
	SDValue operand1 = Op.getOperand(1);

	if(operand0.getOpcode()==ISD::LOAD) {
		operand0 = DAG.getLoad(MVT::i32, dl, operand0.getOperand(0),
							   operand0.getOperand(1), MachinePointerInfo(),
							   false, false, false, 0);
	}

	if(operand1.getOpcode()==ISD::Constant && isa<ConstantSDNode>(operand1)) {
		operand1 = DAG.getConstant(cast<ConstantSDNode>(operand1)->getZExtValue(), dl, MVT::i32, false);
	}

	Op = DAG.getNode(ISD::MUL, dl, MVT::i32, operand0, operand1);

	return DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i64, Op);
}

static SDValue
LowerSINT_TO_FP(SDValue Op, SelectionDAG &DAG)
{
  SDLoc dl(Op.getNode());
  assert(Op.getOperand(0).getValueType() == MVT::i32);
	SDValue Tmp = DAG.getNode(ISD::BITCAST, dl, MVT::f32, Op.getOperand(0));
	// Convert the int value to FP in an FP register.
	return DAG.getNode(UCPS_ISD::ITOF, dl, Op.getValueType(), Tmp);
}

static SDValue
LowerLoadExt(SDValue Op, SelectionDAG &DAG)
{
  LoadSDNode* load = cast<LoadSDNode>(Op.getNode());
  if(load) {
    ISD::LoadExtType extTy = load->getExtensionType();
    switch(extTy) {
      default:
        break;

      case ISD::EXTLOAD: {
        MemSDNode* node = cast<MemSDNode>(Op.getNode());
        EVT memType = node->getMemoryVT();
        assert(memType != MVT::i32);
        const MachinePointerInfo & ptrInfo = node->getPointerInfo();
        SDLoc dl(Op.getNode());
        return DAG.getExtLoad(ISD::ZEXTLOAD, dl, MVT::i32, node->getOperand(0),
                              node->getOperand(1), ptrInfo, memType, node->isVolatile(),
                              node->isNonTemporal(), node->getAlignment(), 0);
      }

      case ISD::SEXTLOAD:
      case ISD::ZEXTLOAD:
      {
        MemSDNode* node = cast<MemSDNode>(Op.getNode());
        EVT memType = node->getMemoryVT();
        assert(memType != MVT::i32);
        if(memType == MVT::i1)
        {
          memType = MVT::i8;
          const MachinePointerInfo & ptrInfo = node->getPointerInfo();
          SDLoc dl(Op.getNode());
          //SDVTList NodeTys = DAG.getVTList(MVT::i32, MVT::Other);
          return DAG.getExtLoad(extTy, dl, MVT::i32, node->getOperand(0),
                                node->getOperand(1), ptrInfo, memType,
                                node->isVolatile(), node->isNonTemporal(),
                                node->getAlignment(), 0);
        }

        break;
      }
    }
  }

  return Op;
}

static SDValue
LowerSETCC(SDValue Op, SelectionDAG &DAG)
{
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);

  // we only hadle:
  // 1. f == f
  // 2. f != f
  if(LHS.getNode()==RHS.getNode() && LHS.getResNo()==RHS.getResNo())
  {
    ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
    switch(CC) {

      case ISD::SETO:
        CC=ISD::SETOEQ; break;

      case ISD::SETUO:
        CC=ISD::SETUNE; break;

      default: llvm_unreachable("unexpected CondCode");
    }

    SDValue CCValue = DAG.getCondCode(CC);
    return DAG.getNode(ISD::SETCC, SDLoc(Op.getNode()), Op.getValueType(),
                       LHS, RHS, CCValue);
  }

  return Op;
}

static SDValue
LowerBR_CC(SDValue Op, SelectionDAG &DAG)
{
	SDValue Chain = Op.getOperand(0);
	ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
	SDValue LHS = Op.getOperand(2);
	SDValue RHS = Op.getOperand(3);
	SDValue Dest = Op.getOperand(4);
	SDLoc dl(Op.getNode());
	unsigned Opc, UCPS_CC = ~0U;

	// If this is a br_cc of a "setcc", and if the setcc got lowered into
	// an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
	LookThroughSetCC(LHS, RHS, CC, UCPS_CC);

	// Get the condition flag.
	SDValue CompareFlag;
	if(LHS.getValueType() == MVT::i32) {
		std::vector<EVT> VTs;
		VTs.push_back(MVT::i32);
		VTs.push_back(MVT::Glue);
		SDValue Ops[2] = {LHS, RHS};
		CompareFlag = DAG.getNode(UCPS_ISD::CMPICC, dl, VTs, Ops).getValue(1);
		if(UCPS_CC == ~0U)
			UCPS_CC = IntCondCodeToICC(CC);
		Opc = UCPS_ISD::BRICC;
	}
	else {
		CompareFlag = DAG.getNode(UCPS_ISD::CMPFCC, dl, MVT::Glue, LHS, RHS);
		if(UCPS_CC == ~0U)
			UCPS_CC = FPCondCodeToFCC(CC);
		Opc = UCPS_ISD::BRFCC;
	}
	return DAG.getNode(Opc, dl, MVT::Other, Chain, Dest, DAG.getConstant(UCPS_CC, dl, MVT::i32), CompareFlag);
}

/*
SDValue UCPSTargetLowering::LowerFrameIndex(SDValue Op, SelectionDAG &DAG) const
{
  FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Op);
  assert(FIN && "should be FrameIndexSDNode");
  unsigned FrameIndex = FIN->getIndex();

  const UCPSFrameLowering *TFL =
   static_cast<const UCPSFrameLowering*>(getTargetMachine().getFrameLowering());
  MachineFunction &MF = DAG.getMachineFunction();
  unsigned Offset = TFL->getFrameIndexOffset(MF, FrameIndex);

  SDValue fp = DAG.getRegister(UCPSReg::J28, MVT::i32);

  SDValue c = DAG.getConstant(Offset, MVT::i32);
  SDLoc dl(Op.getNode());
  return DAG.getNode(ISD::ADD, dl, MVT::i32, fp, c);
}
*/

static SDValue
LowerSELECT_CC(SDValue Op, SelectionDAG &DAG)
{
	SDValue LHS = Op.getOperand(0);
	SDValue RHS = Op.getOperand(1);
	ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
	SDValue TrueVal = Op.getOperand(2);
	SDValue FalseVal = Op.getOperand(3);
	SDLoc dl(Op.getNode());
	unsigned Opc, UCPS_CC = ~0U;

	// If this is a select_cc of a "setcc", and if the setcc got lowered into
	// an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
	LookThroughSetCC(LHS, RHS, CC, UCPS_CC);

	SDValue CompareFlag;
	if(LHS.getValueType() == MVT::i32) {
		std::vector<EVT> VTs;
		VTs.push_back(LHS.getValueType());    // subcc returns a value
		VTs.push_back(MVT::Glue);
		SDValue Ops[2] = {LHS, RHS};
		CompareFlag = DAG.getNode(UCPS_ISD::CMPICC, dl, VTs, Ops).getValue(1);
		Opc = UCPS_ISD::SELECT_ICC;
		if(UCPS_CC == ~0U)
			UCPS_CC = IntCondCodeToICC(CC);
	}
	else {
		CompareFlag = DAG.getNode(UCPS_ISD::CMPFCC, dl, MVT::Glue, LHS, RHS);
		Opc = UCPS_ISD::SELECT_FCC;
		if(UCPS_CC == ~0U)
			UCPS_CC = FPCondCodeToFCC(CC);
	}
	return DAG.getNode(Opc, dl, TrueVal.getValueType(), TrueVal, FalseVal, DAG.getConstant(UCPS_CC, dl, MVT::i32),
						CompareFlag);
}

static void
ReplaceSETCCResult(SDNode * N,
                   SmallVectorImpl<SDValue> &Results,
                   SelectionDAG &DAG)
{
  assert(N->getOpcode() == ISD::SETCC);
  assert(N->getValueType(0) != MVT::i32);

  SDValue SDV = DAG.getNode(ISD::SETCC, SDLoc(N), MVT::i32, N->getOperand(0), N->getOperand(1), N->getOperand(2));
  Results.push_back(SDV);
}

SDValue
UCPSTargetLowering::LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  SDValue Size = Op.getOperand(1);
  SDLoc dl(Op.getNode());

  unsigned SPReg = UCPSReg::J29;
  MVT PtrType = DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout());

  // Get a reference to the stack pointer.
  SDValue StackPointer = DAG.getCopyFromReg(Chain, dl, SPReg, PtrType);

  // Subtract the dynamic size from the actual stack size to
  // obtain the new stack size.
  SDValue NewTop = DAG.getNode(ISD::SUB, dl, PtrType, StackPointer, Size);

  // For UCPS, the outgoing memory arguments area should be on top of the
  // alloca area on the stack i.e., the outgoing memory arguments should be
  // at a lower address than the alloca area. Move the alloca area down the
  // stack by adding back the space reserved for outgoing arguments to SP
  // here.
  //
  // We do not know what the size of the outgoing args is at this point.
  // So, we add a pseudo instruction ADJDYNALLOC that will adjust the
  // stack pointer. We patch this instruction with the correct, known
  // offset in emitPrologue().
  //
  // Use a placeholder immediate (zero) for now. This will be patched up
  // by emitPrologue().
  SDValue ArgAdjust = DAG.getNode(UCPS_ISD::ADJDYNALLOC, dl, PtrType, NewTop,
                                  DAG.getTargetConstant(0, dl, PtrType));

  // The Sub result contains the new stack start address, so it
  // must be placed in the stack pointer register.
  SDValue CopyChain = DAG.getCopyToReg(Chain, dl, SPReg, NewTop);

  SDValue Ops[2] = { ArgAdjust, CopyChain };
  return DAG.getMergeValues(Ops, dl);
}

static SDValue
getFLUSHW(SDValue Op, SelectionDAG &DAG)
{
  SDLoc dl(Op.getNode());
  SDValue Chain = DAG.getNode(UCPS_ISD::FLUSHW, dl, MVT::Other, DAG.getEntryNode());
	return Chain;
}

static SDValue
LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG)
{
	MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
	MFI->setFrameAddressIsTaken(true);

	EVT VT = Op.getValueType();
	SDLoc dl(Op.getNode());
	unsigned FrameReg = UCPSReg::J28;

	uint64_t depth = Op.getConstantOperandVal(0);

	SDValue FrameAddr;
	if(depth == 0)
		FrameAddr = DAG.getCopyFromReg(DAG.getEntryNode(), dl, FrameReg, VT);
	else {
		// flush first to make sure the windowed registers' values are in stack
		SDValue Chain = getFLUSHW(Op, DAG);
		FrameAddr = DAG.getCopyFromReg(Chain, dl, FrameReg, VT);

		for(uint64_t i = 0; i != depth; ++i) {
			SDValue Ptr = DAG.getNode(ISD::ADD, dl, MVT::i32, FrameAddr, DAG.getIntPtrConstant(56, dl));
			FrameAddr = DAG.getLoad(MVT::i32, dl, Chain, Ptr, MachinePointerInfo(), false, false, false, 0);
		}
	}
	return FrameAddr;
}

static SDValue
LowerRETURNADDR(SDValue Op, SelectionDAG &DAG)
{
	MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
	MFI->setReturnAddressIsTaken(true);

	EVT VT = Op.getValueType();
	SDLoc dl(Op.getNode());
	unsigned RetReg = UCPSReg::J30;

	uint64_t depth = Op.getConstantOperandVal(0);

	SDValue RetAddr;
	if(depth == 0)
		RetAddr = DAG.getCopyFromReg(DAG.getEntryNode(), dl, RetReg, VT);
	else {
		// flush first to make sure the windowed registers' values are in stack
		SDValue Chain = getFLUSHW(Op, DAG);
		RetAddr = DAG.getCopyFromReg(Chain, dl, UCPSReg::J28, VT);

		for(uint64_t i = 0; i != depth; ++i) {
			SDValue Ptr = DAG.getNode(ISD::ADD, dl, MVT::i64, RetAddr,
										DAG.getIntPtrConstant((i == depth - 1)? 60:56, dl));
			RetAddr = DAG.getLoad(MVT::i64, dl, Chain, Ptr, MachinePointerInfo(), false, false, false, 0);
		}
	}
	return RetAddr;
}

void UCPSTargetLowering::ReplaceNodeResults(SDNode * N,
                                SmallVectorImpl<SDValue> &Results,
                                SelectionDAG &DAG) const
{
  switch(N->getOpcode()) {
    default:
          llvm_unreachable("Should not replace result for this node!");

    case ISD::SETCC:
      ReplaceSETCCResult(N, Results, DAG); return;
  }
}

SDValue
UCPSTargetLowering::LowerSelect(SDValue Op, SelectionDAG &DAG) const
{
  // stub implementation
  return Op;
}

SDValue
UCPSTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
	switch(Op.getOpcode()) {
		default: {
		  std::string name("should not custom lower SDNode: ");
		  name += Op.getNode()->getOperationName(&DAG);
		  report_fatal_error(name);
		}

		case ISD::RETURNADDR:
			return LowerRETURNADDR(Op, DAG);
		case ISD::FRAMEADDR:
			return LowerFRAMEADDR(Op, DAG);
		case ISD::GlobalTLSAddress:
			llvm_unreachable("TLS not implemented for UCPS.");
		case ISD::GlobalAddress:
			return LowerGlobalAddress(Op, DAG);
    case ISD::BlockAddress:
      return LowerBlockAddress(Op, DAG);
		case ISD::ConstantPool:
			return LowerConstantPool(Op, DAG);
    case ISD::ConstantFP:
      return LowerConstantFP(Op, DAG);
		case ISD::FP_TO_SINT:
			return LowerFP_TO_SINT(Op, DAG);
		case ISD::SINT_TO_FP:
			return LowerSINT_TO_FP(Op, DAG);
		case ISD::BR_CC: // i32, f32, f64
			return LowerBR_CC(Op, DAG);
		case ISD::SELECT_CC:
			return LowerSELECT_CC(Op, DAG);
    case ISD::SETCC:
      return LowerSETCC(Op, DAG);
		case ISD::VASTART:
			return LowerVASTART(Op, DAG);
		case ISD::VAARG:
			return LowerVAARG(Op, DAG);
		case ISD::DYNAMIC_STACKALLOC:
			return LowerDYNAMIC_STACKALLOC(Op, DAG);
		case ISD::MUL:
			return LowerI64Mul(Op, DAG);
    case ISD::LOAD:
      return LowerLoadExt(Op, DAG);
    case ISD::FNEG:
      return LowerFNeg(Op, DAG);
    case ISD::FDIV:
      return LowerFDiv(Op, DAG);
	}
}

MachineBasicBlock *
UCPSTargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI, MachineBasicBlock *MBB) const
{
	const TargetInstrInfo *TII = Subtarget.getInstrInfo();
	MachineFunction *MF = MBB->getParent();
	//MachineRegisterInfo &RegInfo = MF->getRegInfo();
	DebugLoc DL       = MI->getDebugLoc();

	switch(MI->getOpcode()) {

/*
	  case UCPSInst::StackAddr: // (i32 (stackaddr target-frame-index, target-constant))
	  {
	    MachineOperand & MO0 = MI->getOperand(0); // output

	    MachineOperand & MO1 = MI->getOperand(1); // frame index
	    MachineOperand & MO2 = MI->getOperand(2); // offset

	    // bad news is: we can not resolve frame index offset here.
	    // thus we will handle this in eliminateFrameIndex()
		  int index = MF->getFrameInfo()->getObjectOffset(MO1.getIndex());
		  int offset = MO2.getImm();

      unsigned opcode = UCPSInst::AssignPtr;
      unsigned vr = RegInfo.createVirtualRegister(&UCPSReg::PtrRegRegClass);
      BuildMI(*MBB, MI, DL, TII.get(opcode), vr).addImm(index+offset);

		  opcode = UCPSInst::PtrAdd;
		  BuildMI(*MBB, MI, DL, TII.get(opcode), MO0.getReg()).addReg(UCPSReg::J28).addReg(vr);

		  MI->eraseFromParent();
		  break;
		}
*/

    case UCPSInst::AdjDynAlloc: {
      MachineFunction *MF = MBB->getParent();
      UCPSMachineFunctionInfo *FnInfo = MF->getInfo<UCPSMachineFunctionInfo>();
      FnInfo->addAllocaAdjustInst(MI);
      break;
    }

    case UCPSInst::SelectI32:
    case UCPSInst::SelectF32:
    case UCPSInst::SelectF64: {
      // FIXME: we should split original llvm bb?
      const BasicBlock *BB = MBB->getBasicBlock();

      MachineBasicBlock* MBBFalse = MF->CreateMachineBasicBlock(BB);
      MachineBasicBlock* MBBTrueAndEnd = MF->CreateMachineBasicBlock(BB);

      MachineFunction::iterator MBBI = MBB->getIterator();
      MachineBasicBlock::iterator MII= MI;
      ++MBBI; ++MII;
      MF->insert(MBBI, MBBFalse);
      MF->insert(MBBI, MBBTrueAndEnd);

      MachineOperand & Res = MI->getOperand(0);
      MachineOperand & Cn = MI->getOperand(1);
      MachineOperand & V1 = MI->getOperand(2);
      MachineOperand & V2 = MI->getOperand(3);


      // choose the first value as default.
      unsigned opcode;
      /*opcode = UCPSInst::I32RegFromI32Reg;
      unsigned TrueVal = RegInfo.createVirtualRegister(&UCPSReg::I32RegRegClass);
      BuildMI(*MBB, MI, DL, TII.get(opcode), TrueVal).addOperand(V1);*/
      opcode = UCPSInst::JumpBasicBlockCond;
      BuildMI(*MBB, MI, DL, TII->get(opcode)).addOperand(Cn).addMBB(MBBTrueAndEnd);

      // fall through to FalseBB and choose the second value.
      /*opcode = UCPSInst::I32RegFromI32Reg;
      unsigned FalseVal = RegInfo.createVirtualRegister(&UCPSReg::I32RegRegClass);
      BuildMI(MBBFalse, DL, TII.get(opcode), FalseVal).addOperand(V2);*/

      // split original basic block holding select instruction
      MBBTrueAndEnd->splice(MBBTrueAndEnd->begin(), MBB, MII, MBB->end());
      MBBTrueAndEnd->transferSuccessorsAndUpdatePHIs(MBB);
      MBB->addSuccessor(MBBFalse);
      MBB->addSuccessor(MBBTrueAndEnd);
      MBBFalse->addSuccessor(MBBTrueAndEnd);

      // ordering each MBB
      /*for(MachineBasicBlock::succ_iterator si = MBB->succ_begin(); si!=MBB->succ_end();++si ) {
        MBBEnd->addSuccessor(*si);
        MBB->removeSuccessor(si);
      }*/

      opcode = TargetOpcode::PHI;
      BuildMI(*MBBTrueAndEnd, MBBTrueAndEnd->begin(), DL, TII->get(opcode), Res.getReg()).
            addReg(V1.getReg()).addMBB(MBB).addReg(V2.getReg()).addMBB(MBBFalse);

      MI->eraseFromParent();

      MBB = MBBTrueAndEnd; // update return value.
      break;
		}

    default:
      llvm_unreachable("Illegal Custom Insertion!");
	}

	return MBB;
}

//===----------------------------------------------------------------------===//
//                         UCPS Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
TargetLowering::ConstraintType
UCPSTargetLowering::getConstraintType(StringRef Constraint) const
{
	if(Constraint.size() == 1) {
		switch(Constraint[0]) {
			default:
				break;
			case 'r':
				return C_RegisterClass;
		}
	}

	return TargetLowering::getConstraintType(Constraint);
}

std::pair<unsigned, const TargetRegisterClass*>
UCPSTargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                 StringRef Constraint, MVT VT) const
{
	if(Constraint.size() == 1) {
		switch(Constraint[0]) {
			case 'r':
				return std::make_pair(0U, &UCPSReg::I32RegRegClass);
		}
	}

	return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

bool
UCPSTargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const
{
	// The UCPS target isn't yet aware of offsets.
	return false;
}


// VASTART: 0. input chain, 1. pointer, 2. SRCVALUE.
SDValue
UCPSTargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const
{
  // VASTART stores the address of the VarArgsFrameIndex slot into the
  // memory location argument.
  MachineFunction &MF = DAG.getMachineFunction();
  UCPSMachineFunctionInfo *FnInfo = MF.getInfo<UCPSMachineFunctionInfo>();
  SDValue ArgAddr = DAG.getFrameIndex(FnInfo->getVarArgsFrameIndex(),
                                      DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout()));

  const Value *SV = (cast<SrcValueSDNode>(Op.getOperand(2).getNode()))->getValue();
  return DAG.getStore(Op.getOperand(0)/*chain*/, SDLoc(Op.getNode()), ArgAddr/*val*/,
                      Op.getOperand(1)/*addr*/, MachinePointerInfo(SV), false,
                      false, 0);
}

// type a = va_arg(arg_list, type);
/// VAARG has four operands: an input chain, a pointer, a SRCVALUE,
/// and the alignment. It returns a pair of values: the vaarg value and a
/// new chain.
SDValue
UCPSTargetLowering::LowerVAARG(SDValue Op, SelectionDAG &DAG) const
{
  SDNode *Node = Op.getNode();
  SDValue InChain = Node->getOperand(0);
  SDValue VAListPtr = Node->getOperand(1); // point to va_list, which holds a stack address

  // arg type info
  const Value *SV = cast<SrcValueSDNode>(Node->getOperand(2))->getValue();

  EVT VT = Node->getValueType(0); // type of output value for VAARG
  SDLoc DL(Op.getNode());
  EVT PtrVT = DAG.getTargetLoweringInfo().getPointerTy(DAG.getDataLayout());

  // obtain arg address.
  SDValue VAList = DAG.getLoad(PtrVT, DL, InChain, VAListPtr,
                               MachinePointerInfo(SV), false, false, false, 0/* code gen */);

  unsigned dataSize = VT.getSizeInBits() / 8;
  uint64_t argSize = (dataSize+7) & (~0x7); // align to 8

  // Increment the pointer, VAList, to the next variable arg.
  SDValue NextVAList = DAG.getNode(ISD::ADD, DL, PtrVT, VAList,
                                DAG.getIntPtrConstant(argSize, DL));

  // Store the incremented VAList to the legalized pointer.
  InChain = DAG.getStore(VAList.getValue(1), DL, NextVAList,
                         VAListPtr, MachinePointerInfo(SV), false, false, 0);

  // Load the actual argument out of the pointer VAList.
  // We can't count on greater alignment than the word size.
  return DAG.getLoad(VT, DL, InChain, VAList, MachinePointerInfo(),
                     false, false, false, 0);
}
