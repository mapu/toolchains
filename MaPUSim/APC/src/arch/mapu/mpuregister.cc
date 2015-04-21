/*
 * mpuregister.cc
 *
 *  Created on: Nov 20, 2013
 *      Author: wangl & zhangx
 */

#include "arch/mapu/mpuregister.hh"
#include "arch/mapu/registers.hh"
#include "base/types.hh"
#include <fenv.h>
#include <string.h>
#include <cmath>
#include <iostream>

namespace MapuISA {

// addition
uint8_t *MPURegister::operator +(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++) {
    if (trunc)
      res[i] = storage.vub[i] + rhs[i];
    else {
      uint16_t temp = uint16_t(storage.vub[i]) + uint16_t(rhs[i]);
      res[i] = temp > MAX_UINT8 ? MAX_UINT8 : storage.vub[i] + rhs[i];
    }
  }
  return res;
}
uint16_t *MPURegister::operator +(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++) {
    if (trunc)
      res[i] = storage.vuh[i] + rhs[i];
    else {
      uint32_t temp = uint32_t(storage.vuh[i]) + uint32_t(rhs[i]);
      res[i] = temp > MAX_UINT16 ? MAX_UINT16 : storage.vuh[i] + rhs[i];
    }
  }
  return res;
}
uint32_t *MPURegister::operator +(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++) {
    if (trunc)
      res[i] = storage.vuw[i] + rhs[i];
    else {
      uint64_t temp = uint64_t(storage.vuw[i]) + uint64_t(rhs[i]);
      res[i] = temp > MAX_UINT32 ? MAX_UINT32 : storage.vuw[i] + rhs[i];
    }
  }
  return res;
}
int8_t *MPURegister::operator +(const int8_t *rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++) {
    if (trunc)
      res[i] = storage.vb[i] + rhs[i];
    else {
      int16_t temp = int16_t(storage.vb[i]) + int16_t(rhs[i]);
      res[i] = temp > MAX_INT8 ? MAX_INT8 :
                  temp < MIN_INT8 ? MIN_INT8 : storage.vb[i] + rhs[i];
    }
  }
  return res;
}
int16_t *MPURegister::operator +(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++) {
    if (trunc)
      res[i] = storage.vh[i] + rhs[i];
    else {
      int32_t temp = int32_t(storage.vh[i]) + int32_t(rhs[i]);
      res[i] = temp > MAX_INT16 ? MAX_INT16 :
                  temp < MIN_INT16 ? MIN_INT16 : storage.vh[i] + rhs[i];
    }
  }
  return res;
}
int32_t *MPURegister::operator +(const int32_t *rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++) {
    if (trunc)
      res[i] = storage.vw[i] + rhs[i];
    else {
      int64_t temp = int64_t(storage.vw[i]) + int64_t(rhs[i]);
      res[i] = temp > MAX_INT32 ? MAX_INT32 :
                  temp < MIN_INT32 ? MIN_INT32 : storage.vw[i] + rhs[i];
    }
  }
  return res;
}
float *MPURegister::operator +(const float *rhs) {
  float *res = result.vfp;
  if (trunc)
    fesetround(FE_TOWARDZERO);
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vfp[i] + rhs[i];
  fesetround(FE_TONEAREST);
  return res;
}
double *MPURegister::operator +(const double *rhs) {
  double *res = result.vdp;
  if (trunc)
    fesetround(FE_TOWARDZERO);
  for (unsigned i = 0; i < 8; i++)
    res[i] = storage.vdp[i] + rhs[i];
  fesetround(FE_TONEAREST);
  return res;
}

// vector add scalar
uint8_t *MPURegister::operator +(const uint8_t rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] + rhs;
  return res;
}

// subtraction
uint8_t *MPURegister::operator -(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++) {
    if (trunc)
      res[i] = storage.vub[i] - rhs[i];
    else {
      int16_t temp = int16_t(storage.vub[i]) - int16_t(rhs[i]);
      res[i] = temp < 0 ? 0 : storage.vub[i] - rhs[i];
    }
  }
  return res;
}
uint16_t *MPURegister::operator -(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++) {
    if (trunc)
      res[i] = storage.vuh[i] - rhs[i];
    else {
      int32_t temp = int32_t(storage.vuh[i]) - int32_t(rhs[i]);
      res[i] = temp < 0 ? 0 : storage.vuh[i] - rhs[i];
    }
  }
  return res;
}
uint32_t *MPURegister::operator -(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++) {
    if (trunc)
      res[i] = storage.vuw[i] - rhs[i];
    else {
      int64_t temp = int64_t(storage.vuw[i]) - int64_t(rhs[i]);
      res[i] = temp < 0 ? 0 : storage.vuw[i] - rhs[i];
    }
  }
  return res;
}
int8_t *MPURegister::operator -(const int8_t *rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++) {
    if (trunc)
      res[i] = storage.vb[i] - rhs[i];
    else {
      int16_t temp = int16_t(storage.vb[i]) - int16_t(rhs[i]);
      res[i] = temp > MAX_INT8 ? MAX_INT8 :
                  temp < MIN_INT8 ? MIN_INT8 : storage.vb[i] - rhs[i];
    }
  }
  return res;
}
int16_t *MPURegister::operator -(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++) {
    if (trunc)
      res[i] = storage.vh[i] - rhs[i];
    else {
      int32_t temp = int32_t(storage.vh[i]) - int32_t(rhs[i]);
      res[i] = temp > MAX_INT16 ? MAX_INT16 :
                  temp < MIN_INT16 ? MIN_INT16 : storage.vh[i] - rhs[i];
    }
  }
  return res;
}
int32_t *MPURegister::operator -(const int32_t *rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++) {
    if (trunc)
      res[i] = storage.vw[i] - rhs[i];
    else {
      int64_t temp = int64_t(storage.vw[i]) - int64_t(rhs[i]);
      res[i] = temp > MAX_INT32 ? MAX_INT32 :
                  temp < MIN_INT32 ? MIN_INT32 : storage.vw[i] - rhs[i];
    }
  }
  return res;
}
float *MPURegister::operator -(const float *rhs) {
  float *res = result.vfp;
  if (trunc)
    fesetround(FE_TOWARDZERO);
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vfp[i] - rhs[i];
  fesetround(FE_TONEAREST);
  return res;
}
double *MPURegister::operator -(const double *rhs) {
  double *res = result.vdp;
  if (trunc)
    fesetround(FE_TOWARDZERO);
  for (unsigned i = 0; i < 8; i++)
    res[i] = storage.vdp[i] - rhs[i];
  fesetround(FE_TONEAREST);
  return res;
}

