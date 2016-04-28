// This is the header file for slib SPULoad_2
#ifndef SPULoad_2_H
#define SPULoad_2_H
// Modify the parameter definition
// typedef struct {
// } SPULoad_2_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef SPULoad_2
#  undef SPULoad_2
#  endif
#  define SPULoad_2(param, coreid)\
  apcapi_core("SPULoad_2", param, coreid)
#endif

