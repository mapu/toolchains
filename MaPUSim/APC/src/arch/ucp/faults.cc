#include "arch/ucp/faults.hh"
#include "base/trace.hh"
#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "debug/UcpPRA.hh"
#include "mem/page_table.hh"
#include "sim/process.hh"

namespace UcpISA
{

typedef UcpFaultBase::FaultVals FaultVals;

template <> FaultVals UcpFault<SystemCallFault>::vals =
    { "Syscall", 0x180, ExcCodeSys };

template <> FaultVals UcpFault<ReservedInstructionFault>::vals =
    { "Reserved Instruction Fault", 0x180, ExcCodeRI };

template <> FaultVals UcpFault<ThreadFault>::vals =
    { "Thread Fault", 0x180, ExcCodeDummy };

template <> FaultVals UcpFault<IntegerOverflowFault>::vals =
    { "Integer Overflow Exception", 0x180, ExcCodeOv };

template <> FaultVals UcpFault<TrapFault>::vals =
    { "Trap", 0x180, ExcCodeTr };

template <> FaultVals UcpFault<BreakpointFault>::vals =
    { "Breakpoint", 0x180, ExcCodeBp };

template <> FaultVals UcpFault<DspStateDisabledFault>::vals =
    { "DSP Disabled Fault", 0x180, ExcCodeDummy };

template <> FaultVals UcpFault<MachineCheckFault>::vals =
    { "Machine Check", 0x180, ExcCodeMCheck };

template <> FaultVals UcpFault<ResetFault>::vals =
    { "Reset Fault", 0x000, ExcCodeDummy };

template <> FaultVals UcpFault<SoftResetFault>::vals =
    { "Soft Reset Fault", 0x000, ExcCodeDummy };

template <> FaultVals UcpFault<NonMaskableInterrupt>::vals =
    { "Non Maskable Interrupt", 0x000, ExcCodeDummy };

template <> FaultVals UcpFault<CoprocessorUnusableFault>::vals =
    { "Coprocessor Unusable Fault", 0x180, ExcCodeCpU };

template <> FaultVals UcpFault<InterruptFault>::vals =
    { "Interrupt", 0x000, ExcCodeInt };

template <> FaultVals UcpFault<AddressErrorFault>::vals =
    { "Address Error", 0x180, ExcCodeDummy };

template <> FaultVals UcpFault<TlbInvalidFault>::vals =
    { "Invalid TLB Entry Exception", 0x180, ExcCodeDummy };

template <> FaultVals UcpFault<TlbRefillFault>::vals =
    { "TLB Refill Exception", 0x180, ExcCodeDummy };

template <> UcpFaultBase::FaultVals UcpFault<TlbModifiedFault>::vals =
    { "TLB Modified Exception", 0x180, ExcCodeMod };

void
UcpFaultBase::setExceptionState(ThreadContext *tc, uint8_t excCode)
{
}

void
UcpFaultBase::invoke(ThreadContext *tc, StaticInstPtr inst)
{
    if (FullSystem) {
        DPRINTF(UcpPRA, "Fault %s encountered.\n", name());
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
        DPRINTF(UcpPRA, "%s encountered.\n", name());
        /* All reset activity must be invoked from here */
        Addr handler = vect(tc);
        tc->pcState(handler);
        DPRINTF(UcpPRA, "ResetFault::invoke : PC set to %x", handler);
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

} // namespace UcpISA
