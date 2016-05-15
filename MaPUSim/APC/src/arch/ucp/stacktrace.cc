#include <string>

#include "arch/ucp/stacktrace.hh"
#include "base/trace.hh"

using namespace std;

namespace UcpISA {

ProcessInfo::ProcessInfo(ThreadContext *_tc)
    : tc(_tc)
{
    panic("ProcessInfo constructor not implemented.\n");
}

Addr
ProcessInfo::task(Addr ksp) const
{
    panic("ProcessInfo::task not implemented.\n");
    return 0;
}

int
ProcessInfo::pid(Addr ksp) const
{
    panic("ProcessInfo::pid not implemented.\n");
    return 0;
}

string
ProcessInfo::name(Addr ksp) const
{
    panic("ProcessInfo::name not implemented.\n");
    return "";
}

StackTrace::StackTrace()
    : tc(0), stack(64)
{
    panic("StackTrace constructor not implemented.\n");
}

StackTrace::StackTrace(ThreadContext *_tc, StaticInstPtr inst)
    : tc(0), stack(64)
{
    panic("StackTrace constructor not implemented.\n");
}

StackTrace::~StackTrace()
{
    panic("StackTrace destructor not implemented.\n");
}

void
StackTrace::trace(ThreadContext *_tc, bool is_call)
{
    panic("StackTrace::trace not implemented.\n");
}

bool
StackTrace::isEntry(Addr addr)
{
    panic("StackTrace::isEntry not implemented.\n");
    return false;
}

bool
StackTrace::decodeStack(MachInst inst, int &disp)
{
    panic("StackTrace::decodeStack not implemented.\n");
    return false;
}

bool
StackTrace::decodeSave(MachInst inst, int &reg, int &disp)
{
    panic("StackTrace::decodeSave not implemented.\n");
    return true;
}

/*
 * Decode the function prologue for the function we're in, and note
 * which registers are stored where, and how large the stack frame is.
 */
bool
StackTrace::decodePrologue(Addr sp, Addr callpc, Addr func, int &size,
                           Addr &ra)
{
    panic("StackTrace::decodePrologue not implemented.\n");
    return true;
}

#if TRACING_ON
void
StackTrace::dump()
{
    panic("StackTrace::dump not implemented.\n");
}
#endif

} // namespace UcpISA
