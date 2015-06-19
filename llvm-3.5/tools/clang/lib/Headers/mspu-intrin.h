#ifndef MSPU_INTRINSICS_H
#define MSPU_INTRINSICS_H

enum MSPU_KB {
  KB0 = 0,
  KB1 = 1,
  KB2 = 2,
  KB3 = 3,
  KB4 = 4,
  KB5 = 5,
  KB6 = 6,
  KB7 = 7,
  KB8 = 8,
  KB9 = 9,
  KB10 = 10,
  KB11 = 11,
  KB12 = 12,
  KB13 = 13,
  KB14 = 14,
  KB15 = 15
};

int  __mspu_getKB(const int KR);
void __mspu_setKB(const int KR, int);

enum MSPU_KE {
  KE0 = 0,
  KE1 = 1,
  KE2 = 2,
  KE3 = 3,
  KE4 = 4,
  KE5 = 5,
  KE6 = 6,
  KE7 = 7,
  KE8 = 8,
  KE9 = 9,
  KE10 = 10,
  KE11 = 11,
  KE12 = 12,
  KE13 = 13,
  KE14 = 14,
  KE15 = 15
};

int  __mspu_getKE(const int KR);
void __mspu_setKE(const int KR, int);

enum MSPU_KS {
  KS0 = 0,
  KS1 = 1,
  KS2 = 2,
  KS3 = 3,
  KS4 = 4,
  KS5 = 5,
  KS6 = 6,
  KS7 = 7,
  KS8 = 8,
  KS9 = 9,
  KS10 = 10,
  KS11 = 11,
  KS12 = 12,
  KS13 = 13,
  KS14 = 14,
  KS15 = 15
};

int  __mspu_getKS(const int KR);
void __mspu_setKS(const int KR, int);

enum MSPU_KI {
  KI0 = 0,
  KI1 = 1,
  KI2 = 2,
  KI3 = 3,
  KI4 = 4,
  KI5 = 5,
  KI6 = 6,
  KI7 = 7,
  KI8 = 8,
  KI9 = 9,
  KI10 = 10,
  KI11 = 11,
  KI12 = 12,
  KI13 = 13,
  KI14 = 14,
  KI15 = 15
};

int  __mspu_getKI(const int KR);
void __mspu_setKI(const int KR, int);

enum MSPU_KM {
  KM0 = 0,
  KM1 = 1,
  KM2 = 2
};

int  __mspu_getKM(const int KR);
void __mspu_setKM(const int KR, int);

int  __mspu_getHiKM(const int KR);
void __mspu_setHiKM(const int KR, int);

enum MSPU_KG {
  KG0 = 0,
  KG1 = 1,
  KG2 = 2
};

int  __mspu_getKG(const int KR);
void __mspu_setKG(const int KR, int);

enum MSPU_KL {
  KL0 = 0,
  KL1 = 1,
  KL2 = 2
};

int  __mspu_getKL(const int KR);
void __mspu_setKL(const int KR, int);

int __mspu_getCStat(void);

enum MSPU_MC {
  MC_r0 = 0,
  MC_r1 = 1,
  MC_w0 = 2,
  MC_w1 = 3,
  MC_w2 = 4
};

int  __mspu_getMC(const int MC);
void __mspu_setMC(const int MC, int);

int  __mspu_getLPCtrl(void);
void __mspu_setLPCtrl (int);

enum MSPU_CH {
  EventStatus = 0,
  EventMaskWrite = 1,
  EventAck = 2,
  EventMaskRead = 3,
  DMALocalAddress = 4,
  DMALocalXNum = 5,
  DMALocalYStep = 6,
  DMALocalYNum = 7,
  DMALocalZStep = 8,
  DMALocalAllNum = 9,
  DMAGlobalAddress = 10,
  DMAGlobalXNum = 11,
  DMAGlobalYStep = 12,
  DMAGlobalYNum = 13,
  DMAGlobalZStep = 14,
  DMAGlobalAllNum = 15,
  DMAGroupNum = 16,
  DMACmd = 17,
  GroupMaskWrite = 18,
  GroupMaskRead = 19,
  GroupStatusUpdate = 20,
  GroupStatus = 21,
  GroupListStalled = 22,
  GroupListAck = 23,
  MailboxOut = 24,
  MailboxIn = 25,
  MailboxOutInt = 26
};

void __mspu_setCh(const int id, int val);
int  __mspu_getCh(const int id);
int  __mspu_getChNum(const int id);

void __mspu_setChIndr(int id, int val);
int  __mspu_getChIndr(int id);
int  __mspu_getChNumIndr(int id);

// get and set SVR
enum MSPU_SVR {
  SVR0 = 0, // 0b'0
  SVR1 = 1, // 0b'1
};

// encoding for "BH"
enum MSPU_ACCESS_Mode {
  MODE_B = 2, // 0b'10
  MODE_H = 1, // 0b'01
  MODE_W = 0, // 0b'00
};

enum MSPU_SHU {
  SHU0 = 0, // 0b'0
  SHU1 = 1, // 0b'1
};

enum MSPU_T {
  T0 = 0, // 0b'00
  T1 = 1, // 0b'01
  T2 = 2, // 0b'10
  T3 = 3, // 0b'11
};

// SVR0[R0] = R3 (B);
void __mspu_setSVR(const int svr, int index, int val, const int mode);
int  __mspu_getSVR(const int svr, int index, const int mode);

// load and store SVR
//void __mspu_loadSVR (const int svr, int index, void * addr, int offset, const int mode);
//void __mspu_storeSVR(const int svr, int index, void * addr, int offset, const int mode);

void __mspu_setSHU(const int SHU, const int T, int val, const int mode);
int  __mspu_getSHU(const int SHU, const int T, const int mode);

void   __mspu_setSHU64(const int SHU, const int T, double val);
double __mspu_getSHU64(const int SHU, const int T);

// SHU.T = svr
void __mspu_setSHUWithSVR(const int SHU, const int T, const int svr);
// svr = SHU.T
void __mspu_setSVRWithSHU(const int svr, const int SHU, const int T);

// invoke mmpu functions
typedef void MMPUFn(void);

void __mspu_callm (void*  mpu_fn);
void __mspu_callmb(void*  mpu_fn);

// Atomic memory access
int __mspu_atomicld(int *addr, const int offset);
int __mspu_atomicst(int *addr, const int offset, int val);

// Hardware loop
void __mspu_loop(void *label, const int cnt);
void __mspu_delay(void);

// Lpto

#endif // MSPU_INTRINSICS_H
