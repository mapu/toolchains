// This is the header file for slib FFTFloat64_spu
#ifndef FFTFloat64_spu_H
#define FFTFloat64_spu_H
// Modify the parameter definition
// typedef struct {
// } FFTFloat64_spu_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef FFTFloat64_spu
#  undef FFTFloat64_spu
#  endif
#  define FFTFloat64_spu(param, coreid)\
  apcapi_core("FFTFloat64_spu", param, coreid)
#endif

