// This is the header file for slib addVectorFullInt
#ifndef addVectorFullInt_H
#define addVectorFullInt_H
// Modify the parameter definition
// typedef struct {
// } addVectorFullInt_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef addVectorFullInt
#  undef addVectorFullInt
#  endif
#  define addVectorFullInt(param, coreid)\
  apcapi_core("addVectorFullInt", param, coreid)
#endif

