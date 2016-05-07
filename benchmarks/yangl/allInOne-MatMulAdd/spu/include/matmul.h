// This is the header file for slib matmul
#ifndef matmul_H
#define matmul_H
// Modify the parameter definition
// typedef struct {
// } matmul_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef matmul
#  undef matmul
#  endif
#  define matmul(param, coreid)\
  apcapi_core("matmul", param, coreid)
#endif

