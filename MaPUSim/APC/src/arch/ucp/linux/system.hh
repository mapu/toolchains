#ifndef __ARCH_UCP_LINUX_SYSTEM_HH__
#define __ARCH_UCP_LINUX_SYSTEM_HH__

class ThreadContext;

class BreakPCEvent;
class IdleStartEvent;

#include "arch/ucp/idle_event.hh"
#include "arch/ucp/system.hh"
#include "kern/linux/events.hh"
#include "params/LinuxUcpSystem.hh"

/**
 * This class contains linux specific system code (Loading, Events).
 * It points to objects that are the system binaries to load and patches them
 * appropriately to work in simulator.
 */
class LinuxUcpSystem : public UcpSystem
{
  private:
    class SkipDelayLoopEvent : public SkipFuncEvent
    {
      public:
        SkipDelayLoopEvent(PCEventQueue *q, const std::string &desc, Addr addr)
            : SkipFuncEvent(q, desc, addr) {}
        virtual void process(ThreadContext *tc);
    };

    class PrintThreadInfo : public PCEvent
    {
      public:
        PrintThreadInfo(PCEventQueue *q, const std::string &desc, Addr addr)
            : PCEvent(q, desc, addr) {}
        virtual void process(ThreadContext *tc);
    };


    /**
     * Addresses defining where the kernel bootloader places various
     * elements.  Details found in include/asm-ucp/system.h
     */
    Addr KernelStart; // Lookup the symbol swapper_pg_dir

  public:
    Addr InitStack() const { return KernelStart + 0x02000; }
    Addr EmptyPGT() const  { return KernelStart + 0x04000; }
    Addr EmptyPGE() const  { return KernelStart + 0x08000; }
    Addr ZeroPGE() const   { return KernelStart + 0x0A000; }
    Addr StartAddr() const { return KernelStart + 0x10000; }

    Addr Param() const { return ZeroPGE() + 0x0; }
    Addr CommandLine() const { return Param() + 0x0; }
    Addr InitrdStart() const { return Param() + 0x100; }
    Addr InitrdSize() const { return Param() + 0x108; }
    static const int CommandLineSize = 256;

  private:
#ifndef NDEBUG
    /** Event to halt the simulator if the kernel calls panic()  */
    BreakPCEvent *kernelPanicEvent;

    /** Event to halt the simulator if the kernel calls die_if_kernel  */
    BreakPCEvent *kernelDieEvent;
#endif

    /**
     * Event to skip determine_cpu_caches() because we don't support
     * the IPRs that the code can access to figure out cache sizes
     */
    SkipFuncEvent *skipCacheProbeEvent;

    /** PC based event to skip the ide_delay_50ms() call */
    SkipFuncEvent *skipIdeDelay50msEvent;

    /**
     * PC based event to skip the dprink() call and emulate its
     * functionality
     */
    Linux::DebugPrintkEvent *debugPrintkEvent;

    /**
     * Skip calculate_delay_loop() rather than waiting for this to be
     * calculated
     */
    SkipDelayLoopEvent *skipDelayLoopEvent;

    /**
     * Event to print information about thread switches if the trace flag
     * Thread is set
     */
    PrintThreadInfo *printThreadEvent;

    /** Grab the PCBB of the idle process when it starts */
    IdleStartEvent *idleStartEvent;

  public:
    typedef LinuxUcpSystemParams Params;
    LinuxUcpSystem(Params *p);
    ~LinuxUcpSystem();

    void setDelayLoop(ThreadContext *tc);
};

#endif // __ARCH_UCP_LINUX_SYSTEM_HH__
