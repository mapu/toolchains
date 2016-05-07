// This is the header file for slib FFTFix4096_spu
#ifndef FFTFix4096_spu_H
#define FFTFix4096_spu_H
// Modify the parameter definition
// typedef struct {
// } FFTFix4096_spu_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFix4096_spu
#  undef FFTFix4096_spu
#  endif
#  define FFTFix4096_spu(param, coreid)\
  apcapi_core("FFTFix4096_spu", param, coreid)
#endif

