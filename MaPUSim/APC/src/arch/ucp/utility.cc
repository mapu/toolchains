#include <cmath>

#include "arch/ucp/isa_traits.hh"
#include "arch/ucp/registers.hh"
#include "arch/ucp/utility.hh"
#include "arch/ucp/vtophys.hh"
#include "base/bitfield.hh"
#include "base/misc.hh"
#include "cpu/static_inst.hh"
#include "cpu/thread_context.hh"
#include "mem/fs_translating_port_proxy.hh"
#include "sim/serialize.hh"


using namespace UcpISA;
using namespace std;

namespace UcpISA {

uint64_t
getArgument(ThreadContext *tc, int &number, uint16_t size, bool fp)
{
    panic("getArgument() not implemented\n");
    M5_DUMMY_RETURN
}

uint64_t
fpConvert(ConvertType cvt_type, double fp_val)
{

    switch (cvt_type)
    {
      case SINGLE_TO_DOUBLE:   //done
        {
            double sdouble_val = fp_val;
            void  *sdouble_ptr = &sdouble_val;
            uint64_t sdp_bits  = *(uint64_t *) sdouble_ptr;
            return sdp_bits;
        }

      case DOUBLE_TO_SINGLE:
        {
            float wfloat_val   = fp_val;
            void  *wfloat_ptr  = &wfloat_val;
            uint64_t wfloat_bits = *(uint32_t *) wfloat_ptr;
            return wfloat_bits;
        }

      case DOUBLE_TO_WORD:
        {
            int32_t dword_val = (uint64_t) fp_val;
            void   *dword_ptr = &dword_val;
            uint64_t dwp_bits  = *(uint32_t *) dword_ptr;
            return dwp_bits;
        }

      case SINGLE_TO_WORD:  //done
        {
            int32_t sword_val  = (int32_t) fp_val;
            void  *sword_ptr   = &sword_val;
            uint64_t sword_bits= *(uint32_t *) sword_ptr;
            return sword_bits;
        }

      case SINGLE_TO_UWORD:
        {
            uint32_t sword_val  = (uint32_t) fp_val;
            void  *sword_ptr   = &sword_val;
            uint64_t sword_bits= *(uint32_t *) sword_ptr;
            return sword_bits;
        }

      case WORD_TO_SINGLE:  //done
        {
            float wfloat_val   = fp_val;
            void  *wfloat_ptr  = &wfloat_val;
            uint64_t wfloat_bits = *(uint32_t *) wfloat_ptr;
            return wfloat_bits;
        }

      case WORD_TO_DOUBLE:  //done
        {
            double wdouble_val = fp_val;
            void  *wdouble_ptr = &wdouble_val;
            uint64_t wdp_bits  = *(uint64_t *) wdouble_ptr;
            return wdp_bits;
        }

      case DOUBLE_TO_UWORD:
        {
            uint32_t dword_val = (uint32_t) fp_val;
            void   *dword_ptr = &dword_val;
            int64_t dwp_bits  = *(uint64_t *) dword_ptr;
            return dwp_bits;
        }

      default:
        panic("Invalid Floating Point Conversion Type (%d). See \"types.hh\" for List of Conversions\n",cvt_type);
        return 0;
    }
}

double
roundFP(double val, int digits)
{
    double digit_offset = pow(10.0,digits);
    val = val * digit_offset;
    val = val + 0.5;
    val = floor(val);
    val = val / digit_offset;
    return val;
}

double
truncFP(double val)
{
    int trunc_val = (int) val;
    return (double) trunc_val;
}

bool
isNan(void *val_ptr, int size)
{
    switch (size)
    {
      case 32:
        {
            uint32_t val_bits = *(uint32_t *) val_ptr;
            return (bits(val_bits, 30, 23) == 0xFF);
        }

      case 64:
        {
            uint64_t val_bits = *(uint64_t *) val_ptr;
            return (bits(val_bits, 62, 52) == 0x7FF);
        }

      default:
        panic("Type unsupported. Size mismatch\n");
    }
}


bool
isQnan(void *val_ptr, int size)
{
    switch (size)
    {
      case 32:
        {
            uint32_t val_bits = *(uint32_t *) val_ptr;
            return (bits(val_bits, 30, 22) == 0x1FE);
        }

      case 64:
        {
            uint64_t val_bits = *(uint64_t *) val_ptr;
            return (bits(val_bits, 62, 51) == 0xFFE);
        }

      default:
        panic("Type unsupported. Size mismatch\n");
    }
}

bool
isSnan(void *val_ptr, int size)
{
    switch (size)
    {
      case 32:
        {
            uint32_t val_bits = *(uint32_t *) val_ptr;
            return (bits(val_bits, 30, 22) == 0x1FF);
        }

      case 64:
        {
            uint64_t val_bits = *(uint64_t *) val_ptr;
            return (bits(val_bits, 62, 51) == 0xFFF);
        }

      default:
        panic("Type unsupported. Size mismatch\n");
    }
}

uint32_t CrtRsqrtSeed32(uint32_t crtRsqrtSeed32In){
  uint32_t crtRcpSedOut;
  switch(crtRsqrtSeed32In){
    case 0x0:     crtRcpSedOut = 0x6a;  break;
    case 0x01:    crtRcpSedOut = 0x66;  break;
    case 0x02:    crtRcpSedOut = 0x64;  break;
    case 0x03:    crtRcpSedOut = 0x60;  break;
    case 0x04:    crtRcpSedOut = 0x5e;  break;
    case 0x05:    crtRcpSedOut = 0x5c;  break;
    case 0x06:    crtRcpSedOut = 0x5a;  break;
    case 0x07:    crtRcpSedOut = 0x56;  break;
    case 0x08:    crtRcpSedOut = 0x54;  break;
    case 0x09:    crtRcpSedOut = 0x52;  break;
    case 0x0a:    crtRcpSedOut = 0x50;  break;
    case 0x0b:    crtRcpSedOut = 0x4e;  break;
    case 0x0c:    crtRcpSedOut = 0x4c;  break;
    case 0x0d:    crtRcpSedOut = 0x4a;  break;
    case 0x0e:    crtRcpSedOut = 0x46;  break;
    case 0x0f:    crtRcpSedOut = 0x44;  break;
    case 0x10:    crtRcpSedOut = 0x42;  break;
    case 0x11:    crtRcpSedOut = 0x40;  break;
    case 0x12:    crtRcpSedOut = 0x3e;  break;
    case 0x13:    crtRcpSedOut = 0x3c;  break;
    case 0x14:    crtRcpSedOut = 0x3c;  break;
    case 0x15:    crtRcpSedOut = 0x3a;  break;
    case 0x16:    crtRcpSedOut = 0x38;  break;
    case 0x17:    crtRcpSedOut = 0x36;  break;
    case 0x18:    crtRcpSedOut = 0x34;  break;
    case 0x19:    crtRcpSedOut = 0x32;  break;
    case 0x1a:    crtRcpSedOut = 0x30;  break;
    case 0x1b:    crtRcpSedOut = 0x2e;  break;
    case 0x1c:    crtRcpSedOut = 0x2c;  break;
    case 0x1d:    crtRcpSedOut = 0x2c;  break;
    case 0x1e:    crtRcpSedOut = 0x2a;  break;
    case 0x1f:    crtRcpSedOut = 0x28;  break;
    case 0x20:    crtRcpSedOut = 0x26;  break;
    case 0x21:    crtRcpSedOut = 0x26;  break;
    case 0x22:    crtRcpSedOut = 0x24;  break;
    case 0x23:    crtRcpSedOut = 0x22;  break;
    case 0x24:    crtRcpSedOut = 0x20;  break;
    case 0x25:    crtRcpSedOut = 0x20;  break;
    case 0x26:    crtRcpSedOut = 0x1e;  break;
    case 0x27:    crtRcpSedOut = 0x1c;  break;
    case 0x28:    crtRcpSedOut = 0x1c;  break;
    case 0x29:    crtRcpSedOut = 0x1a;  break;
    case 0x2a:    crtRcpSedOut = 0x18;  break;
    case 0x2b:    crtRcpSedOut = 0x16;  break;
    case 0x2c:    crtRcpSedOut = 0x16;  break;
    case 0x2d:    crtRcpSedOut = 0x14;  break;
    case 0x2e:    crtRcpSedOut = 0x14;  break;
    case 0x2f:    crtRcpSedOut = 0x12;  break;
    case 0x30:    crtRcpSedOut = 0x10;  break;
    case 0x31:    crtRcpSedOut = 0x10;  break;
    case 0x32:    crtRcpSedOut = 0x0e;  break;
    case 0x33:    crtRcpSedOut = 0x0e;  break;
    case 0x34:    crtRcpSedOut = 0x0c;  break;
    case 0x35:    crtRcpSedOut = 0x0a;  break;
    case 0x36:    crtRcpSedOut = 0x0a;  break;
    case 0x37:    crtRcpSedOut = 0x08;  break;
    case 0x38:    crtRcpSedOut = 0x08;  break;
    case 0x39:    crtRcpSedOut = 0x06;  break;
    case 0x3a:    crtRcpSedOut = 0x06;  break;
    case 0x3b:    crtRcpSedOut = 0x04;  break;
    case 0x3c:    crtRcpSedOut = 0x04;  break;
    case 0x3d:    crtRcpSedOut = 0x02;  break;
    case 0x3e:    crtRcpSedOut = 0x02;  break;
    case 0x3f:    crtRcpSedOut = 0x00;  break;
    case 0x40:    crtRcpSedOut = 0xff;  break;
    case 0x41:    crtRcpSedOut = 0xfc;  break;
    case 0x42:    crtRcpSedOut = 0xf8;  break;
    case 0x43:    crtRcpSedOut = 0xf4;  break;
    case 0x44:    crtRcpSedOut = 0xf0;  break;
    case 0x45:    crtRcpSedOut = 0xec;  break;
    case 0x46:    crtRcpSedOut = 0xe8;  break;
    case 0x47:    crtRcpSedOut = 0xe6;  break;
    case 0x48:    crtRcpSedOut = 0xe2;  break;
    case 0x49:    crtRcpSedOut = 0xde;  break;
    case 0x4a:    crtRcpSedOut = 0xdc;  break;
    case 0x4b:    crtRcpSedOut = 0xd8;  break;
    case 0x4c:    crtRcpSedOut = 0xd4;  break;
    case 0x4d:    crtRcpSedOut = 0xd2;  break;
    case 0x4e:    crtRcpSedOut = 0xce;  break;
    case 0x4f:    crtRcpSedOut = 0xcc;  break;
    case 0x50:    crtRcpSedOut = 0xc8;  break;
    case 0x51:    crtRcpSedOut = 0xc6;  break;
    case 0x52:    crtRcpSedOut = 0xc4;  break;
    case 0x53:    crtRcpSedOut = 0xc0;  break;
    case 0x54:    crtRcpSedOut = 0xbe;  break;
    case 0x55:    crtRcpSedOut = 0xbc;  break;
    case 0x56:    crtRcpSedOut = 0xb8;  break;
    case 0x57:    crtRcpSedOut = 0xb6;  break;
    case 0x58:    crtRcpSedOut = 0xb4;  break;
    case 0x59:    crtRcpSedOut = 0xb2;  break;
    case 0x5a:    crtRcpSedOut = 0xae;  break;
    case 0x5b:    crtRcpSedOut = 0xac;  break;
    case 0x5c:    crtRcpSedOut = 0xaa;  break;
    case 0x5d:    crtRcpSedOut = 0xa8;  break;
    case 0x5e:    crtRcpSedOut = 0xa6;  break;
    case 0x5f:    crtRcpSedOut = 0xa4;  break;
    case 0x60:    crtRcpSedOut = 0xa2;  break;
    case 0x61:    crtRcpSedOut = 0x9e;  break;
    case 0x62:    crtRcpSedOut = 0x9c;  break;
    case 0x63:    crtRcpSedOut = 0x9a;  break;
    case 0x64:    crtRcpSedOut = 0x98;  break;
    case 0x65:    crtRcpSedOut = 0x96;  break;
    case 0x66:    crtRcpSedOut = 0x94;  break;
    case 0x67:    crtRcpSedOut = 0x92;  break;
    case 0x68:    crtRcpSedOut = 0x90;  break;
    case 0x69:    crtRcpSedOut = 0x8e;  break;
    case 0x6a:    crtRcpSedOut = 0x8c;  break;
    case 0x6b:    crtRcpSedOut = 0x8a;  break;
    case 0x6c:    crtRcpSedOut = 0x8a;  break;
    case 0x6d:    crtRcpSedOut = 0x88;  break;
    case 0x6e:    crtRcpSedOut = 0x86;  break;
    case 0x6f:    crtRcpSedOut = 0x84;  break;
    case 0x70:    crtRcpSedOut = 0x82;  break;
    case 0x71:    crtRcpSedOut = 0x80;  break;
    case 0x72:    crtRcpSedOut = 0x7e;  break;
    case 0x73:    crtRcpSedOut = 0x7c;  break;
    case 0x74:    crtRcpSedOut = 0x7c;  break;
    case 0x75:    crtRcpSedOut = 0x7a;  break;
    case 0x76:    crtRcpSedOut = 0x78;  break;
    case 0x77:    crtRcpSedOut = 0x76;  break;
    case 0x78:    crtRcpSedOut = 0x74;  break;
    case 0x79:    crtRcpSedOut = 0x74;  break;
    case 0x7a:    crtRcpSedOut = 0x72;  break;
    case 0x7b:    crtRcpSedOut = 0x70;  break;
    case 0x7c:    crtRcpSedOut = 0x6e;  break;
    case 0x7d:    crtRcpSedOut = 0x6e;  break;
    case 0x7e:    crtRcpSedOut = 0x6c;  break;
    case 0x7f:    crtRcpSedOut = 0x6a;  break;
    default  :    crtRcpSedOut = 0xff;  break;
  }
  return crtRcpSedOut;
}

uint64_t CrtRsqrtSeed64(uint64_t crtRsqrtSeed64In){
  uint64_t crtRcpSed64Out;
  switch(crtRsqrtSeed64In){
    case 0x00:    crtRcpSed64Out = 0xd4;  break;
    case 0x01:    crtRcpSed64Out = 0xd1;  break;
    case 0x02:    crtRcpSed64Out = 0xce;  break;
    case 0x03:    crtRcpSed64Out = 0xcb;  break;
    case 0x04:    crtRcpSed64Out = 0xc9;  break;
    case 0x05:    crtRcpSed64Out = 0xc6;  break;
    case 0x06:    crtRcpSed64Out = 0xc3;  break;
    case 0x07:    crtRcpSed64Out = 0xc1;  break;
    case 0x08:    crtRcpSed64Out = 0xbe;  break;
    case 0x09:    crtRcpSed64Out = 0xbb;  break;
    case 0x0a:    crtRcpSed64Out = 0xb9;  break;
    case 0x0b:    crtRcpSed64Out = 0xb6;  break;
    case 0x0c:    crtRcpSed64Out = 0xb4;  break;
    case 0x0d:    crtRcpSed64Out = 0xb1;  break;
    case 0x0e:    crtRcpSed64Out = 0xaf;  break;
    case 0x0f:    crtRcpSed64Out = 0xad;  break;
    case 0x10:    crtRcpSed64Out = 0xaa;  break;
    case 0x11:    crtRcpSed64Out = 0xa8;  break;
    case 0x12:    crtRcpSed64Out = 0xa5;  break;
    case 0x13:    crtRcpSed64Out = 0xa3;  break;
    case 0x14:    crtRcpSed64Out = 0xa1;  break;
    case 0x15:    crtRcpSed64Out = 0x9f;  break;
    case 0x16:    crtRcpSed64Out = 0x9c;  break;
    case 0x17:    crtRcpSed64Out = 0x9a;  break;
    case 0x18:    crtRcpSed64Out = 0x98;  break;
    case 0x19:    crtRcpSed64Out = 0x96;  break;
    case 0x1a:    crtRcpSed64Out = 0x94;  break;
    case 0x1b:    crtRcpSed64Out = 0x91;  break;
    case 0x1c:    crtRcpSed64Out = 0x8f;  break;
    case 0x1d:    crtRcpSed64Out = 0x8d;  break;
    case 0x1e:    crtRcpSed64Out = 0x8b;  break;
    case 0x1f:    crtRcpSed64Out = 0x89;  break;
    case 0x20:    crtRcpSed64Out = 0x87;  break;
    case 0x21:    crtRcpSed64Out = 0x85;  break;
    case 0x22:    crtRcpSed64Out = 0x83;  break;
    case 0x23:    crtRcpSed64Out = 0x81;  break;
    case 0x24:    crtRcpSed64Out = 0x7f;  break;
    case 0x25:    crtRcpSed64Out = 0x7d;  break;
    case 0x26:    crtRcpSed64Out = 0x7b;  break;
    case 0x27:    crtRcpSed64Out = 0x79;  break;
    case 0x28:    crtRcpSed64Out = 0x78;  break;
    case 0x29:    crtRcpSed64Out = 0x76;  break;
    case 0x2a:    crtRcpSed64Out = 0x74;  break;
    case 0x2b:    crtRcpSed64Out = 0x72;  break;
    case 0x2c:    crtRcpSed64Out = 0x70;  break;
    case 0x2d:    crtRcpSed64Out = 0x6e;  break;
    case 0x2e:    crtRcpSed64Out = 0x6d;  break;
    case 0x2f:    crtRcpSed64Out = 0x6b;  break;
    case 0x30:    crtRcpSed64Out = 0x69;  break;
    case 0x31:    crtRcpSed64Out = 0x67;  break;
    case 0x32:    crtRcpSed64Out = 0x66;  break;
    case 0x33:    crtRcpSed64Out = 0x64;  break;
    case 0x34:    crtRcpSed64Out = 0x62;  break;
    case 0x35:    crtRcpSed64Out = 0x60;  break;
    case 0x36:    crtRcpSed64Out = 0x5f;  break;
    case 0x37:    crtRcpSed64Out = 0x5d;  break;
    case 0x38:    crtRcpSed64Out = 0x5b;  break;
    case 0x39:    crtRcpSed64Out = 0x5a;  break;
    case 0x3a:    crtRcpSed64Out = 0x58;  break;
    case 0x3b:    crtRcpSed64Out = 0x57;  break;
    case 0x3c:    crtRcpSed64Out = 0x55;  break;
    case 0x3d:    crtRcpSed64Out = 0x53;  break;
    case 0x3e:    crtRcpSed64Out = 0x52;  break;
    case 0x3f:    crtRcpSed64Out = 0x50;  break;
    case 0x40:    crtRcpSed64Out = 0x4f;  break;
    case 0x41:    crtRcpSed64Out = 0x4d;  break;
    case 0x42:    crtRcpSed64Out = 0x4c;  break;
    case 0x43:    crtRcpSed64Out = 0x4a;  break;
    case 0x44:    crtRcpSed64Out = 0x49;  break;
    case 0x45:    crtRcpSed64Out = 0x47;  break;
    case 0x46:    crtRcpSed64Out = 0x46;  break;
    case 0x47:    crtRcpSed64Out = 0x44;  break;
    case 0x48:    crtRcpSed64Out = 0x43;  break;
    case 0x49:    crtRcpSed64Out = 0x41;  break;
    case 0x4a:    crtRcpSed64Out = 0x40;  break;
    case 0x4b:    crtRcpSed64Out = 0x3e;  break;
    case 0x4c:    crtRcpSed64Out = 0x3d;  break;
    case 0x4d:    crtRcpSed64Out = 0x3c;  break;
    case 0x4e:    crtRcpSed64Out = 0x3a;  break;
    case 0x4f:    crtRcpSed64Out = 0x39;  break;
    case 0x50:    crtRcpSed64Out = 0x38;  break;
    case 0x51:    crtRcpSed64Out = 0x36;  break;
    case 0x52:    crtRcpSed64Out = 0x35;  break;
    case 0x53:    crtRcpSed64Out = 0x33;  break;
    case 0x54:    crtRcpSed64Out = 0x32;  break;
    case 0x55:    crtRcpSed64Out = 0x31;  break;
    case 0x56:    crtRcpSed64Out = 0x2f;  break;
    case 0x57:    crtRcpSed64Out = 0x2e;  break;
    case 0x58:    crtRcpSed64Out = 0x2d;  break;
    case 0x59:    crtRcpSed64Out = 0x2c;  break;
    case 0x5a:    crtRcpSed64Out = 0x2a;  break;
    case 0x5b:    crtRcpSed64Out = 0x29;  break;
    case 0x5c:    crtRcpSed64Out = 0x28;  break;
    case 0x5d:    crtRcpSed64Out = 0x27;  break;
    case 0x5e:    crtRcpSed64Out = 0x25;  break;
    case 0x5f:    crtRcpSed64Out = 0x24;  break;
    case 0x60:    crtRcpSed64Out = 0x23;  break;
    case 0x61:    crtRcpSed64Out = 0x22;  break;
    case 0x62:    crtRcpSed64Out = 0x20;  break;
    case 0x63:    crtRcpSed64Out = 0x1f;  break;
    case 0x64:    crtRcpSed64Out = 0x1e;  break;
    case 0x65:    crtRcpSed64Out = 0x1d;  break;
    case 0x66:    crtRcpSed64Out = 0x1c;  break;
    case 0x67:    crtRcpSed64Out = 0x1a;  break;
    case 0x68:    crtRcpSed64Out = 0x19;  break;
    case 0x69:    crtRcpSed64Out = 0x18;  break;
    case 0x6a:    crtRcpSed64Out = 0x17;  break;
    case 0x6b:    crtRcpSed64Out = 0x16;  break;
    case 0x6c:    crtRcpSed64Out = 0x15;  break;
    case 0x6d:    crtRcpSed64Out = 0x14;  break;
    case 0x6e:    crtRcpSed64Out = 0x13;  break;
    case 0x6f:    crtRcpSed64Out = 0x11;  break;
    case 0x70:    crtRcpSed64Out = 0x10;  break;
    case 0x71:    crtRcpSed64Out = 0xf;   break;
    case 0x72:    crtRcpSed64Out = 0xe;   break;
    case 0x73:    crtRcpSed64Out = 0xd;   break;
    case 0x74:    crtRcpSed64Out = 0xc;   break;
    case 0x75:    crtRcpSed64Out = 0xb;   break;
    case 0x76:    crtRcpSed64Out = 0xa;   break;
    case 0x77:    crtRcpSed64Out = 0x9;   break;
    case 0x78:    crtRcpSed64Out = 0x8;   break;
    case 0x79:    crtRcpSed64Out = 0x7;   break;
    case 0x7a:    crtRcpSed64Out = 0x6;   break;
    case 0x7b:    crtRcpSed64Out = 0x5;   break;
    case 0x7c:    crtRcpSed64Out = 0x4;   break;
    case 0x7d:    crtRcpSed64Out = 0x3;   break;
    case 0x7e:    crtRcpSed64Out = 0x2;   break;
    case 0x7f:    crtRcpSed64Out = 0x1;   break;
    case 0x80:    crtRcpSed64Out = 0x1ff; break;
    case 0x81:    crtRcpSed64Out = 0x1fc; break;
    case 0x82:    crtRcpSed64Out = 0x1f8; break;
    case 0x83:    crtRcpSed64Out = 0x1f4; break;
    case 0x84:    crtRcpSed64Out = 0x1f0; break;
    case 0x85:    crtRcpSed64Out = 0x1ec; break;
    case 0x86:    crtRcpSed64Out = 0x1e8; break;
    case 0x87:    crtRcpSed64Out = 0x1e5; break;
    case 0x88:    crtRcpSed64Out = 0x1e1; break;
    case 0x89:    crtRcpSed64Out = 0x1dd; break;
    case 0x8a:    crtRcpSed64Out = 0x1da; break;
    case 0x8b:    crtRcpSed64Out = 0x1d6; break;
    case 0x8c:    crtRcpSed64Out = 0x1d3; break;
    case 0x8d:    crtRcpSed64Out = 0x1cf; break;
    case 0x8e:    crtRcpSed64Out = 0x1cc; break;
    case 0x8f:    crtRcpSed64Out = 0x1c8; break;
    case 0x90:    crtRcpSed64Out = 0x1c5; break;
    case 0x91:    crtRcpSed64Out = 0x1c2; break;
    case 0x92:    crtRcpSed64Out = 0x1be; break;
    case 0x93:    crtRcpSed64Out = 0x1bb; break;
    case 0x94:    crtRcpSed64Out = 0x1b8; break;
    case 0x95:    crtRcpSed64Out = 0x1b5; break;
    case 0x96:    crtRcpSed64Out = 0x1b1; break;
    case 0x97:    crtRcpSed64Out = 0x1ae; break;
    case 0x98:    crtRcpSed64Out = 0x1ab; break;
    case 0x99:    crtRcpSed64Out = 0x1a8; break;
    case 0x9a:    crtRcpSed64Out = 0x1a5; break;
    case 0x9b:    crtRcpSed64Out = 0x1a2; break;
    case 0x9c:    crtRcpSed64Out = 0x19f; break;
    case 0x9d:    crtRcpSed64Out = 0x19c; break;
    case 0x9e:    crtRcpSed64Out = 0x199; break;
    case 0x9f:    crtRcpSed64Out = 0x196; break;
    case 0xa0:    crtRcpSed64Out = 0x193; break;
    case 0xa1:    crtRcpSed64Out = 0x191; break;
    case 0xa2:    crtRcpSed64Out = 0x18e; break;
    case 0xa3:    crtRcpSed64Out = 0x18b; break;
    case 0xa4:    crtRcpSed64Out = 0x188; break;
    case 0xa5:    crtRcpSed64Out = 0x185; break;
    case 0xa6:    crtRcpSed64Out = 0x183; break;
    case 0xa7:    crtRcpSed64Out = 0x180; break;
    case 0xa8:    crtRcpSed64Out = 0x17d; break;
    case 0xa9:    crtRcpSed64Out = 0x17b; break;
    case 0xaa:    crtRcpSed64Out = 0x178; break;
    case 0xab:    crtRcpSed64Out = 0x175; break;
    case 0xac:    crtRcpSed64Out = 0x173; break;
    case 0xad:    crtRcpSed64Out = 0x170; break;
    case 0xae:    crtRcpSed64Out = 0x16e; break;
    case 0xaf:    crtRcpSed64Out = 0x16b; break;
    case 0xb0:    crtRcpSed64Out = 0x169; break;
    case 0xb1:    crtRcpSed64Out = 0x166; break;
    case 0xb2:    crtRcpSed64Out = 0x164; break;
    case 0xb3:    crtRcpSed64Out = 0x161; break;
    case 0xb4:    crtRcpSed64Out = 0x15f; break;
    case 0xb5:    crtRcpSed64Out = 0x15d; break;
    case 0xb6:    crtRcpSed64Out = 0x15a; break;
    case 0xb7:    crtRcpSed64Out = 0x158; break;
    case 0xb8:    crtRcpSed64Out = 0x156; break;
    case 0xb9:    crtRcpSed64Out = 0x153; break;
    case 0xba:    crtRcpSed64Out = 0x151; break;
    case 0xbb:    crtRcpSed64Out = 0x14f; break;
    case 0xbc:    crtRcpSed64Out = 0x14c; break;
    case 0xbd:    crtRcpSed64Out = 0x14a; break;
    case 0xbe:    crtRcpSed64Out = 0x148; break;
    case 0xbf:    crtRcpSed64Out = 0x146; break;
    case 0xc0:    crtRcpSed64Out = 0x144; break;
    case 0xc1:    crtRcpSed64Out = 0x141; break;
    case 0xc2:    crtRcpSed64Out = 0x13f; break;
    case 0xc3:    crtRcpSed64Out = 0x13d; break;
    case 0xc4:    crtRcpSed64Out = 0x13b; break;
    case 0xc5:    crtRcpSed64Out = 0x139; break;
    case 0xc6:    crtRcpSed64Out = 0x137; break;
    case 0xc7:    crtRcpSed64Out = 0x135; break;
    case 0xc8:    crtRcpSed64Out = 0x133; break;
    case 0xc9:    crtRcpSed64Out = 0x131; break;
    case 0xca:    crtRcpSed64Out = 0x12f; break;
    case 0xcb:    crtRcpSed64Out = 0x12d; break;
    case 0xcc:    crtRcpSed64Out = 0x12b; break;
    case 0xcd:    crtRcpSed64Out = 0x129; break;
    case 0xce:    crtRcpSed64Out = 0x127; break;
    case 0xcf:    crtRcpSed64Out = 0x125; break;
    case 0xd0:    crtRcpSed64Out = 0x123; break;
    case 0xd1:    crtRcpSed64Out = 0x121; break;
    case 0xd2:    crtRcpSed64Out = 0x11f; break;
    case 0xd3:    crtRcpSed64Out = 0x11d; break;
    case 0xd4:    crtRcpSed64Out = 0x11b; break;
    case 0xd5:    crtRcpSed64Out = 0x119; break;
    case 0xd6:    crtRcpSed64Out = 0x117; break;
    case 0xd7:    crtRcpSed64Out = 0x116; break;
    case 0xd8:    crtRcpSed64Out = 0x114; break;
    case 0xd9:    crtRcpSed64Out = 0x112; break;
    case 0xda:    crtRcpSed64Out = 0x110; break;
    case 0xdb:    crtRcpSed64Out = 0x10e; break;
    case 0xdc:    crtRcpSed64Out = 0x10d; break;
    case 0xdd:    crtRcpSed64Out = 0x10b; break;
    case 0xde:    crtRcpSed64Out = 0x109; break;
    case 0xdf:    crtRcpSed64Out = 0x107; break;
    case 0xe0:    crtRcpSed64Out = 0x106; break;
    case 0xe1:    crtRcpSed64Out = 0x104; break;
    case 0xe2:    crtRcpSed64Out = 0x102; break;
    case 0xe3:    crtRcpSed64Out = 0x100; break;
    case 0xe4:    crtRcpSed64Out = 0xff;  break;
    case 0xe5:    crtRcpSed64Out = 0xfd;  break;
    case 0xe6:    crtRcpSed64Out = 0xfb;  break;
    case 0xe7:    crtRcpSed64Out = 0xfa;  break;
    case 0xe8:    crtRcpSed64Out = 0xf8;  break;
    case 0xe9:    crtRcpSed64Out = 0xf6;  break;
    case 0xea:    crtRcpSed64Out = 0xf5;  break;
    case 0xeb:    crtRcpSed64Out = 0xf3;  break;
    case 0xec:    crtRcpSed64Out = 0xf2;  break;
    case 0xed:    crtRcpSed64Out = 0xf0;  break;
    case 0xee:    crtRcpSed64Out = 0xee;  break;
    case 0xef:    crtRcpSed64Out = 0xed;  break;
    case 0xf0:    crtRcpSed64Out = 0xeb;  break;
    case 0xf1:    crtRcpSed64Out = 0xea;  break;
    case 0xf2:    crtRcpSed64Out = 0xe8;  break;
    case 0xf3:    crtRcpSed64Out = 0xe7;  break;
    case 0xf4:    crtRcpSed64Out = 0xe5;  break;
    case 0xf5:    crtRcpSed64Out = 0xe4;  break;
    case 0xf6:    crtRcpSed64Out = 0xe2;  break;
    case 0xf7:    crtRcpSed64Out = 0xe1;  break;
    case 0xf8:    crtRcpSed64Out = 0xdf;  break;
    case 0xf9:    crtRcpSed64Out = 0xde;  break;
    case 0xfa:    crtRcpSed64Out = 0xdc;  break;
    case 0xfb:    crtRcpSed64Out = 0xdb;  break;
    case 0xfc:    crtRcpSed64Out = 0xd9;  break;
    case 0xfd:    crtRcpSed64Out = 0xd8;  break;
    case 0xfe:    crtRcpSed64Out = 0xd6;  break;
    case 0xff:    crtRcpSed64Out = 0xd5;  break;
    default:      crtRcpSed64Out = 0x0;   break;
  }
  return crtRcpSed64Out;

}

uint32_t recips(uint32_t recipsIn){
  uint32_t recipsOut;
  uint32_t sig1;
  uint32_t exp0, exp1;
  uint32_t man0, man1;
  sig1 = recipsIn >> 31;
  exp0 = (recipsIn & 0x7FFFFFFF) >> 23; 
  exp1 = (253 - exp0) & 0xFF;
  man0 = (recipsIn & 0x7FFFFF) >> 16;
  man1 = (man0==0x0)?0xFF:((256*256)/(128+man0) - 256)&0xfe;
  if( (exp0==0xFF) && (man0!=0x0) ){   //not a number
    recipsOut = 0xFFFFFFFF;
  } else if( ((exp0==0xFF) && (man0==0x0)) || (exp0==0xfe) || (exp0==0xfd) ) {  // endless
    recipsOut = recipsIn & 0x80000000;
  } else if (exp0==0x0) {                    // 0
    recipsOut = (recipsIn & 0x80000000) | 0x7F800000;
  } else {
    recipsOut =  (sig1 << 31) | (exp1 << 23) | (man1 << 15);
  }
  return recipsOut;
}

uint64_t recipdr(uint64_t recipdrIn){
  uint64_t recipdrOut;
  uint64_t sig1;
  uint64_t exp0, exp1;
  uint64_t man0, man1;
  sig1 = recipdrIn >> 63;
  exp0 = (recipdrIn & 0x7FFFFFFFFFFFFFFF) >> 52; 
  exp1 = (2045 - exp0) & 0x7FF;
  man0 = (recipdrIn & 0xFFFFFFFFFFFFF) >> 44;
  man1 = (man0==0x0)?0x1FF:((512*512)/(256+man0) - 512)&0x1FF;;

  if( (exp0==0x7FF) && (man0!=0x0) ){   //not a number
    recipdrOut = 0xFFFFFFFFFFFFFFFF;
  } else if( ((exp0==0x7FF) && (man0==0x0)) || (exp0==0x7fe) || (exp0==0x7fd) ) {  // endless
    recipdrOut = recipdrIn & 0x8000000000000000;
  } else if (exp0==0x0) {                    // 0
    recipdrOut = (recipdrIn & 0x8000000000000000) | 0x7FF0000000000000;
  } else {
    recipdrOut =  (sig1 << 63) | (exp1 << 52) | (man1 << 43);
  }
  return recipdrOut;
}

uint32_t rsqrts(uint32_t rsqrtsIn){
  uint32_t rsqrtsOut;
  uint32_t sig1;
  uint32_t exp0, exp1;
  uint32_t man0, man1, man2;
  sig1 = rsqrtsIn >> 31;
  exp0 = (rsqrtsIn & 0x7FFFFFFF) >> 23; 
  exp1 = (exp0&0x1)?(189-(exp0>>1)):(190-(exp0>>1));
  man0 = (rsqrtsIn & 0xFFFFFF) >> 17; //contain exp0[0]
  man2 = (rsqrtsIn & 0x7FFFFF) >> 16; //
  //man1 = CrtRsqrtSeed32(man0);
  man1 = (exp0&0x1) ? (man0==0x40)?0xFF : ((int)(256.0 * ( sqrt((float)256/(float)man0)-1.0)))&0xFE:((int)(256.0*( sqrt((float)128/(float)(64 + man0)) - 1.0 )))&0xFE;

  if( (exp0==0xFF) && (man2!=0x0) ){   //not a number
    rsqrtsOut = 0xFFFFFFFF;
  } else if (exp0==0x0) {                    // 0
    rsqrtsOut = (rsqrtsIn & 0x80000000) | 0x7F800000;
  } else if (sig1==0x1){                    //-
    rsqrtsOut = 0xFFFFFFFF;
  } else if( ((exp0==0xFF) && (man2==0x0))) {  // endless
    rsqrtsOut = rsqrtsIn & 0x80000000;
  } else {
    rsqrtsOut =  (sig1 << 31) | (exp1 << 23) | (man1 << 15);
  }
  return rsqrtsOut;
}

uint64_t rsqrtdr(uint64_t rsqrtdrIn){
  uint64_t rsqrtdrOut;
  uint64_t sig1;
  uint64_t exp0, exp1;
  uint64_t man0, man1, man2;
  sig1 = rsqrtdrIn >> 63;
  exp0 = (rsqrtdrIn & 0x7FFFFFFFFFFFFFFF) >> 52; 
  exp1 = (exp0&0x1)?(1533-(exp0>>1)):(1534-(exp0>>1));
  man0 = (rsqrtdrIn & 0x1FFFFFFFFFFFFF) >> 45; //contain exp0[0]
  man2 = (rsqrtdrIn & 0xFFFFFFFFFFFFF) >> 44;
  //man1 = CrtRsqrtSeed64(man0);
  man1 = (exp0&0x1) ? (man0==0x80)?0x1FF : 512.0 * ( sqrt((float)512/(float)man0)-1.0):512.0*( sqrt((float)256/(128+man0)) - 1.0 );

  if( (exp0==0x7FF) && (man2!=0x0) ){   //not a number
    rsqrtdrOut = 0xFFFFFFFFFFFFFFFF;
  } else if (exp0==0x0) {                    // 0
    rsqrtdrOut = (rsqrtdrIn & 0x8000000000000000) | 0x7FF0000000000000;
  } else if (sig1==0x1){                    //-
    rsqrtdrOut = 0xFFFFFFFFFFFFFFFF;
  } else if( ((exp0==0x7FF) && (man2==0x0))) {  // endless
    rsqrtdrOut = rsqrtdrIn & 0x8000000000000000;
  } else {
    rsqrtdrOut =  (sig1 << 63) | (exp1 << 52) | (man1 << 43);
  }
  return rsqrtdrOut;
}

void
startupCPU(ThreadContext *tc, int cpuId)
{
    tc->activate(0/*tc->threadId()*/);
}

void
initCPU(ThreadContext *tc, int cpuId)
{}

void
copyRegs(ThreadContext *src, ThreadContext *dest)
{
    panic("Copy Regs Not Implemented Yet\n");
}

void
copyMiscRegs(ThreadContext *src, ThreadContext *dest)
{
    panic("Copy Misc. Regs Not Implemented Yet\n");
}
void
skipFunction(ThreadContext *tc)
{
    TheISA::PCState newPC = tc->pcState();
    newPC.set(tc->readIntReg(ReturnAddressReg));
    tc->pcState(newPC);
}

// CRC 24a/b
static const unsigned int crc_table_24a[256] = { 0x000000, 0x864cfb, 0x8ad50d, 0x0c99f6,
		0x93e6e1, 0x15aa1a, 0x1933ec, 0x9f7f17, 0xa18139, 0x27cdc2, 0x2b5434,
		0xad18cf, 0x3267d8, 0xb42b23, 0xb8b2d5, 0x3efe2e, 0xc54e89, 0x430272,
		0x4f9b84, 0xc9d77f, 0x56a868, 0xd0e493, 0xdc7d65, 0x5a319e, 0x64cfb0,
		0xe2834b, 0xee1abd, 0x685646, 0xf72951, 0x7165aa, 0x7dfc5c, 0xfbb0a7,
		0x0cd1e9, 0x8a9d12, 0x8604e4, 0x00481f, 0x9f3708, 0x197bf3, 0x15e205,
		0x93aefe, 0xad50d0, 0x2b1c2b, 0x2785dd, 0xa1c926, 0x3eb631, 0xb8faca,
		0xb4633c, 0x322fc7, 0xc99f60, 0x4fd39b, 0x434a6d, 0xc50696, 0x5a7981,
		0xdc357a, 0xd0ac8c, 0x56e077, 0x681e59, 0xee52a2, 0xe2cb54, 0x6487af,
		0xfbf8b8, 0x7db443, 0x712db5, 0xf7614e, 0x19a3d2, 0x9fef29, 0x9376df,
		0x153a24, 0x8a4533, 0x0c09c8, 0x00903e, 0x86dcc5, 0xb822eb, 0x3e6e10,
		0x32f7e6, 0xb4bb1d, 0x2bc40a, 0xad88f1, 0xa11107, 0x275dfc, 0xdced5b,
		0x5aa1a0, 0x563856, 0xd074ad, 0x4f0bba, 0xc94741, 0xc5deb7, 0x43924c,
		0x7d6c62, 0xfb2099, 0xf7b96f, 0x71f594, 0xee8a83, 0x68c678, 0x645f8e,
		0xe21375, 0x15723b, 0x933ec0, 0x9fa736, 0x19ebcd, 0x8694da, 0x00d821,
		0x0c41d7, 0x8a0d2c, 0xb4f302, 0x32bff9, 0x3e260f, 0xb86af4, 0x2715e3,
		0xa15918, 0xadc0ee, 0x2b8c15, 0xd03cb2, 0x567049, 0x5ae9bf, 0xdca544,
		0x43da53, 0xc596a8, 0xc90f5e, 0x4f43a5, 0x71bd8b, 0xf7f170, 0xfb6886,
		0x7d247d, 0xe25b6a, 0x641791, 0x688e67, 0xeec29c, 0x3347a4, 0xb50b5f,
		0xb992a9, 0x3fde52, 0xa0a145, 0x26edbe, 0x2a7448, 0xac38b3, 0x92c69d,
		0x148a66, 0x181390, 0x9e5f6b, 0x01207c, 0x876c87, 0x8bf571, 0x0db98a,
		0xf6092d, 0x7045d6, 0x7cdc20, 0xfa90db, 0x65efcc, 0xe3a337, 0xef3ac1,
		0x69763a, 0x578814, 0xd1c4ef, 0xdd5d19, 0x5b11e2, 0xc46ef5, 0x42220e,
		0x4ebbf8, 0xc8f703, 0x3f964d, 0xb9dab6, 0xb54340, 0x330fbb, 0xac70ac,
		0x2a3c57, 0x26a5a1, 0xa0e95a, 0x9e1774, 0x185b8f, 0x14c279, 0x928e82,
		0x0df195, 0x8bbd6e, 0x872498, 0x016863, 0xfad8c4, 0x7c943f, 0x700dc9,
		0xf64132, 0x693e25, 0xef72de, 0xe3eb28, 0x65a7d3, 0x5b59fd, 0xdd1506,
		0xd18cf0, 0x57c00b, 0xc8bf1c, 0x4ef3e7, 0x426a11, 0xc426ea, 0x2ae476,
		0xaca88d, 0xa0317b, 0x267d80, 0xb90297, 0x3f4e6c, 0x33d79a, 0xb59b61,
		0x8b654f, 0x0d29b4, 0x01b042, 0x87fcb9, 0x1883ae, 0x9ecf55, 0x9256a3,
		0x141a58, 0xefaaff, 0x69e604, 0x657ff2, 0xe33309, 0x7c4c1e, 0xfa00e5,
		0xf69913, 0x70d5e8, 0x4e2bc6, 0xc8673d, 0xc4fecb, 0x42b230, 0xddcd27,
		0x5b81dc, 0x57182a, 0xd154d1, 0x26359f, 0xa07964, 0xace092, 0x2aac69,
		0xb5d37e, 0x339f85, 0x3f0673, 0xb94a88, 0x87b4a6, 0x01f85d, 0x0d61ab,
		0x8b2d50, 0x145247, 0x921ebc, 0x9e874a, 0x18cbb1, 0xe37b16, 0x6537ed,
		0x69ae1b, 0xefe2e0, 0x709df7, 0xf6d10c, 0xfa48fa, 0x7c0401, 0x42fa2f,
		0xc4b6d4, 0xc82f22, 0x4e63d9, 0xd11cce, 0x575035, 0x5bc9c3, 0xdd8538 };

static const unsigned int crc_table_24b[256] = { 0x000000, 0x800063, 0x8000a5, 0x0000c6,
		0x800129, 0x00014a, 0x00018c, 0x8001ef, 0x800231, 0x000252, 0x000294,
		0x8002f7, 0x000318, 0x80037b, 0x8003bd, 0x0003de, 0x800401, 0x000462,
		0x0004a4, 0x8004c7, 0x000528, 0x80054b, 0x80058d, 0x0005ee, 0x000630,
		0x800653, 0x800695, 0x0006f6, 0x800719, 0x00077a, 0x0007bc, 0x8007df,
		0x800861, 0x000802, 0x0008c4, 0x8008a7, 0x000948, 0x80092b, 0x8009ed,
		0x00098e, 0x000a50, 0x800a33, 0x800af5, 0x000a96, 0x800b79, 0x000b1a,
		0x000bdc, 0x800bbf, 0x000c60, 0x800c03, 0x800cc5, 0x000ca6, 0x800d49,
		0x000d2a, 0x000dec, 0x800d8f, 0x800e51, 0x000e32, 0x000ef4, 0x800e97,
		0x000f78, 0x800f1b, 0x800fdd, 0x000fbe, 0x8010a1, 0x0010c2, 0x001004,
		0x801067, 0x001188, 0x8011eb, 0x80112d, 0x00114e, 0x001290, 0x8012f3,
		0x801235, 0x001256, 0x8013b9, 0x0013da, 0x00131c, 0x80137f, 0x0014a0,
		0x8014c3, 0x801405, 0x001466, 0x801589, 0x0015ea, 0x00152c, 0x80154f,
		0x801691, 0x0016f2, 0x001634, 0x801657, 0x0017b8, 0x8017db, 0x80171d,
		0x00177e, 0x0018c0, 0x8018a3, 0x801865, 0x001806, 0x8019e9, 0x00198a,
		0x00194c, 0x80192f, 0x801af1, 0x001a92, 0x001a54, 0x801a37, 0x001bd8,
		0x801bbb, 0x801b7d, 0x001b1e, 0x801cc1, 0x001ca2, 0x001c64, 0x801c07,
		0x001de8, 0x801d8b, 0x801d4d, 0x001d2e, 0x001ef0, 0x801e93, 0x801e55,
		0x001e36, 0x801fd9, 0x001fba, 0x001f7c, 0x801f1f, 0x802121, 0x002142,
		0x002184, 0x8021e7, 0x002008, 0x80206b, 0x8020ad, 0x0020ce, 0x002310,
		0x802373, 0x8023b5, 0x0023d6, 0x802239, 0x00225a, 0x00229c, 0x8022ff,
		0x002520, 0x802543, 0x802585, 0x0025e6, 0x802409, 0x00246a, 0x0024ac,
		0x8024cf, 0x802711, 0x002772, 0x0027b4, 0x8027d7, 0x002638, 0x80265b,
		0x80269d, 0x0026fe, 0x002940, 0x802923, 0x8029e5, 0x002986, 0x802869,
		0x00280a, 0x0028cc, 0x8028af, 0x802b71, 0x002b12, 0x002bd4, 0x802bb7,
		0x002a58, 0x802a3b, 0x802afd, 0x002a9e, 0x802d41, 0x002d22, 0x002de4,
		0x802d87, 0x002c68, 0x802c0b, 0x802ccd, 0x002cae, 0x002f70, 0x802f13,
		0x802fd5, 0x002fb6, 0x802e59, 0x002e3a, 0x002efc, 0x802e9f, 0x003180,
		0x8031e3, 0x803125, 0x003146, 0x8030a9, 0x0030ca, 0x00300c, 0x80306f,
		0x8033b1, 0x0033d2, 0x003314, 0x803377, 0x003298, 0x8032fb, 0x80323d,
		0x00325e, 0x803581, 0x0035e2, 0x003524, 0x803547, 0x0034a8, 0x8034cb,
		0x80340d, 0x00346e, 0x0037b0, 0x8037d3, 0x803715, 0x003776, 0x803699,
		0x0036fa, 0x00363c, 0x80365f, 0x8039e1, 0x003982, 0x003944, 0x803927,
		0x0038c8, 0x8038ab, 0x80386d, 0x00380e, 0x003bd0, 0x803bb3, 0x803b75,
		0x003b16, 0x803af9, 0x003a9a, 0x003a5c, 0x803a3f, 0x003de0, 0x803d83,
		0x803d45, 0x003d26, 0x803cc9, 0x003caa, 0x003c6c, 0x803c0f, 0x803fd1,
		0x003fb2, 0x003f74, 0x803f17, 0x003ef8, 0x803e9b, 0x803e5d, 0x003e3e };

void crc_sim(unsigned int &t_crc, unsigned char data_in, bool crc_type) {
	unsigned int tbl_idx;
	if (!crc_type) {//24a
		tbl_idx = ((t_crc >> 16) ^ data_in) & 0xff;
		t_crc = (crc_table_24a[tbl_idx] ^ (t_crc << 8)) & 0xffffff;
	} else {//24b
		tbl_idx = ((t_crc >> 16) ^ data_in) & 0xff;
		t_crc = (crc_table_24b[tbl_idx] ^ (t_crc << 8)) & 0xffffff;
	}
}

void bit2byte(unsigned char *byte_stream, unsigned char *bit_stream,
		unsigned int bit_len) {

	unsigned int i;
	unsigned char tmp = 0x0;
	for (i = 1; i <= bit_len; i++) {
		if (i % 8 != 0)
			tmp |= (*(bit_stream + i - 1) & 0x1) << (8 - i % 8);
		else {
			*(byte_stream + i / 8 - 1) = tmp | (*(bit_stream + i - 1) & 0x1);
			tmp = 0x0;
		}
	}
}

} // namespace UcpISA
