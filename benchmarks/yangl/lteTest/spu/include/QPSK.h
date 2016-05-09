// This is the header file for slib QPSK
#ifndef QPSK_H
#define QPSK_H
// Modify the parameter definition
// typedef struct {
// } QPSK_param;
// DO NOT change the prototype
// slib_name(param, coreid, flag)
//   slib_name: the spu function name
//   param: the pointer to the parameter location on arm
//   coreid: the APE core id
//   flag: APC_BLOCK: arm will wait until this function is finished
#  ifdef QPSK
#  undef QPSK
#  endif
#  define QPSK(param, coreid)\
  apcapi_core("QPSK", param, coreid)
#endif

