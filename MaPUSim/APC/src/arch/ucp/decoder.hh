#ifndef __ARCH_UCP_DECODER_HH__
#define __ARCH_UCP_DECODER_HH__

#include "arch/generic/decode_cache.hh"
#include "arch/ucp/types.hh"
#include "base/misc.hh"
#include "base/types.hh"
#include "cpu/static_inst.hh"

class ThreadContext;

namespace UcpISA
{

class Decoder
{
  protected:
    ThreadContext * tc;
    //The extended machine instruction being generated
    ExtMachInst emi;
    bool instDone;
    bool instCached;

  public:
    Decoder(ThreadContext * _tc) : tc(_tc), instDone(false), instCached(false)
    {}

    ThreadContext *getTC()
    {
        return tc;
    }

    void
    setTC(ThreadContext *_tc)
    {
        tc = _tc;
    }

    void
    process()
    {
    }

    void
    reset()
    {
        instDone = false;
        instCached = false;
    }

    //Use this to give data to the decoder. This should be used
    //when there is control flow.
    void
    moreBytes(const PCState &pc, Addr fetchPC, ExtMachInst inst)
    {
        if (instCached) {
          if (((inst >> 28) & 0x7) == 0x5) {
            emi = (emi | (inst << 32ULL));
            emi &= (-1ULL >> 1ULL);
          }
        } else if (!instDone) {
          emi = inst;
          instCached = true;
        }
        if (inst >> 31) instCached = false;
        instDone = true;
    }

    void
    moreBytes(ExtMachInst inst) {
        emi = inst;
        instDone = true;
    }

    //Use this to give data to the decoder. This should be used
    //when there is control flow.
/*    void
    moreBytes(const PCState &pc, Addr fetchPC, MachInst inst)
    {
        emi = inst;
        instDone = true;
    }*/

    bool
    needMoreBytes()
    {
      return true;
    }

    bool
    instReady()
    {
        return instDone;
    }

  protected:
    /// A cache of decoded instruction objects.
    static GenericISA::BasicDecodeCache defaultCache;

  public:
    StaticInstPtr decodeInst(ExtMachInst mach_inst);

    /// Decode a machine instruction.
    /// @param mach_inst The binary instruction to decode.
    /// @retval A pointer to the corresponding StaticInst object.
    StaticInstPtr
    decode(ExtMachInst mach_inst, Addr addr)
    {
        return defaultCache.decode(this, mach_inst, addr);
    }

    StaticInstPtr
    decode(UcpISA::PCState &nextPC)
    {
        if (!instDone)
            return NULL;
        instDone = false;
        instCached = false;
        return decode(emi, nextPC.instAddr());
    }
};

} // namespace UcpISA

#endif // __ARCH_Ucp_DECODER_HH__
