#include "arch/mapu/idle_event.hh"
#include "arch/mapu/kernel_stats.hh"
#include "cpu/thread_context.hh"

using namespace MapuISA;

void
IdleStartEvent::process(ThreadContext *tc)
{
    fatal("Idle Start Event Not Defined for MAPU ISA ");
}
