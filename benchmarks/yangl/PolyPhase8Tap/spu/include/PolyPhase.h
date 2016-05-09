// This is the header file for slib PolyPhase
#ifndef PolyPhase_H
#define PolyPhase_H
// Modify the parameter definition
// typedef struct {
// } PolyPhase_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef PolyPhase
#  undef PolyPhase
#  endif
#  define PolyPhase(param, coreid)\
  apcapi_core("PolyPhase", param, coreid)
#endif

