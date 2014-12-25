/*
 * mpuregister.hh
 *
 *  Created on: Oct 28, 2013
 *      Author: wangl
 */

#ifndef MPUREGISTER_HH_
#define MPUREGISTER_HH_

#include "base/types.hh"
#include <fenv.h>
#include <string.h>

const uint8_t MAX_UINT8 = 255;
const int8_t MAX_INT8 = 127;
const int8_t MIN_INT8 = -128;
const uint16_t MAX_UINT16 = 65535;
const int16_t MAX_INT16 = 32767;
const int16_t MIN_INT16 = -32768;
const uint32_t MAX_UINT32 = 4294967295;
const int32_t MAX_INT32 = 2147483647;
const int32_t MIN_INT32 = -2147483648;


namespace MapuISA {

struct BiuContext;
class MPURegister {
private:
  union {
    uint8_t vub[64];
    uint16_t vuh[32];
    uint32_t vuw[16];
    uint64_t vud[8];
    int8_t vb[64];
    int16_t vh[32];
    int32_t vw[16];
    int64_t vd[8];
    float vfp[16];
    double vdp[8];
  } storage, result;
  bool trunc;
  bool isInt;
public:
  MPURegister() :
      trunc(true), isInt(true) {
  }
  MPURegister(const MPURegister &m) :
      trunc(m.trunc), isInt(m.isInt) {
    storage = m.storage;
    result = m.result;
  }
  MPURegister(const uint8_t &m) :
      trunc(false), isInt(true) {
    memset(&storage, m, 64);
    memset(&result, m, 64);
  }

  void settrunc(bool t) {
    trunc = t;
  }

  void setInt(bool i) {
    isInt = i;
  }

  // convertor
  operator uint8_t*() {
    return storage.vub;
  }
  operator uint16_t*() {
    return storage.vuh;
  }
  operator uint32_t*() {
    return storage.vuw;
  }
  operator uint64_t*() {
    return storage.vud;
  }
  operator int8_t*() {
    return storage.vb;
  }
  operator int16_t*() {
    return storage.vh;
  }
  operator int32_t*() {
    return storage.vw;
  }
  operator int64_t*() {
    return storage.vd;
  }
  operator float*() {
    return storage.vfp;
  }
  operator double*() {
    return storage.vdp;
  }
  // addition
  uint8_t *operator +(const uint8_t *rhs);
  uint16_t *operator +(const uint16_t *rhs);
  uint32_t *operator +(const uint32_t *rhs);
  int8_t *operator +(const int8_t *rhs);
  int16_t *operator +(const int16_t *rhs);
  int32_t *operator +(const int32_t *rhs);
  float *operator +(const float *rhs);
  double *operator +(const double *rhs);

  // vector add scalar
  uint8_t *operator +(const uint8_t rhs);

  // subtraction
  uint8_t *operator -(const uint8_t *rhs);
  uint16_t *operator -(const uint16_t *rhs);
  uint32_t *operator -(const uint32_t *rhs);
  int8_t *operator -(const int8_t *rhs);
  int16_t *operator -(const int16_t *rhs);
  int32_t *operator -(const int32_t *rhs);
  float *operator -(const float *rhs);
  double *operator -(const double *rhs);

  // comparison
  uint8_t *operator >(const uint8_t *rhs);
  uint16_t *operator >(const uint16_t *rhs);
  uint32_t *operator >(const uint32_t *rhs);
  int8_t *operator >(const int8_t *rhs);
  int16_t *operator >(const int16_t *rhs);
  int32_t *operator >(const int32_t *rhs);
  uint32_t *operator >(const float *rhs);
  uint64_t *operator >(const double *rhs);
  uint8_t *operator <=(const uint8_t *rhs);
  uint16_t *operator <=(const uint16_t *rhs);
  uint32_t *operator <=(const uint32_t *rhs);
  int8_t *operator <=(const int8_t *rhs);
  int16_t *operator <=(const int16_t *rhs);
  int32_t *operator <=(const int32_t *rhs);
  uint32_t *operator <=(const float *rhs);
  uint64_t *operator <=(const double *rhs);

  uint8_t *operator <(const uint8_t *rhs);
  uint16_t *operator <(const uint16_t *rhs);
  uint32_t *operator <(const uint32_t *rhs);
  int8_t *operator <(const int8_t *rhs);
  int16_t *operator <(const int16_t *rhs);
  int32_t *operator <(const int32_t *rhs);
  uint32_t *operator <(const float *rhs);
  uint64_t *operator <(const double *rhs);

  uint8_t *operator >=(const uint8_t *rhs);
  uint16_t *operator >=(const uint16_t *rhs);
  uint32_t *operator >=(const uint32_t *rhs);
  int8_t *operator >=(const int8_t *rhs);
  int16_t *operator >=(const int16_t *rhs);
  int32_t *operator >=(const int32_t *rhs);
  uint32_t *operator >=(const float *rhs);
  uint64_t *operator >=(const double *rhs);

  uint8_t *operator ==(const uint8_t *rhs);
  uint16_t *operator ==(const uint16_t *rhs);
  uint32_t *operator ==(const uint32_t *rhs);
  int8_t *operator ==(const int8_t *rhs);
  int16_t *operator ==(const int16_t *rhs);
  int32_t *operator ==(const int32_t *rhs);
  uint32_t *operator ==(const float *rhs);
  uint64_t *operator ==(const double *rhs);

