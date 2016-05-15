#include "arch/ucp/interrupts.hh"
#include "arch/ucp/isa_traits.hh"
#include "base/trace.hh"
#include "cpu/thread_context.hh"
#include "debug/Interrupt.hh"

namespace UcpISA
{
void
Interrupts::post(int int_num, ThreadContext* tc)
{
    DPRINTF(Interrupt, "Interrupt %d posted\n", int_num);
}

void
Interrupts::post(int int_num, int index)
{
    fatal("Must use Thread Context when posting UCP Interrupts in M5");
}

void
Interrupts::clear(int int_num, ThreadContext* tc)
{
    DPRINTF(Interrupt, "Interrupt %d cleared\n", int_num);
}

void
Interrupts::clear(int int_num, int index)
{
    fatal("Must use Thread Context when clearing UCP Interrupts in M5");
}

void
Interrupts::clearAll(ThreadContext *tc)
{
    DPRINTF(Interrupt, "Interrupts all cleared\n");
}

void
Interrupts::clearAll()
{
    fatal("Must use Thread Context when clearing UCP Interrupts in M5");
}



Fault
Interrupts::getInterrupt(ThreadContext * tc)
{
    DPRINTF(Interrupt, "Interrupts getInterrupt\n");

    return NoFault;
}

bool
Interrupts::onCpuTimerInterrupt(ThreadContext * tc) const
{
    return false;
}

void
Interrupts::updateIntrInfo(ThreadContext *tc) const
{
    //Nothing needs to be done.
}

bool
Interrupts::interruptsPending(ThreadContext *tc) const
{
    return false;

}
}

UcpISA::Interrupts *
UcpInterruptsParams::create()
{
    return new UcpISA::Interrupts(this);
}
