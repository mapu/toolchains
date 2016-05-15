#ifndef __ARCH_UCP_REMOTE_GDB_HH__
#define __ARCH_UCP_REMOTE_GDB_HH__

#include "arch/ucp/registers.hh"
#include "base/bitfield.hh"
#include "base/remote_gdb.hh"

class System;
class ThreadContext;

namespace UcpISA
{

    // The number of special regs depends on gdb.
    // Two 32-bit regs are packed into one 64-bit reg.
    const int GdbIntArchRegs = NumIntArchRegs / 2;
    const int GdbIntSpecialRegs = 6 / 2;
    const int GdbFloatArchRegs = NumFloatArchRegs / 2;
    const int GdbFloatSpecialRegs = 2 / 2;

    const int GdbIntRegs = GdbIntArchRegs + GdbIntSpecialRegs;
    const int GdbFloatRegs = GdbFloatArchRegs + GdbFloatSpecialRegs;
    const int GdbNumRegs = GdbIntRegs + GdbFloatRegs;

    class RemoteGDB : public BaseRemoteGDB
    {
      protected:
        Addr notTakenBkpt;
        Addr takenBkpt;

      public:
        RemoteGDB(System *_system, ThreadContext *tc);

      protected:
        bool acc(Addr addr, size_t len);

        void getregs();
        void setregs();

        void clearSingleStep();
        void setSingleStep();

        /* num: gcc/dwarf number
         * sz:  the number of bytes filled into buf
         * buf: container of register value
         */
        virtual bool getRegVal(uint32_t num, void *buf, uint32_t *sz);
        virtual bool setRegVal(uint32_t num, void *buf, uint32_t  sz);

        virtual bool getRegDesc(uint32_t num, char * desc);

      private:
        uint64_t
        pack(uint32_t lo, uint32_t hi)
        {
            return static_cast<uint64_t>(hi) << 32 | lo;
        }
        uint32_t
        unpackLo(uint64_t val)
        {
            return bits(val, 31, 0);
        }
        uint32_t
        unpackHi(uint64_t val)
        {
            return bits(val, 63, 32);
        }
    };
}

#endif /* __ARCH_UCP_REMOTE_GDB_H__ */
