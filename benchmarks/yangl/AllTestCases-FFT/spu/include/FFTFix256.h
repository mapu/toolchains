// This is the header file for slib FFTFix256
#ifndef FFTFix256_H
#define FFTFix256_H
// Modify the parameter definition
// typedef struct {
// } FFTFix256_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFix256
#  undef FFTFix256
#  endif
#  define FFTFix256(param, coreid)\
  apcapi_core("FFTFix256", param, coreid)
#endif