// comparison
uint8_t *MPURegister::operator >(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] > rhs[i];
  return res;
}
uint16_t *MPURegister::operator >(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] > rhs[i];
  return res;
}
uint32_t *MPURegister::operator >(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] > rhs[i];
  return res;
}
int8_t *MPURegister::operator >(const int8_t *rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vb[i] > rhs[i];
  return res;
}
int16_t *MPURegister::operator >(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vh[i] > rhs[i];
  return res;
}
int32_t *MPURegister::operator >(const int32_t *rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vw[i] > rhs[i];
  return res;
}
uint32_t *MPURegister::operator >(const float *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vfp[i] > rhs[i];
  return res;
}
uint64_t *MPURegister::operator >(const double *rhs) {
  uint64_t *res = (uint64_t *)result.vub;
  for (unsigned i = 0; i < 8; i++)
    res[i] = storage.vdp[i] > rhs[i];
  return res;
}

uint8_t *MPURegister::operator <=(const uint8_t *rhs) {
  uint8_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 64; i++)
    res[i] = !res[i];
  return res;
}
uint16_t *MPURegister::operator <=(const uint16_t *rhs) {
  uint16_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 32; i++)
    res[i] = !res[i];
  return res;
}
uint32_t *MPURegister::operator <=(const uint32_t *rhs) {
  uint32_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
int8_t *MPURegister::operator <=(const int8_t *rhs) {
  int8_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 64; i++)
    res[i] = !res[i];
  return res;
}
int16_t *MPURegister::operator <=(const int16_t *rhs) {
  int16_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 32; i++)
    res[i] = !res[i];
  return res;
}
int32_t *MPURegister::operator <=(const int32_t *rhs) {
  int32_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
uint32_t *MPURegister::operator <=(const float *rhs) {
  uint32_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
uint64_t *MPURegister::operator <=(const double *rhs) {
  uint64_t *res = (*this) > rhs;
  for (unsigned i = 0; i < 8; i++)
    res[i] = !res[i];
  return res;
}

uint8_t *MPURegister::operator <(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] < rhs[i];
  return res;
}
uint16_t *MPURegister::operator <(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] < rhs[i];
  return res;
}
uint32_t *MPURegister::operator <(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] < rhs[i];
  return res;
}
int8_t *MPURegister::operator <(const int8_t *rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vb[i] < rhs[i];
  return res;
}
int16_t *MPURegister::operator <(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vh[i] < rhs[i];
  return res;
}
int32_t *MPURegister::operator <(const int32_t *rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vw[i] < rhs[i];
  return res;
}
uint32_t *MPURegister::operator <(const float *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vfp[i]< rhs[i];
  return res;
}
uint64_t *MPURegister::operator <(const double *rhs) {
  uint64_t *res = (uint64_t *)result.vub;
  for (unsigned i = 0; i < 8; i++)
    res[i] = storage.vdp[i] < rhs[i];
  return res;
}

uint8_t *MPURegister::operator >=(const uint8_t *rhs) {
  uint8_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 64; i++)
    res[i] = !res[i];
  return res;
}
uint16_t *MPURegister::operator >=(const uint16_t *rhs) {
  uint16_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 32; i++)
    res[i] = !res[i];
  return res;
}
uint32_t *MPURegister::operator >=(const uint32_t *rhs) {
  uint32_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
int8_t *MPURegister::operator >=(const int8_t *rhs) {
  int8_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 64; i++)
    res[i] = !res[i];
  return res;
}
int16_t *MPURegister::operator >=(const int16_t *rhs) {
  int16_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 32; i++)
    res[i] = !res[i];
  return res;
}
int32_t *MPURegister::operator >=(const int32_t *rhs) {
  int32_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
uint32_t *MPURegister::operator >=(const float *rhs) {
  uint32_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
uint64_t *MPURegister::operator >=(const double *rhs) {
  uint64_t *res = (*this) < rhs;
  for (unsigned i = 0; i < 8; i++)
    res[i] = !res[i];
  return res;
}

uint8_t *MPURegister::operator ==(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] == rhs[i];
  return res;
}
uint16_t *MPURegister::operator ==(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] == rhs[i];
  return res;
}
uint32_t *MPURegister::operator ==(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] == rhs[i];
  return res;
}
int8_t *MPURegister::operator ==(const int8_t *rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vb[i] == rhs[i];
  return res;
}
int16_t *MPURegister::operator ==(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vh[i] == rhs[i];
  return res;
}
int32_t *MPURegister::operator ==(const int32_t *rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vw[i] == rhs[i];
  return res;
}
uint32_t *MPURegister::operator ==(const float *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vfp[i] == rhs[i];
  return res;
}
uint64_t *MPURegister::operator ==(const double *rhs) {
  uint64_t *res = (uint64_t *)result.vub;
  for (unsigned i = 0; i < 8; i++)
    res[i] = storage.vdp[i] == rhs[i];
  return res;
}

