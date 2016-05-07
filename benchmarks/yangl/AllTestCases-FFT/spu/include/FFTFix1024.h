// This is the header file for slib FFTFix1024
#ifndef FFTFix1024_H
#define FFTFix1024_H
// Modify the parameter definition
// typedef struct {
// } FFTFix1024_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFix1024
#  undef FFTFix1024
#  endif
#  define FFTFix1024(param, coreid)\
  apcapi_core("FFTFix1024", param, coreid)
#endif

