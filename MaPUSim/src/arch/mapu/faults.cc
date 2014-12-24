#include "arch/mapu/faults.hh"
#include "base/trace.hh"
#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "debug/MapuPRA.hh"
#include "mem/page_table.hh"
#include "sim/process.hh"

namespace MapuISA
{

typedef MapuFaultBase::FaultVals FaultVals;

template <> FaultVals MapuFault<SystemCallFault>::vals =
    { "Syscall", 0x180, ExcCodeSys };

template <> FaultVals MapuFault<ReservedInstructionFault>::vals =
    { "Reserved Instruction Fault", 0x180, ExcCodeRI };

template <> FaultVals MapuFault<ThreadFault>::vals =
    { "Thread Fault", 0x180, ExcCodeDummy };

template <> FaultVals MapuFault<IntegerOverflowFault>::vals =
    { "Integer Overflow Exception", 0x180, ExcCodeOv };

template <> FaultVals MapuFault<TrapFault>::vals =
    { "Trap", 0x180, ExcCodeTr };

template <> FaultVals MapuFault<BreakpointFault>::vals =
    { "Breakpoint", 0x180, ExcCodeBp };

template <> FaultVals MapuFault<DspStateDisabledFault>::vals =
    { "DSP Disabled Fault", 0x180, ExcCodeDummy };

template <> FaultVals MapuFault<MachineCheckFault>::vals =
    { "Machine Check", 0x180, ExcCodeMCheck };

template <> FaultVals MapuFault<ResetFault>::vals =
    { "Reset Fault", 0x000, ExcCodeDummy };

template <> FaultVals MapuFault<SoftResetFault>::vals =
    { "Soft Reset Fault", 0x000, ExcCodeDummy };

template <> FaultVals MapuFault<NonMaskableInterrupt>::vals =
    { "Non Maskable Interrupt", 0x000, ExcCodeDummy };

template <> FaultVals MapuFault<CoprocessorUnusableFault>::vals =
    { "Coprocessor Unusable Fault", 0x180, ExcCodeCpU };

template <> FaultVals MapuFault<InterruptFault>::vals =
    { "Interrupt", 0x000, ExcCodeInt };

template <> FaultVals MapuFault<AddressErrorFault>::vals =
    { "Address Error", 0x180, ExcCodeDummy };

template <> FaultVals MapuFault<TlbInvalidFault>::vals =
    { "Invalid TLB Entry Exception", 0x180, ExcCodeDummy };

template <> FaultVals MapuFault<TlbRefillFault>::vals =
    { "TLB Refill Exception", 0x180, ExcCodeDummy };

template <> MapuFaultBase::FaultVals MapuFault<TlbModifiedFault>::vals =
    { "TLB Modified Exception", 0x180, ExcCodeMod };

void
MapuFaultBase::setExceptionState(ThreadContext *tc, uint8_t excCode)
{
}

void
MapuFaultBase::invoke(ThreadContext *tc, StaticInstPtr inst)
{
    if (FullSystem) {
        DPRINTF(MapuPRA, "Fault %s encountered.\n", name());
        setExceptionState(tc, code());
        tc->pcState(vect(tc));
    } else {
        panic("Fault %s encountered.\n", name());
    }
}

void
ResetFault::invoke(ThreadContext *tc, StaticInstPtr inst)
{
    if (FullSystem) {
        DPRINTF(MapuPRA, "%s encountered.\n", name());
        /* All reset activity must be invoked from here */
        Addr handler = vect(tc);
        tc->pcState(handler);
        DPRINTF(MapuPRA, "ResetFault::invoke : PC set to %x", handler);
    }
}

void
SoftResetFault::invoke(ThreadContext *tc, StaticInstPtr inst)
{
    panic("Soft reset not implemented.\n");
}

void
NonMaskableInterrupt::invoke(ThreadContext *tc, StaticInstPtr inst)
{
    panic("Non maskable interrupt not implemented.\n");
}

} // namespace MapuISA
