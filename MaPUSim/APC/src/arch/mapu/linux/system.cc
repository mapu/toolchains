/**
 * @file
 * This code loads the linux kernel, console, pal and patches certain
 * functions.  The symbol tables are loaded so that traces can show
 * the executing function and we can skip functions. Various delay
 * loops are skipped and their final values manually computed to speed
 * up boot time.
 */

#include "arch/mapu/linux/system.hh"
#include "arch/mapu/linux/threadinfo.hh"
#include "arch/mapu/idle_event.hh"
#include "arch/mapu/system.hh"
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
using namespace MapuISA;
using namespace Linux;

LinuxMapuSystem::LinuxMapuSystem(Params *p)
    : MapuSystem(p)
{
}

LinuxMapuSystem::~LinuxMapuSystem()
{
}


void
LinuxMapuSystem::setDelayLoop(ThreadContext *tc)
{
    panic("setDelayLoop not implemented.\n");
}


void
LinuxMapuSystem::SkipDelayLoopEvent::process(ThreadContext *tc)
{
    SkipFuncEvent::process(tc);
    // calculate and set loops_per_jiffy
    ((LinuxMapuSystem *)tc->getSystemPtr())->setDelayLoop(tc);
}

void
LinuxMapuSystem::PrintThreadInfo::process(ThreadContext *tc)
{
    Linux::ThreadInfo ti(tc);

    DPRINTF(Thread, "Currently Executing Thread %s, pid %d, started at: %d\n",
            ti.curTaskName(), ti.curTaskPID(), ti.curTaskStart());
}

LinuxMapuSystem *
LinuxMapuSystemParams::create()
{
    return new LinuxMapuSystem(this);
}
