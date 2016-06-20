// This is the header file for slib medias
#ifndef medias_H
#define medias_H
// Modify the parameter definition
// typedef struct {
// } medias_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef medias
#  undef medias
#  endif
#  define medias(param, coreid)\
  apcapi_core("medias", param, coreid)
#endif

