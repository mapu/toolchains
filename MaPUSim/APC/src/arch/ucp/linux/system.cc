/**
 * @file
 * This code loads the linux kernel, console, pal and patches certain
 * functions.  The symbol tables are loaded so that traces can show
 * the executing function and we can skip functions. Various delay
 * loops are skipped and their final values manually computed to speed
 * up boot time.
 */

#include "arch/ucp/linux/system.hh"
#include "arch/ucp/linux/threadinfo.hh"
#include "arch/ucp/idle_event.hh"
#include "arch/ucp/system.hh"
#include "arch/vtophys.hh"
#include "base/loader/symtab.hh"
#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "debug/Thread.hh"
#include "dev/platform.hh"
#include "kern/linux/events.hh"
#include "kern/linux/printk.hh"
#include "mem/physical.hh"
#include "mem/port.hh"
#include "sim/arguments.hh"
#include "sim/byteswap.hh"

using namespace std;
using namespace UcpISA;
using namespace Linux;

LinuxUcpSystem::LinuxUcpSystem(Params *p)
    : UcpSystem(p)
{
}

LinuxUcpSystem::~LinuxUcpSystem()
{
}


void
LinuxUcpSystem::setDelayLoop(ThreadContext *tc)
{
    panic("setDelayLoop not implemented.\n");
}


void
LinuxUcpSystem::SkipDelayLoopEvent::process(ThreadContext *tc)
{
    SkipFuncEvent::process(tc);
    // calculate and set loops_per_jiffy
    ((LinuxUcpSystem *)tc->getSystemPtr())->setDelayLoop(tc);
}

void
LinuxUcpSystem::PrintThreadInfo::process(ThreadContext *tc)
{
    Linux::ThreadInfo ti(tc);

    DPRINTF(Thread, "Currently Executing Thread %s, pid %d, started at: %d\n",
            ti.curTaskName(), ti.curTaskPID(), ti.curTaskStart());
}

LinuxUcpSystem *
LinuxUcpSystemParams::create()
{
    return new LinuxUcpSystem(this);
}
