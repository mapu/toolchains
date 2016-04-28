// This is the header file for slib PolyFull
#ifndef PolyFull_H
#define PolyFull_H
// Modify the parameter definition
// typedef struct {
// } PolyFull_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef PolyFull
#  undef PolyFull
#  endif
#  define PolyFull(param, coreid)\
  apcapi_core("PolyFull", param, coreid)
#endif

