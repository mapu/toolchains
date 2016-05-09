// This is the header file for slib ChanEst_MMSE
#ifndef ChanEst_MMSE_H
#define ChanEst_MMSE_H
// Modify the parameter definition
// typedef struct {
// } ChanEst_MMSE_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef ChanEst_MMSE
#  undef ChanEst_MMSE
#  endif
#  define ChanEst_MMSE(param, coreid)\
  apcapi_core("ChanEst_MMSE", param, coreid)
#endif

