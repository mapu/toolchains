#include "arch/ucp/idle_event.hh"
#include "arch/ucp/kernel_stats.hh"
#include "cpu/thread_context.hh"

using namespace UcpISA;

void
IdleStartEvent::process(ThreadContext *tc)
{
    fatal("Idle Start Event Not Defined for UCP ISA ");
}
