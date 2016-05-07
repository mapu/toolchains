// This is the header file for slib FIRFix
#ifndef FIRFix_H
#define FIRFix_H
// Modify the parameter definition
// typedef struct {
// } FIRFix_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FIRFix
#  undef FIRFix
#  endif
#  define FIRFix(param, coreid)\
  apcapi_core("FIRFix", param, coreid)
#endif

