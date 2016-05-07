// This is the header file for slib FFTFloat256
#ifndef FFTFloat256_H
#define FFTFloat256_H
// Modify the parameter definition
// typedef struct {
// } FFTFloat256_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFloat256
#  undef FFTFloat256
#  endif
#  define FFTFloat256(param, coreid)\
  apcapi_core("FFTFloat256", param, coreid)
#endif

