// This is the header file for slib SPULoad
#ifndef SPULoad_H
#define SPULoad_H
// Modify the parameter definition
// typedef struct {
// } SPULoad_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef SPULoad
#  undef SPULoad
#  endif
#  define SPULoad(param, coreid)\
  apcapi_core("SPULoad", param, coreid)
#endif

