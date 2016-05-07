// This is the header file for slib FFTFloat1024
#ifndef FFTFloat1024_H
#define FFTFloat1024_H
// Modify the parameter definition
// typedef struct {
// } FFTFloat1024_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFloat1024
#  undef FFTFloat1024
#  endif
#  define FFTFloat1024(param, coreid)\
  apcapi_core("FFTFloat1024", param, coreid)
#endif

