#ifndef __ARCH_UCP_INTERRUPT_HH__
#define __ARCH_UCP_INTERRUPT_HH__

#include <string>

#include "arch/ucp/faults.hh"
#include "base/compiler.hh"
#include "base/misc.hh"
#include "params/UcpInterrupts.hh"
#include "sim/serialize.hh"
#include "sim/sim_object.hh"

class BaseCPU;
class Checkpoint;

namespace UcpISA
{

class Interrupts : public SimObject
{
  public:
    typedef UcpInterruptsParams Params;

    const Params *
    params() const
    {
        return dynamic_cast<const Params *>(_params);
    }

    Interrupts(Params * p) : SimObject(p)
    {
        newInfoSet = false;
    }

    void
    setCPU(BaseCPU *_cpu)
    {}

    //  post(int int_num, int index) is responsible
    //  for posting an interrupt. It sets a bit
    //  in intstatus corresponding to Cause IP*. The
    //  MIPS register Cause is updated by updateIntrInfo
    //  which is called by checkInterrupts
    //
    void post(int int_num, ThreadContext *tc);
    void post(int int_num, int index);

    // clear(int int_num, int index) is responsible
    //  for clearing an interrupt. It clear a bit
    //  in intstatus corresponding to Cause IP*. The
    //  MIPS register Cause is updated by updateIntrInfo
    //  which is called by checkInterrupts
    //
    void clear(int int_num, ThreadContext* tc);
    void clear(int int_num, int index);

    //  clearAll() is responsible
    //  for clearing all interrupts. It clears all bits
    //  in intstatus corresponding to Cause IP*. The
    //  MIPS register Cause is updated by updateIntrInfo
    //  which is called by checkInterrupts
    //
    void clearAll(ThreadContext *tc);
    void clearAll();

    // getInterrupt(ThreadContext * tc) checks if an interrupt
    //  should be returned. It ands the interrupt mask and
    //  and interrupt pending bits to see if one exists. It
    //  also makes sure interrupts are enabled (IE) and
    //  that ERL and ERX are not set
    //
    Fault getInterrupt(ThreadContext *tc);

    // updateIntrInfo(ThreadContext *tc) const syncs the
    //  MIPS cause register with the instatus variable. instatus
    //  is essentially a copy of the MIPS cause[IP7:IP0]
    //
    void updateIntrInfo(ThreadContext *tc) const;
    bool interruptsPending(ThreadContext *tc) const;
    bool onCpuTimerInterrupt(ThreadContext *tc) const;

    bool
    checkInterrupts(ThreadContext *tc) const
    {
        return interruptsPending(tc);
    }


    void
    serialize(std::ostream &os)
    {
        fatal("Serialization of Interrupts Unimplemented for MIPS");
    }

    void
    unserialize(Checkpoint *cp, const std::string &section)
    {
        fatal("Unserialization of Interrupts Unimplemented for MIPS");
    }

  private:
    bool newInfoSet;
    int newIpl;
    int newSummary;
};

}

#endif // __ARCH_UCP_INTERRUPT_HH__
