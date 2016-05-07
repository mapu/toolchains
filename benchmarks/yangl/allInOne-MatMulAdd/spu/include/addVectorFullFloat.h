// This is the header file for slib addVectorFullFloat
#ifndef addVectorFullFloat_H
#define addVectorFullFloat_H
// Modify the parameter definition
// typedef struct {
// } addVectorFullFloat_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef addVectorFullFloat
#  undef addVectorFullFloat
#  endif
#  define addVectorFullFloat(param, coreid)\
  apcapi_core("addVectorFullFloat", param, coreid)
#endif

