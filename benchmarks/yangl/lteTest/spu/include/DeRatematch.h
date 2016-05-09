// This is the header file for slib DeRatematch
#ifndef DeRatematch_H
#define DeRatematch_H
// Modify the parameter definition
// typedef struct {
// } DeRatematch_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef DeRatematch
#  undef DeRatematch
#  endif
#  define DeRatematch(param, coreid)\
  apcapi_core("DeRatematch", param, coreid)
#endif

