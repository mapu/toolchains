// This is the header file for slib freqShift
#ifndef freqShift_H
#define freqShift_H
// Modify the parameter definition
// typedef struct {
// } freqShift_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef freqShift
#  undef freqShift
#  endif
#  define freqShift(param, coreid)\
  apcapi_core("freqShift", param, coreid)
#endif