uint8_t *MPURegister::operator !=(const uint8_t *rhs) {
  uint8_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 64; i++)
    res[i] = !res[i];
  return res;
}
uint16_t *MPURegister::operator !=(const uint16_t *rhs) {
  uint16_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 32; i++)
    res[i] = !res[i];
  return res;
}
uint32_t *MPURegister::operator !=(const uint32_t *rhs) {
  uint32_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
int8_t *MPURegister::operator !=(const int8_t *rhs) {
  int8_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 64; i++)
    res[i] = !res[i];
  return res;
}
int16_t *MPURegister::operator !=(const int16_t *rhs) {
  int16_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 32; i++)
    res[i] = !res[i];
  return res;
}
int32_t *MPURegister::operator !=(const int32_t *rhs) {
  int32_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
uint32_t *MPURegister::operator !=(const float *rhs) {
  uint32_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 16; i++)
    res[i] = !res[i];
  return res;
}
uint64_t *MPURegister::operator !=(const double *rhs) {
  uint64_t *res = (*this) == rhs;
  for (unsigned i = 0; i < 8; i++)
    res[i] = !res[i];
  return res;
}

// shift
uint8_t *MPURegister::operator >>(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] >> rhs[i];
  return res;
}
uint16_t *MPURegister::operator >>(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] >> rhs[i];
  return res;
}
uint32_t *MPURegister::operator >>(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] >> rhs[i];
  return res;
}
int8_t *MPURegister::operator >>(const int8_t *rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vb[i] >> rhs[i];
  return res;
}
int16_t *MPURegister::operator >>(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vh[i] >> rhs[i];
  return res;
}
int32_t *MPURegister::operator >>(const int32_t *rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vw[i] >> rhs[i];
  return res;
}

uint8_t *MPURegister::operator >>(const uint8_t rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] >> rhs;
  return res;
}
uint16_t *MPURegister::operator >>(const uint16_t rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] >> rhs;
  return res;
}
uint32_t *MPURegister::operator >>(const uint32_t rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] >> rhs;
  return res;
}
int8_t *MPURegister::operator >>(const int8_t rhs) {
  int8_t *res = result.vb;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vb[i] >> rhs;
  return res;
}
int16_t *MPURegister::operator >>(const int16_t rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vh[i] >> rhs;
  return res;
}
int32_t *MPURegister::operator >>(const int32_t rhs) {
  int32_t *res = result.vw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vw[i] >> rhs;
  return res;
}

uint8_t *MPURegister::operator <<(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] << rhs[i];
  return res;
}
uint16_t *MPURegister::operator <<(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] << rhs[i];
  return res;
}
uint32_t *MPURegister::operator <<(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] << rhs[i];
  return res;
}

uint8_t *MPURegister::operator <<(const uint8_t rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[i] << rhs;
  return res;
}
uint16_t *MPURegister::operator <<(const uint16_t rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++)
    res[i] = storage.vuh[i] << rhs;
  return res;
}
uint32_t *MPURegister::operator <<(const uint32_t rhs) {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vuw[i] << rhs;
  return res;
}

// multipication
uint8_t *MPURegister::operator *(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  union {
    uint8_t db[2];
    uint16_t sh;
  };

  for (unsigned i = 0; i < 64; i++) {
    if (isInt){
      sh = int16_t(storage.vub[i]) * rhs[i];
      if (!trunc)
        res[i] = db[1];
      else
        res[i] = db[0];
    } else {
      // Unimplemented
      double temp0 = ((double)storage.vub[i])/256.0; 
      double temp1 = ((double)rhs[i])/256.0;
      double temp2 = (temp0 * temp1) >= 1 ? 255.0/256.0 : (temp0 * temp1);
      res[i] = temp2 * 256.0;
    }
  }

  return res;
}

