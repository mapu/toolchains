// This is the header file for slib MatSFMul_spu
#ifndef MatSFMul_spu_H
#define MatSFMul_spu_H
// Modify the parameter definition
// typedef struct {
// } MatSFMul_spu_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef MatSFMul_spu
#  undef MatSFMul_spu
#  endif
#  define MatSFMul_spu(param, coreid)\
  apcapi_core("MatSFMul_spu", param, coreid)
#endif

