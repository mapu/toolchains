// RUN: clang -target mspu -S -o %t.s %s
#include "intrinsics.h"
extern  MMPUFn fn;
void  intr()
{
  __mspu_setKB(KB3, 3);
  __mspu_setKE(KE1, 3);
  __mspu_setKS(KS2, 3);
  __mspu_setKI(KI3, 3);
  __mspu_setKM(KM0, 3);
  __mspu_setKG(KG1, 3);
  __mspu_setKL(KL2, 3);

  __mspu_getKB(KB0);
  __mspu_getKE(KE1);
  __mspu_getKS(KS2);
  __mspu_getKI(KI3);
  __mspu_getKM(KM0);
  __mspu_getKG(KG1);
  __mspu_getKL(KL2);

  __mspu_getCStat();


  __mspu_getMC(MC_r1);
  __mspu_getLPCtrl();
  __mspu_setMC(MC_w2, 12);

  __mspu_setCh(3, 93);
  __mspu_getCh(3);

  __mspu_setChIndr(3, 93);
  __mspu_getChIndr(3);

  __mspu_getChNum(3);
  __mspu_getChNumIndr(3);

  __mspu_callm(fn);
  __mspu_callmb(fn);

}