uint16_t *MPURegister::operator *(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  union {
    uint16_t dh[2];
    uint32_t sw;
  };
  for (unsigned i = 0; i < 32; i++) {    
    if (isInt) {
      sw = int32_t(storage.vuh[i]) * rhs[i];
      if (!trunc )
        res[i] = dh[1];
      else
        res[i] = dh[0];
    } else {
      sw = (uint32_t)storage.vuh[i] * rhs[i];
      res[i] = dh[1] + (dh[0] >> 15);
    }
  }

  return res;
}
uint32_t *MPURegister::operator *(const uint32_t *rhs) {
  uint32_t *res = result.vuw;
  union {
    uint32_t dw[2];
    uint64_t sd;
  };
  for (unsigned i = 0; i < 16; i++) {
    if (isInt){
      sd = int64_t(storage.vuw[i]) * rhs[i];
      if (!trunc)
        res[i] = dw[1];
      else
        res[i] = dw[0];
    } else {
      // Unimplemented
      double temp0 = ((double)storage.vuw[i])/4294967296.0;
      double temp1 = ((double)rhs[i])/4294967296.0;
      double temp2 = (temp0 * temp1) >= 1 ? 4294967295.0/4294967296.0 :(temp0 * temp1);
      res[i] = temp2 * 4294967296.0;
    }
  }
  return res;
}
int8_t *MPURegister::operator *(const int8_t *rhs) {
  int8_t *res = result.vb;
  union {
    int8_t db[2];
    int16_t sh;
  };
  for (unsigned i = 0; i < 64; i++) {
    if (isInt){
      sh = int16_t(storage.vb[i]) * rhs[i];
      if (!trunc)
        res[i] = db[1];
      else
        res[i] = db[0];
    } else {
      // Unimplemented
      double temp0 = ((double)storage.vb[i])/128.0; 
      double temp1 = ((double)rhs[i])/128.0;
      double temp2 = (temp0 * temp1) >= 1 ? 127.0/128.0 :
                     (temp0 * temp1) < -1 ? -1 : (temp0 * temp1);
      res[i] = temp2 * 128.0;
    }
  }
  return res;
}
int16_t *MPURegister::operator *(const int16_t *rhs) {
  int16_t *res = result.vh;
  union {
    int16_t dh[2];
    int32_t sw;
  };
  for (unsigned i = 0; i < 32; i++) {    
    if (isInt) {
      sw = int32_t(storage.vh[i]) * rhs[i];
      if (!trunc )
        res[i] = dh[1];
      else
        res[i] = dh[0];
    } else {
      sw = ((int32_t)storage.vh[i] * rhs[i]) << 1;
      res[i] = dh[1] + ((uint16_t)dh[0] >> 15);
    }
  }
  return res;
}
int32_t *MPURegister::operator *(const int32_t *rhs) {
  int32_t *res = result.vw;
  union {
    int32_t dw[2];
    int64_t sd;
  };
  for (unsigned i = 0; i < 16; i++) {
    if (isInt){
      sd = int64_t(storage.vw[i]) * rhs[i];
      if (!trunc)
        res[i] = dw[1];
      else
        res[i] = dw[0];
    } else {
      // Unimplemented
      double temp0 = ((double)storage.vw[i])/2147483648.0;
      double temp1 = ((double)rhs[i])/2147483648.0;
      double temp2 = (temp0 * temp1) >= 1 ? 2147483647.0/2147483648.0 :
                     (temp0 * temp1) < -1 ? -1 : (temp0 * temp1);
      res[i] = temp2 * 2147483647.0;
    }
  }
  return res;
}
float *MPURegister::operator *(const float *rhs) {
  float *res = result.vfp;
  if(trunc)
    fesetround(FE_TOWARDZERO);
  for (unsigned i = 0; i < 16; i++)
    res[i] = storage.vfp[i] * rhs[i];
  fesetround(FE_TONEAREST);
  return res;
}
double *MPURegister::operator *(const double *rhs) {
  double *res = result.vdp;
  if (trunc)
    fesetround(FE_TOWARDZERO);
  for (unsigned i = 0; i < 8; i++)
    res[i] = storage.vdp[i] * rhs[i];
  fesetround(FE_TONEAREST);
  return res;
}

uint16_t *MPURegister::operator /(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++) {    
    res[i] = storage.vuh[i] / rhs[i];
  }
  return res;
}

int16_t *MPURegister::operator /(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++) {    
    res[i] = storage.vh[i] / rhs[i];
  }
  return res;
}

uint16_t *MPURegister::operator %(const uint16_t *rhs) {
  uint16_t *res = result.vuh;
  for (unsigned i = 0; i < 32; i++) {    
    res[i] = storage.vuh[i] % rhs[i];
  }
  return res;
}

int16_t *MPURegister::operator %(const int16_t *rhs) {
  int16_t *res = result.vh;
  for (unsigned i = 0; i < 32; i++) {    
    res[i] = storage.vh[i] % rhs[i];
  }
  return res;
}

// index
uint8_t *MPURegister::operator [](const MPURegister &idx) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++)
    res[i] = storage.vub[idx.storage.vub[i] & 0x3F];
  return res;
}

uint8_t &MPURegister::operator [](const int idx) {
  return storage.vub[idx];
}

uint8_t MPURegister::operator [](const int idx) const {
  return storage.vub[idx];
}

// assignment
MPURegister &MPURegister::operator =(const void *rhs) {
  memcpy(&storage, rhs, 64);
  return *this;
}

MPURegister &MPURegister::operator =(const MPURegister &rhs) {
  trunc = rhs.trunc;
  isInt = rhs.isInt;
  memcpy(&storage, &rhs.storage, 64);
  return *this;
}

MPURegister &MPURegister::operator =(const BiuContext &rhs) {
  //assign KData={56'h0,KB3,KS3,KC3,KI3,18'h0,
  //              63 57 56 54 53 51 49  47  45
  //                KB2,KS2,KC2,KI2,KE2,
  //               44 42 41 39  37  35 33
  //                KB1,KS1,KC1,KI1,KE1,
  //               32 30 29 27  25  23 21
  //                KB0,KS0,KC0,KI0,KE0,
  //               20 18 17 15  13  11 9
  //                KM,2'b0,KGM,KL};
  //                8  1         0
  uint32_t temp = 0;
  memset(storage.vub, 0, 64);
  storage.vub[0] = rhs.KL | (rhs.KG << 3);
  *(uint32_t *)(&storage.vub[1]) = rhs.KM[0];
  *(uint32_t *)(&storage.vub[5]) = rhs.KM[1];
  for (int32_t i = 0; i < 4; i++) {
    *(uint32_t *)&(storage.vub[9 + i * 12]) = rhs.KE[i];
    temp = (rhs.KI[i] << 2) | (rhs.KE[i] >> 16);
    *(uint32_t *)&(storage.vub[11 + i * 12]) = temp;
    temp = (rhs.KC[i] << 4) | (rhs.KI[i] >> 14);
    *(uint32_t *)&(storage.vub[13 + i * 12]) = temp;
    temp = (rhs.KS[i] << 6) | (rhs.KC[i] >> 12);
    *(uint32_t *)&(storage.vub[15 + i * 12]) = temp;
    *(uint32_t *)&(storage.vub[18 + i * 12]) = rhs.KB[i];
  }
  return *this;
}

