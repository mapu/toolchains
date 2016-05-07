// This is the header file for slib FFTFloat4096_spu
#ifndef FFTFloat4096_spu_H
#define FFTFloat4096_spu_H
// Modify the parameter definition
// typedef struct {
// } FFTFloat4096_spu_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFloat4096_spu
#  undef FFTFloat4096_spu
#  endif
#  define FFTFloat4096_spu(param, coreid)\
  apcapi_core("FFTFloat4096_spu", param, coreid)
#endif

