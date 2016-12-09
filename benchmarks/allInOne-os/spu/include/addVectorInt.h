// This is the header file for slib addVectorInt
#ifndef addVectorInt_H
#define addVectorInt_H
// Modify the parameter definition
// typedef struct {
// } addVectorInt_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef addVectorInt
#  undef addVectorInt
#  endif
#  define addVectorInt(param, coreid)\
  apcapi_core("addVectorInt", param, coreid)
#endif