void MPURegister::swap(MPURegister &lo, MPURegister &hi, unsigned size) {
  if (size == 1) {
    // lo: 0 1 2 3
    // hi: 4 5 6 7
    // 1->2 2->4 4->1 3->6 6->5 5->3
    for (unsigned i = 0; i < 16; i++) {
      uint8_t tmp = lo[1 + i * 4];
      lo[1 + i * 4] = hi[i * 4];
      hi[i * 4] = lo[2 + i * 4];
      lo[2 + i * 4] = tmp;
      tmp = lo[3 + i * 4];
      lo[3 + i * 4] = hi[1 + i * 4];
      hi[1 + i * 4] = hi[2 + i * 4];
      hi[2 + i * 4] = tmp;
    }
  } else if (size == 2) {
    // lo: 0 1 2 3
    // hi: 4 5 6 7
    // 2->4 4->2 3->5 5->3
    for (unsigned i = 0; i < 16; i++) {
      uint8_t tmp = lo[2 + i * 2];
      lo[2 + i * 4] = hi[i * 4];
      hi[i * 4] = tmp;
      tmp = lo[3 + i * 4];
      lo[3 + i * 4] = hi[1 + i * 4];
      hi[1 + i * 4] = tmp;
    }
  }
}

uint8_t *MPURegister::carry_ub(MPURegister &lhs, MPURegister &rhs) {
  uint8_t *carry = lhs.result.vub;
  union {
    uint8_t db[2];
    uint16_t sh;
  };
  for (unsigned i = 0; i < 64; i++) {
    sh = (uint16_t) (lhs.storage.vub[i]) + rhs.storage.vub[i];
    carry[i] = db[1];
  }
  return carry;
}
uint16_t *MPURegister::carry_uh(MPURegister &lhs, MPURegister &rhs) {
  uint16_t *carry = lhs.result.vuh;
  union {
    uint16_t dh[2];
    uint32_t sw;
  };
  for (unsigned i = 0; i < 32; i++) {
    sw = (uint32_t) (lhs.storage.vuh[i]) + rhs.storage.vuh[i];
    carry[i] = dh[1];
  }
  return carry;
}
uint32_t *MPURegister::carry_uw(MPURegister &lhs, MPURegister &rhs) {
  uint32_t *carry = lhs.result.vuw;
  union {
    uint32_t dw[2];
    uint64_t sd;
  };
  for (unsigned i = 0; i < 16; i++) {
    sd = (uint64_t) (lhs.storage.vuw[i]) + rhs.storage.vuw[i];
    carry[i] = dw[1];
  }
  return carry;
}

// abs
void MPURegister::mabs_ub(MPURegister &lhs, MPURegister &rhs) {
  uint8_t *abs = storage.vub;
  uint8_t res[64];
  for (unsigned i = 0; i < 64; i++) {
    int16_t temp = int16_t(lhs.storage.vub[i]) - int16_t(rhs.storage.vub[i]);
    res[i] = temp < 0 ? 0 : lhs.storage.vub[i] - rhs.storage.vub[i];
    abs[i] = fabs(res[i]);
  }
}

void MPURegister::mabs_uh(MPURegister &lhs, MPURegister &rhs) {
  uint16_t *abs = storage.vuh;
  uint16_t res[32];
  for (unsigned i = 0; i < 32; i++) {
    int32_t temp = int32_t(lhs.storage.vuh[i]) - int32_t(rhs.storage.vuh[i]);
    res[i] = temp < 0 ? 0 : lhs.storage.vuh[i] - rhs.storage.vuh[i];
    abs[i] = fabs(res[i]);
  }
}

void MPURegister::mabs_uw(MPURegister &lhs, MPURegister &rhs) {
  uint32_t *abs = storage.vuw;
  uint32_t res[16];
  for (unsigned i = 0; i < 16; i++) {
    int64_t temp = int64_t(lhs.storage.vuw[i]) - int64_t(rhs.storage.vuw[i]);
    res[i] = temp < 0 ? 0 : lhs.storage.vuw[i] - rhs.storage.vuw[i];
    abs[i] = fabs(res[i]);
  }
}

void MPURegister::mabs_b(MPURegister &lhs, MPURegister &rhs) {
  int8_t *abs = storage.vb;
  int8_t res[64];
  for (unsigned i = 0; i < 64; i++) {
    int16_t temp = int16_t(lhs.storage.vb[i]) - int16_t(rhs.storage.vb[i]);
    res[i] =
      temp > MAX_INT8 ? MAX_INT8 :
      temp < MIN_INT8 ? MIN_INT8 : lhs.storage.vb[i] - rhs.storage.vb[i];
    abs[i] = fabs(res[i]);
  }
}

void MPURegister::mabs_h(MPURegister &lhs, MPURegister &rhs) {
  int16_t *abs = storage.vh;
  int16_t res[32];
  for (unsigned i = 0; i < 32; i++) {
    int32_t temp = int32_t(lhs.storage.vh[i]) - int32_t(rhs.storage.vh[i]);
    res[i] =
      temp > MAX_INT16 ? MAX_INT16 :
      temp < MIN_INT16 ? MIN_INT16 : lhs.storage.vh[i] - rhs.storage.vh[i];
    abs[i] = fabs(res[i]);
  }
}

