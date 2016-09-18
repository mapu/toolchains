//==- IntrinsicsUCPS.td - UCPS intrinsics                 -*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the UCPS-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "ucps" in {  // All intrinsics start with "llvm.ucps.".

  /////////////////////////////////////////////////////////////////////
  def int_ucps_setKB: GCCBuiltin<"__ucps_setKB">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKB: GCCBuiltin<"__ucps_getKB">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setKE: GCCBuiltin<"__ucps_setKE">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKE: GCCBuiltin<"__ucps_getKE">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setKS: GCCBuiltin<"__ucps_setKS">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKS: GCCBuiltin<"__ucps_getKS">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setKI: GCCBuiltin<"__ucps_setKI">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKI: GCCBuiltin<"__ucps_getKI">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setKM: GCCBuiltin<"__ucps_setKM">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKM: GCCBuiltin<"__ucps_getKM">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setHiKM: GCCBuiltin<"__ucps_setHiKM">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getHiKM: GCCBuiltin<"__ucps_getHiKM">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setKG: GCCBuiltin<"__ucps_setKG">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKG: GCCBuiltin<"__ucps_getKG">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setKL: GCCBuiltin<"__ucps_setKL">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getKL: GCCBuiltin<"__ucps_getKL">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_ucps_getCStat: GCCBuiltin<"__ucps_getCStat">,
  						Intrinsic <[llvm_i32_ty], [], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_ucps_setLPCtrl: GCCBuiltin<"__ucps_setLPCtrl">,
  						Intrinsic <[], [llvm_i32_ty], []>;

  def int_ucps_getLPCtrl: GCCBuiltin<"__ucps_getLPCtrl">,
  						Intrinsic <[llvm_i32_ty], [], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_ucps_setMC: GCCBuiltin<"__ucps_setMC">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getMC: GCCBuiltin<"__ucps_getMC">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  /////////////////////////////////////////////////////////////////////
  def int_ucps_setChIndr: GCCBuiltin<"__ucps_setChIndr">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getChIndr: GCCBuiltin<"__ucps_getChIndr">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], []>;

  /////////////////////////
  def int_ucps_setCh: GCCBuiltin<"__ucps_setCh">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getCh: GCCBuiltin<"__ucps_getCh">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], []>;

  ////////////////////////
  def int_ucps_getChNum: GCCBuiltin<"__ucps_getChNum">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_getChNumIndr: GCCBuiltin<"__ucps_getChNumIndr">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem]>;

  ////////////////////////
  def int_ucps_callm: GCCBuiltin<"__ucps_callm">,
  						Intrinsic <[], [LLVMPointerType<llvm_i32_ty>], []>;

  def int_ucps_callmb: GCCBuiltin<"__ucps_callmb">,
  						Intrinsic <[], [LLVMPointerType<llvm_i32_ty>], []>;

  ////////////////////////
  def int_ucps_syscall: GCCBuiltin<"__ucps_syscall">,
  						Intrinsic <[], [llvm_i32_ty], []>;

  //////////// SVR access ////////////
  def int_ucps_setSVR: GCCBuiltin<"__ucps_setSVR">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getSVR: GCCBuiltin<"__ucps_getSVR">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrReadMem]>;

  /////////// SHUx.Tn /////////////
  def int_ucps_setSHU: GCCBuiltin<"__ucps_setSHU">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_getSHU: GCCBuiltin<"__ucps_getSHU">,
  						Intrinsic <[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setSHU64: GCCBuiltin<"__ucps_setSHU64">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_double_ty], []>;

  def int_ucps_getSHU64: GCCBuiltin<"__ucps_getSHU64">,
  						Intrinsic <[llvm_double_ty], [llvm_i32_ty, llvm_i32_ty], [IntrReadMem]>;

  def int_ucps_setSHUWithSVR: GCCBuiltin<"__ucps_setSHUWithSVR">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_setSVRWithSHU: GCCBuiltin<"__ucps_setSVRWithSHU">,
  						Intrinsic <[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], []>;

  def int_ucps_atomicld: GCCBuiltin<"__ucps_atomicld">,
  						Intrinsic <[llvm_i32_ty], [LLVMPointerType<llvm_i32_ty>, llvm_i32_ty], []>;
  
  def int_ucps_atomicst: GCCBuiltin<"__ucps_atomicst">,
  						Intrinsic <[llvm_i32_ty], [LLVMPointerType<llvm_i32_ty>, llvm_i32_ty, llvm_i32_ty], []>;
  
  def int_ucps_loop: GCCBuiltin<"__ucps_loop">,
              Intrinsic <[], [LLVMPointerType<llvm_i32_ty>, llvm_i32_ty], []>;
              
  def int_ucps_delay: GCCBuiltin<"__ucps_delay">,
              Intrinsic <[], [], []>;              
}
