// This is the header file for slib ChanEst_LS
#ifndef ChanEst_LS_H
#define ChanEst_LS_H
// Modify the parameter definition
// typedef struct {
// } ChanEst_LS_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef ChanEst_LS
#  undef ChanEst_LS
#  endif
#  define ChanEst_LS(param, coreid)\
  apcapi_core("ChanEst_LS", param, coreid)
#endif