void MPURegister::mabs_w(MPURegister &lhs, MPURegister &rhs) {
  int32_t *abs = storage.vw;
  int32_t res[16];
  for (unsigned i = 0; i < 16; i++) {
    int64_t temp = int64_t(lhs.storage.vw[i]) - int64_t(rhs.storage.vw[i]);
    res[i] =
      temp > MAX_INT32 ? MAX_INT32 :
      temp < MIN_INT32 ? MIN_INT32 : lhs.storage.vw[i] - rhs.storage.vw[i];
    abs[i] = fabs(res[i]);
  }
}

//merge
void MPURegister::merge_b(MPURegister &sel,
                             MPURegister &lhs,
                             MPURegister &rhs) {
  uint8_t *res = storage.vub;
  for (unsigned i = 0; i < 64; i++) {
    res[i] = sel.storage.vub[i] & 0x1 ? lhs.storage.vub[i] : rhs.storage.vub[i];
  }
}

void MPURegister::merge_h(MPURegister &sel,
                              MPURegister &lhs,
                              MPURegister &rhs) {
  uint16_t *res = storage.vuh;
  for (unsigned i = 0; i < 32; i++) {
    res[i] = sel.storage.vuh[i] & 0x1 ? lhs.storage.vuh[i] : rhs.storage.vuh[i];
  }
}

void MPURegister::merge_w(MPURegister &sel,
                              MPURegister &lhs,
                              MPURegister &rhs) {
  uint32_t *res = storage.vuw;
  for (unsigned i = 0; i < 16; i++) {
    res[i] = sel.storage.vuw[i] & 0x1 ? lhs.storage.vuw[i] : rhs.storage.vuw[i];
  }
}

//Rmax
void MPURegister::rmax_ub(MPURegister &tm) {
  uint8_t *res = storage.vub;
  for (unsigned i = 0; i < 8; i++) {
    for (unsigned j = 0; j < 8; j++) {
      res[8*i] = tm.storage.vub[8 * i];
      if (tm.storage.vub[8 * i + j] > res[i])
        res[8*i+j] = tm.storage.vub[8 * i + j];
    }
  }
}
void MPURegister::rmax_b(MPURegister &tm) {
  int8_t *res = storage.vb;
  for (unsigned i = 0; i < 8; i++) {
    for (unsigned j = 0; j < 8; j++) {
      res[8*i] = tm.storage.vb[8 * i];
      if (tm.storage.vb[8 * i + j] > res[i])
        res[8*i+j] = tm.storage.vb[8 * i + j];
    }
  }
}

//Rmin
void MPURegister::rmin_ub(MPURegister &tm) {
  uint8_t *res = storage.vub;
  for (unsigned i = 0; i < 8; i++) {
    for (unsigned j = 0; j < 8; j++) {
      res[8*i] = tm.storage.vub[8 * i];
      if (tm.storage.vub[8 * i + j] < res[i])
        res[8*i+j] = tm.storage.vub[8 * i + j];
    }
  }
}
void MPURegister::rmin_b(MPURegister &tm) {
  int8_t *res = storage.vb;
  for (unsigned i = 0; i < 8; i++) {
    for (unsigned j = 0; j < 8; j++) {
      res[8*i] = tm.storage.vb[8 * i];
      if (tm.storage.vb[8 * i + j] < res[i])
        res[8*i+j] = tm.storage.vb[8 * i + j];
    }
  }
}

// &
uint8_t *MPURegister::operator &(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++) {
    res[i] = storage.vub[i] & rhs[i];
  }
  return res;
}

uint8_t *MPURegister::operator |(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++) {
    res[i] = storage.vub[i] | rhs[i];
  }
  return res;
}

uint8_t *MPURegister::operator ^(const uint8_t *rhs) {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++) {
    res[i] = storage.vub[i] ^ rhs[i];
  }
  return res;
}

uint8_t *MPURegister::operator ~() {
  uint8_t *res = result.vub;
  for (unsigned i = 0; i < 64; i++) {
    res[i] = ~storage.vub[i];
  }
  return res;
}

//cprs
void MPURegister::cprs_uh(MPURegister &lhs, MPURegister &rhs) {
  uint8_t lo[32];
  uint8_t hi[32];
  uint8_t *res = storage.vub;

  for (unsigned i = 0; i < 32; i++) {
    if (lhs.trunc) {
      lo[i] = lhs.storage.vub[2*i];
      hi[i] = rhs.storage.vub[2*i];
    } else {
      uint8_t temp1 = lhs.storage.vub[2*i];
      uint8_t temp2 = rhs.storage.vub[2*i];
      lo[i] = temp1 > MAX_UINT8 ? MAX_UINT8 : lhs.storage.vub[2*i];
      hi[i] = temp2 > MAX_UINT8 ? MAX_UINT8 : rhs.storage.vub[2*i];
    }
  }

  for (unsigned i = 0; i < 16; i++) {
    res[4 * i] = lo[2 * i];
    res[4 * i + 1] = lo[2 * i + 1];
    res[4 * i + 2] = hi[2 * i];
    res[4 * i + 3] = hi[2 * i + 1];
  }
}

