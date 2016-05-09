// This is the header file for slib Filter2D
#ifndef Filter2D_H
#define Filter2D_H
// Modify the parameter definition
// typedef struct {
// } Filter2D_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef Filter2D
#  undef Filter2D
#  endif
#  define Filter2D(param, coreid)\
  apcapi_core("Filter2D", param, coreid)
#endif

