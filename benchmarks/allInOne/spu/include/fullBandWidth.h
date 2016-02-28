// This is the header file for slib fullBandWidth
#ifndef fullBandWidth_H
#define fullBandWidth_H
// Modify the parameter definition
// typedef struct {
// } fullBandWidth_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef fullBandWidth
#  undef fullBandWidth
#  endif
#  define fullBandWidth(param, coreid)\
  apcapi_core("fullBandWidth", param, coreid)
#endif

