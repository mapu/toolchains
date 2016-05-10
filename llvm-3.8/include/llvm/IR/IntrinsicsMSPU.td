//==- IntrinsicsMSPU.td - MSPU intrinsics                 -*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the MSPU-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "mspu" in {  // All intrinsics start with "llvm.mspu.".

  /////////////////////////////////////////////////////////////////////
  def int_mspu_setKB: GCCBuiltin<"__mspu_setKB">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKB: GCCBuiltin<"__mspu_getKB">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setKE: GCCBuiltin<"__mspu_setKE">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKE: GCCBuiltin<"__mspu_getKE">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setKS: GCCBuiltin<"__mspu_setKS">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKS: GCCBuiltin<"__mspu_getKS">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setKI: GCCBuiltin<"__mspu_setKI">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKI: GCCBuiltin<"__mspu_getKI">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setKM: GCCBuiltin<"__mspu_setKM">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKM: GCCBuiltin<"__mspu_getKM">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setHiKM: GCCBuiltin<"__mspu_setHiKM">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getHiKM: GCCBuiltin<"__mspu_getHiKM">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setKG: GCCBuiltin<"__mspu_setKG">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKG: GCCBuiltin<"__mspu_getKG">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setKL: GCCBuiltin<"__mspu_setKL">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getKL: GCCBuiltin<"__mspu_getKL">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_mspu_getCStat: GCCBuiltin<"__mspu_getCStat">,
  						Intrinsic <[llvm_i32_ty], [], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_mspu_setLPCtrl: GCCBuiltin<"__mspu_setLPCtrl">,
  						Intrinsic <[], [llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getLPCtrl: GCCBuiltin<"__mspu_getLPCtrl">,
  						Intrinsic <[llvm_i32_ty], [], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_mspu_setMC: GCCBuiltin<"__mspu_setMC">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getMC: GCCBuiltin<"__mspu_getMC">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_mspu_setChIndr: GCCBuiltin<"__mspu_setChIndr">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getChIndr: GCCBuiltin<"__mspu_getChIndr">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  /////////////////////////
  def int_mspu_setCh: GCCBuiltin<"__mspu_setCh">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getCh: GCCBuiltin<"__mspu_getCh">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  ////////////////////////
  def int_mspu_getChNum: GCCBuiltin<"__mspu_getChNum">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_getChNumIndr: GCCBuiltin<"__mspu_getChNumIndr">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  ////////////////////////
  def int_mspu_callm: GCCBuiltin<"__mspu_callm">,
  						Intrinsic <[], [LLVMPointerType<llvm_i32_ty>], []>;

  def int_mspu_callmb: GCCBuiltin<"__mspu_callmb">,
  						Intrinsic <[], [LLVMPointerType<llvm_i32_ty>], []>;

  ////////////////////////
  def int_mspu_syscall: GCCBuiltin<"__mspu_syscall">,
  						Intrinsic <[], [llvm_i32_ty], []>;

  //////////// SVR access ////////////
  def int_mspu_setSVR: GCCBuiltin<"__mspu_setSVR">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getSVR: GCCBuiltin<"__mspu_getSVR">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrReadMem]>;

  /////////// SHUx.Tn /////////////
  def int_mspu_setSHU: GCCBuiltin<"__mspu_setSHU">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_getSHU: GCCBuiltin<"__mspu_getSHU">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setSHU64: GCCBuiltin<"__mspu_setSHU64">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_double_ty], [IntrWriteMem]>;

  def int_mspu_getSHU64: GCCBuiltin<"__mspu_getSHU64">,
  						Intrinsic <[llvm_double_ty], [llvm_i32_ty, llvm_i32_ty], [IntrReadMem]>;

  def int_mspu_setSHUWithSVR: GCCBuiltin<"__mspu_setSHUWithSVR">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_setSVRWithSHU: GCCBuiltin<"__mspu_setSVRWithSHU">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;

  def int_mspu_atomicld: GCCBuiltin<"__mspu_atomicld">,
  						Intrinsic <[llvm_i32_ty], [LLVMPointerType<llvm_i32_ty>, llvm_i32_ty], [IntrReadMem]>;
  
  def int_mspu_atomicst: GCCBuiltin<"__mspu_atomicst">,
  						Intrinsic <[llvm_i32_ty], [LLVMPointerType<llvm_i32_ty>, llvm_i32_ty, llvm_i32_ty], [IntrWriteMem]>;
  
  def int_mspu_loop: GCCBuiltin<"__mspu_loop">,
              Intrinsic <[], [LLVMPointerType<llvm_i32_ty>, llvm_i32_ty], []>;
              
  def int_mspu_delay: GCCBuiltin<"__mspu_delay">,
              Intrinsic <[], [], []>;              
}
