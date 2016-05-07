// This is the header file for slib FFTFloat2048
#ifndef FFTFloat2048_H
#define FFTFloat2048_H
// Modify the parameter definition
// typedef struct {
// } FFTFloat2048_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFloat2048
#  undef FFTFloat2048
#  endif
#  define FFTFloat2048(param, coreid)\
  apcapi_core("FFTFloat2048", param, coreid)
#endif

