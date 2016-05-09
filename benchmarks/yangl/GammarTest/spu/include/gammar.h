// This is the header file for slib gammar
#ifndef gammar_H
#define gammar_H
// Modify the parameter definition
// typedef struct {
// } gammar_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef gammar
#  undef gammar
#  endif
#  define gammar(param, coreid)\
  apcapi_core("gammar", param, coreid)
#endif