void MPURegister::cprs_uw(MPURegister &lhs, MPURegister &rhs) {
  uint16_t lo[16];
  uint16_t hi[16];
  uint16_t *res = storage.vuh;
  for (unsigned i = 0; i < 16; i++) {
    if (lhs.trunc) {
      lo[i] = lhs.storage.vuh[2*i];
      hi[i] = rhs.storage.vuh[2*i];
    } else {
      uint16_t temp1 = lhs.storage.vuh[2*i];
      uint16_t temp2 = rhs.storage.vuh[2*i];
      lo[i] = temp1 > MAX_UINT16 ? MAX_UINT16 : lhs.storage.vuh[2*i];
      hi[i] = temp2 > MAX_UINT16 ? MAX_UINT16 : rhs.storage.vuh[2*i];
    }
  }

  for (unsigned i = 0; i < 16; i++) {
    res[2 * i] = lo[i];
    res[2 * i + 1] = hi[i];
  }
}

void MPURegister::cprs_h(MPURegister &lhs, MPURegister &rhs) {
  int8_t lo[32];
  int8_t hi[32];
  int8_t *res = storage.vb;

  for (unsigned i = 0; i < 32; i++) {
    if (lhs.trunc) {
      lo[i] = lhs.storage.vb[2*i];
      hi[i] = rhs.storage.vb[2*i];
    } else {
      uint8_t temp1 = lhs.storage.vb[2*i];
      uint8_t temp2 = rhs.storage.vb[2*i];
      lo[i] = temp1 > MAX_INT8 ? MAX_INT8 : lhs.storage.vb[2*i];
      hi[i] = temp2 > MAX_INT8 ? MAX_INT8 : rhs.storage.vb[2*i];
    }
  }

  for (unsigned i = 0; i < 16; i++) {
    res[4 * i] = lo[2 * i];
    res[4 * i + 1] = lo[2 * i + 1];
    res[4 * i + 2] = hi[2 * i];
    res[4 * i + 3] = hi[2 * i + 1];
  }
}

void MPURegister::cprs_w(MPURegister &lhs, MPURegister &rhs) {
  int16_t lo[16];
  int16_t hi[16];
  int16_t *res = storage.vh;
  for (unsigned i = 0; i < 16; i++) {
    if (lhs.trunc) {
      lo[i] = lhs.storage.vh[2*i];
      hi[i] = rhs.storage.vh[2*i];
    } else {
      uint16_t temp1 = lhs.storage.vh[i];
      uint16_t temp2 = rhs.storage.vh[i];
      lo[i] = temp1 > MAX_INT16 ? MAX_INT16 : lhs.storage.vh[2*i];
      hi[i] = temp2 > MAX_INT16 ? MAX_INT16 : rhs.storage.vh[2*i];
    }
  }

  for (unsigned i = 0; i < 16; i++) {
    res[2 * i] = lo[i];
    res[2 * i + 1] = hi[i];
  }
}

