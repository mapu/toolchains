//===-- MSPUCallingConv.td - Calling Conventions MSPU ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the MSPU architectures.
//
//===----------------------------------------------------------------------===//

// MSPU calling convention.

// MSPU Calling convention for fixed arg list.
def MSPUPromoteTypeTmpl : CallingConv<[

  	// when passing args, value type and location type are distinguished.
  	// this only promotes location types for arg passing.
  	// for example, we pass i8 as i32, and thus it takes an i32 register or
  	// 4 bytes in stack to be passed.
  	CCIfType< [i1, i8, i16], CCPromoteToType<i32> >
]>;

// sub-calling convention
def MSPUPassMetaType : CallingConv<[

  CCIfType< [i32, f32], CCAssignToReg<[R0, R1, R2, R3, R4, R5, R6, R7]> >,
  CCIfType< [f64],      CCAssignToReg<[D0, D1, D2, D3]> >,

  // when registers run out.
  // Alternatively, they are assigned to the stack.
  /// If size is zero then the ABI size is used,
  /// if align is zero then the ABI alignment is used.
  CCIfType< [i32, f32], CCAssignToStack</*size*/ 0, /*align*/ 0> >,
  CCIfType< [     f64], CCAssignToStack</*size*/ 0, /*align*/ 0> >
]>;

// MSPU Calling convention for fixed arg list.
def MSPUPassFixedArg : CallingConv<[
	// by-value way uses its size from Flags, meanwhile, to-stack way uses its data type.
	// for struct object, its data type is usually pointer. We pass its address and a copy via stack.
    //CCIfByVal< CCPassByVal</*size*/ 0, /*align*/ 0> >,

	CCDelegateTo<MSPUPassMetaType>
]>;

// MSPU Calling convention template for var arg list.
// We need to explicitly upgrade its size to at least 4.
def MSPUPassVarArg : CallingConv<[
	// by-value way uses its size from Flags, meanwhile, to-stack way gets size from its data type.
	// for struct object, its data type is usually pointer.
    CCIfByVal< CCPassByVal</*size*/ 0, /*align*/ 0> >,

    CCAssignToStack</*size*/ 0, /*align*/ 0>
]>;

// all callee-saved registers live across a function call instruction.
def MSPU_CSRs : CalleeSavedRegs<(add (sequence "R%u", 16, 30),
			  						 (sequence "J%u", 16, 27),
									 (sequence "R%u", 8, 14))>;