  uint8_t *operator !=(const uint8_t *rhs);
  uint16_t *operator !=(const uint16_t *rhs);
  uint32_t *operator !=(const uint32_t *rhs);
  int8_t *operator !=(const int8_t *rhs);
  int16_t *operator !=(const int16_t *rhs);
  int32_t *operator !=(const int32_t *rhs);
  uint32_t *operator !=(const float *rhs);
  uint64_t *operator !=(const double *rhs);

  // shift
  uint8_t *operator >>(const uint8_t *rhs);
  uint16_t *operator >>(const uint16_t *rhs);
  uint32_t *operator >>(const uint32_t *rhs);

  int8_t *operator >>(const int8_t *rhs);
  int16_t *operator >>(const int16_t *rhs);
  int32_t *operator >>(const int32_t *rhs);

  uint8_t *operator >>(const uint8_t rhs);
  uint16_t *operator >>(const uint16_t rhs);
  uint32_t *operator >>(const uint32_t rhs);

  int8_t *operator >>(const int8_t rhs);
  int16_t *operator >>(const int16_t rhs);
  int32_t *operator >>(const int32_t rhs);

  uint8_t *operator <<(const uint8_t *rhs);
  uint16_t *operator <<(const uint16_t *rhs);
  uint32_t *operator <<(const uint32_t *rhs);

  uint8_t *operator <<(const uint8_t rhs);
  uint16_t *operator <<(const uint16_t rhs);
  uint32_t *operator <<(const uint32_t rhs);

  // multipication
  uint8_t *operator *(const uint8_t *rhs);
  uint16_t *operator *(const uint16_t *rhs);
  uint32_t *operator *(const uint32_t *rhs);
  int8_t *operator *(const int8_t *rhs);
  int16_t *operator *(const int16_t *rhs);
  int32_t *operator *(const int32_t *rhs);
  float *operator *(const float *rhs);
  double *operator *(const double *rhs);

  // div
  uint16_t *operator /(const uint16_t *rhs);
  int16_t  *operator /(const int16_t *rhs);

  // mod
  uint16_t *operator %(const uint16_t *rhs);
  int16_t  *operator %(const int16_t *rhs);

  // index
  uint8_t *operator [](const MPURegister &idx); //const

  uint8_t &operator [](const int idx);

  uint8_t operator [](const int idx) const; //const

  // assignment
  MPURegister &operator =(const void *rhs);

  MPURegister &operator =(const MPURegister &rhs);

  MPURegister &operator =(const BiuContext &rhs);

  static void swap(MPURegister &lo, MPURegister &hi, unsigned size);

  static uint8_t *carry_ub(MPURegister &lhs, MPURegister &rhs);
  static uint16_t *carry_uh(MPURegister &lhs, MPURegister &rhs);
  static uint32_t *carry_uw(MPURegister &lhs, MPURegister &rhs);

// abs
  void mabs_ub(MPURegister &lhs, MPURegister &rhs);
  void mabs_uh(MPURegister &lhs, MPURegister &rhs);
  void mabs_uw(MPURegister &lhs, MPURegister &rhs);

  void mabs_b(MPURegister &lhs, MPURegister &rhs);
  void mabs_h(MPURegister &lhs, MPURegister &rhs);
  void mabs_w(MPURegister &lhs, MPURegister &rhs);

//merge
  void merge_b(MPURegister &sel, MPURegister &lhs, MPURegister &rhs);
  void merge_h(MPURegister &sel, MPURegister &lhs,
                           MPURegister &rhs);
  void merge_w(MPURegister &sel, MPURegister &lhs,
                           MPURegister &rhs);

//Rmax
  void rmax_ub(MPURegister &tm);
  void rmax_b(MPURegister &tm);

//Rmin
  void rmin_ub(MPURegister &tm);
  void rmin_b(MPURegister &tm);

//logic
  uint8_t *operator &(const uint8_t *rhs);
  uint8_t *operator |(const uint8_t *rhs);
  uint8_t *operator ^(const uint8_t *rhs);
  uint8_t *operator ~();

//cprs
  void cprs_uh(MPURegister &lhs, MPURegister &rhs);
  void cprs_uw(MPURegister &lhs, MPURegister &rhs);

  void cprs_h(MPURegister &lhs, MPURegister &rhs);
  void cprs_w(MPURegister &lhs, MPURegister &rhs);

//expd
  void expd_ub0(MPURegister &lhs);
  void expd_ub1(MPURegister &lhs);

  void expd_uh0(MPURegister &lhs);
  void expd_uh1(MPURegister &lhs);

  void expd_b0(MPURegister &lhs);
  void expd_b1(MPURegister &lhs);

  void expd_h0(MPURegister &lhs);
  void expd_h1(MPURegister &lhs);

  void cmul(MPURegister &lhs, MPURegister &rhs, MPURegister &tps);

  void floatcmul(MPURegister &lhs, MPURegister &rhs);

  uint32_t * WShuffle(uint8_t type);

  uint32_t * fftind();
};

}

#endif /* MPUREGISTER_HH_ */