//expd
void MPURegister::expd_ub0(MPURegister &lhs) {
  uint16_t *res = storage.vuh;
  uint16_t temp[64];
  for (unsigned i = 0; i < 64; i++) {
    temp[i] = uint16_t(lhs.storage.vub[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[2*i] = temp[4*i];
    res[2*i+1] = temp[4*i+1];
  }
}

void MPURegister::expd_ub1(MPURegister &lhs) {
  uint16_t *res = storage.vuh;
  uint16_t temp[64];
  for (unsigned i = 0; i < 64; i++) {
    temp[i] = uint16_t(lhs.storage.vub[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[2*i] = temp[4 * i + 2];
    res[2*i+1] = temp[4 * i + 3];
  }
}

void MPURegister::expd_uh0(MPURegister &lhs) {
  uint32_t *res = storage.vuw;
  uint32_t temp[32];
  for (unsigned i = 0; i < 32; i++) {
    temp[i] = uint32_t(lhs.storage.vuh[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[i] = temp[2 * i];
  }
}

void MPURegister::expd_uh1(MPURegister &lhs) {
  uint32_t *res = storage.vuw;
  uint32_t temp[32];
  for (unsigned i = 0; i < 32; i++) {
    temp[i] = uint32_t(lhs.storage.vuh[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[i] = temp[2 * i + 1];
  }
}

void MPURegister::expd_b0(MPURegister &lhs) {
  int16_t *res = storage.vh;
  int16_t temp[64];
  for (unsigned i = 0; i < 64; i++) {
    temp[i] = int16_t(lhs.storage.vb[i]);
  }

  for (unsigned i = 0; i < 16; i++) {    
    res[2*i] = temp[4*i];
    res[2*i+1] = temp[4*i+1];
  }
}

void MPURegister::expd_b1(MPURegister &lhs) {
  int16_t *res = storage.vh;
  int16_t temp[64];
  for (unsigned i = 0; i < 64; i++) {
    temp[i] = int16_t(lhs.storage.vb[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[2*i] = temp[4*i+2];
    res[2*i+1] = temp[4*i+3];
  }
}

void MPURegister::expd_h0(MPURegister &lhs) {
  int32_t *res = storage.vw;
  int32_t temp[32];
  for (unsigned i = 0; i < 32; i++) {
    temp[i] = int32_t(lhs.storage.vh[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[i] = temp[2 * i];
  }
}

void MPURegister::expd_h1(MPURegister &rhs) {
  int32_t *res = storage.vw;
  int32_t temp[32];
  for (unsigned i = 0; i < 32; i++) {
    temp[i] = int32_t(rhs.storage.vh[i]);
  }

  for (unsigned i = 0; i < 16; i++) {
    res[i] = temp[2 * i + 1];
  }
}

void MPURegister::cmul(MPURegister &lhs, MPURegister &rhs, MPURegister &tps) {
  union {
    int16_t temp0_h[2];
    int32_t temp0_w;
  };
  union {
    int16_t temp1_h[2];
    int32_t temp1_w;
  };

  int16_t res[32];;
  int16_t *out   = storage.vh;

  int32_t sw0;
  int32_t sw1;
  int32_t sw2;
  int32_t sw3;

  for (unsigned i = 0; i < 16; i++) {
    sw0 = int32_t(lhs.storage.vh[2*i]) * rhs.storage.vh[2*i];
    sw0 <<= 1;

    sw1 = int32_t(lhs.storage.vh[2*i + 1]) * rhs.storage.vh[2*i + 1];
    sw1 <<= 1;

    sw2 = int32_t(lhs.storage.vh[2*i]) * rhs.storage.vh[2*i + 1];
    sw2 <<= 1;

    sw3 = int32_t(lhs.storage.vh[2*i + 1]) * rhs.storage.vh[2*i];
    sw3 <<= 1;

    temp0_w = sw0 - sw1;
    temp1_w = sw2 + sw3;

    res[2*i]   = temp0_h[1] + ((uint16_t)temp0_h[0] >> 15);
    res[2*i+1] = temp1_h[1] + ((uint16_t)temp1_h[0] >> 15);
  }
  for (unsigned i = 0; i < 8; i++) {
    int32_t m = int32_t(tps.storage.vh[4*i]) + int32_t(res[4*i]);
    out[4*i] = m > MAX_INT16 ? MAX_INT16 :
                  m < MIN_INT16 ? MIN_INT16 : tps.storage.vh[4*i] + res[4*i];
    int32_t n = int32_t(tps.storage.vh[4*i+1]) + int32_t(res[4*i + 1]);
    out[4*i + 1] = n > MAX_INT16 ? MAX_INT16 :
                  n < MIN_INT16 ? MIN_INT16 : tps.storage.vh[4*i+1] + res[4*i+1];
    int32_t c = int32_t(tps.storage.vh[4*i+2]) - int32_t(res[4*i+2]);
    out[4*i+2] = c > MAX_INT16 ? MAX_INT16 :
                  c < MIN_INT16 ? MIN_INT16 :tps.storage.vh[4*i+2] - res[4*i+2];
    int32_t d = int32_t(tps.storage.vh[4*i+3]) - int32_t(res[4*i+3]);
    out[4*i+3] = d > MAX_INT16 ? MAX_INT16 :
                  d < MIN_INT16 ? MIN_INT16 :tps.storage.vh[4*i+3] - res[4*i+3];
  }
}

void MPURegister::floatcmul(MPURegister &lhs, MPURegister &rhs) {
  float *res = storage.vfp;
  for (unsigned i = 0; i < 8; i++) {
    if (i & 0x1)
      res[2*i + 1] = lhs.storage.vfp[2*i] * rhs.storage.vfp[2*i] + lhs.storage.vfp[2*i+1] * rhs.storage.vfp[2*i+1];
    else
      res[2*i + 1] = lhs.storage.vfp[2*i] * rhs.storage.vfp[2*i] - lhs.storage.vfp[2*i+1] * rhs.storage.vfp[2*i+1];
    res[2*i] = 0;
  }
}
static const uint8_t widx[8][16] =
{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
 {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
 {2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5},
 {6, 7, 6, 7, 8, 9, 8, 9,10,11,10,11,12,13,12,13},
 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0},
 {1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2},
 {3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6},
 {7, 7, 8, 8, 9, 9,10,10,11,11,12,12,13,13,14,14}};

uint32_t * MPURegister::WShuffle(uint8_t type) {
  uint32_t *res = result.vuw;
  if (type == 0) return storage.vuw;
  else {
   for (unsigned i = 0; i < 16; i++)
     res[i] = storage.vuw[widx[type][i]];
  }
  return res;
}

uint32_t * MPURegister::fftind() {
  uint32_t *res = result.vuw;
  for (unsigned i = 0; i < 16; i++) {
    if (i & 0x1)
      res[(i >> 1) + 8] = storage.vuw[i];
    else
      res[i >> 1] = storage.vuw[i];
  }
  return res;
}

BiuContext &BiuContext::operator =(const MPURegister &rhs) {
  //assign KData={56'h0,KB3,KS3,KC3,KI3,18'h0,
  //              63 57 56 54 53 51 49  47  45
  //                KB2,KS2,KC2,KI2,KE2,
  //               44 42 41 39  37  35 33
  //                KB1,KS1,KC1,KI1,KE1,
  //               32 30 29 27  25  23 21
  //                KB0,KS0,KC0,KI0,KE0,
  //               20 18 17 15  13  11 9
  //                KM,2'b0,KGM,KL};
  //                8  1         0
  MPURegister temp = rhs;
  this->KL = temp[0] & 0x7;
  this->KG = (temp[0] >> 3) & 0x7;
  this->KM[0] = *(uint32_t *)&(((uint8_t *)temp)[1]);
  this->KM[1] = *(uint32_t *)&(((uint8_t *)temp)[5]);
  for (int32_t i = 0; i < 4; i++) {
    this->KE[i] = (*(uint32_t *)&(((uint8_t *)temp)[9 + i * 12])) & 0x3FFFF;
    this->KI[i] = ((*(uint32_t *)&(((uint8_t *)temp)[11 + i * 12])) >> 2) & 0x3FFFF;
    this->KC[i] = ((*(uint32_t *)&(((uint8_t *)temp)[13 + i * 12])) >> 4) & 0x3FFFF;
    this->KS[i] = ((*(uint32_t *)&(((uint8_t *)temp)[15 + i * 12])) >> 6) & 0x3FFFF;
    this->KB[i] = (*(uint32_t *)&(((uint8_t *)temp)[18 + i * 12])) & 0xFFFFFF;
  }
  return *this;
}

} // namespace MapuISA


