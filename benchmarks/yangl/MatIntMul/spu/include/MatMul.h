// This is the header file for slib MatMul
#ifndef MatMul_H
#define MatMul_H
// Modify the parameter definition
// typedef struct {
// } MatMul_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef MatMul
#  undef MatMul
#  endif
#  define MatMul(param, coreid)\
  apcapi_core("MatMul", param, coreid)
#endif

