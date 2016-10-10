/*
 * Copyright (c) 2011 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Kevin Lim
 */

#ifndef __CPU_THREAD_CONTEXT_HH__
#define __CPU_THREAD_CONTEXT_HH__

#include <iostream>
#include <string>

#include "arch/registers.hh"
#include "arch/types.hh"
#include "base/types.hh"
#include "config/the_isa.hh"

// @todo: Figure out a more architecture independent way to obtain the ITB and
// DTB pointers.
namespace TheISA
{
    class Decoder;
    class TLB;
}
class BaseCPU;
class CheckerCPU;
class Checkpoint;
class EndQuiesceEvent;
class SETranslatingPortProxy;
class FSTranslatingPortProxy;
class PortProxy;
class Process;
class System;
namespace TheISA {
    namespace Kernel {
        class Statistics;
    }
}

/**
 * ThreadContext is the external interface to all thread state for
 * anything outside of the CPU. It provides all accessor methods to
 * state that might be needed by external objects, ranging from
 * register values to things such as kernel stats. It is an abstract
 * base class; the CPU can create its own ThreadContext by either
 * deriving from it, or using the templated ProxyThreadContext.
 *
 * The ThreadContext is slightly different than the ExecContext.  The
 * ThreadContext provides access to an individual thread's state; an
 * ExecContext provides ISA access to the CPU (meaning it is
 * implicitly multithreaded on SMT systems).  Additionally the
 * ThreadState is an abstract class that exactly defines the
 * interface; the ExecContext is a more implicit interface that must
 * be implemented so that the ISA can access whatever state it needs.
 */
class ThreadContext
{
  protected:
    typedef TheISA::MachInst MachInst;
    typedef TheISA::IntReg IntReg;
    typedef TheISA::FloatReg FloatReg;
    typedef TheISA::FloatRegBits FloatRegBits;
    typedef TheISA::MiscReg MiscReg;
  public:

    enum Status
    {
        /// Running.  Instructions should be executed only when
        /// the context is in this state.
        Active,

        /// Temporarily inactive.  Entered while waiting for
        /// synchronization, etc.
        Suspended,

        /// Permanently shut down.  Entered when target executes
        /// m5exit pseudo-instruction.  When all contexts enter
        /// this state, the simulation will terminate.
        Halted
    };

    virtual ~ThreadContext() { };

    virtual BaseCPU *getCpuPtr() = 0;

    virtual int cpuId() = 0;

    virtual int threadId() = 0;

    virtual void setThreadId(int id) = 0;

    virtual int contextId() = 0;

    virtual void setContextId(int id) = 0;

    virtual TheISA::TLB *getITBPtr() = 0;

    virtual TheISA::TLB *getDTBPtr() = 0;

    virtual CheckerCPU *getCheckerCpuPtr() = 0;

    virtual TheISA::Decoder *getDecoderPtr() = 0;

    virtual System *getSystemPtr() = 0;

    virtual TheISA::Kernel::Statistics *getKernelStats() = 0;

    virtual PortProxy &getPhysProxy() = 0;

    virtual FSTranslatingPortProxy &getVirtProxy() = 0;

    /**
     * Initialise the physical and virtual port proxies and tie them to
     * the data port of the CPU.
     *
     * tc ThreadContext for the virtual-to-physical translation
     */
    virtual void initMemProxies(ThreadContext *tc) = 0;

    virtual SETranslatingPortProxy &getMemProxy() = 0;

    virtual Process *getProcessPtr() = 0;

    virtual Status status() const = 0;

    virtual void setStatus(Status new_status) = 0;

    /// Set the status to Active.  Optional delay indicates number of
    /// cycles to wait before beginning execution.
    virtual void activate(int delay = 1) = 0;

    /// Set the status to Suspended.
    virtual void suspend(int delay = 0) = 0;

    /// Set the status to Halted.
    virtual void halt(int delay = 0) = 0;

    virtual void dumpFuncProfile() = 0;

    virtual void takeOverFrom(ThreadContext *old_context) = 0;

    virtual void regStats(const std::string &name) = 0;

    virtual void serialize(std::ostream &os) = 0;
    virtual void unserialize(Checkpoint *cp, const std::string &section) = 0;

    virtual EndQuiesceEvent *getQuiesceEvent() = 0;

    // Not necessarily the best location for these...
    // Having an extra function just to read these is obnoxious
    virtual Tick readLastActivate() = 0;
    virtual Tick readLastSuspend() = 0;

    virtual void profileClear() = 0;
    virtual void profileSample() = 0;

    virtual void copyArchRegs(ThreadContext *tc) = 0;

    virtual void clearArchRegs() = 0;

    //
    // New accessors for new decoder.
    //
    virtual uint64_t readIntReg(int reg_idx) = 0;

    virtual FloatReg readFloatReg(int reg_idx) = 0;

    virtual FloatRegBits readFloatRegBits(int reg_idx) = 0;

    virtual void setIntReg(int reg_idx, uint64_t val) = 0;

    virtual void setFloatReg(int reg_idx, FloatReg val) = 0;

    virtual void setFloatRegBits(int reg_idx, FloatRegBits val) = 0;

    virtual TheISA::PCState pcState() = 0;

    virtual void pcState(const TheISA::PCState &val) = 0;

    virtual void pcStateNoRecord(const TheISA::PCState &val) = 0;

    virtual Addr instAddr() = 0;

    virtual Addr nextInstAddr() = 0;

    virtual TheISA::PCState bpPC() = 0;

    virtual void bpPC(const TheISA::PCState &val) = 0;

    virtual MicroPC microPC() = 0;

    virtual MiscReg readMiscRegNoEffect(int misc_reg) = 0;

    virtual MiscReg readMiscReg(int misc_reg) = 0;

    virtual void setMiscRegNoEffect(int misc_reg, const MiscReg &val) = 0;

    virtual void setMiscReg(int misc_reg, const MiscReg &val) = 0;

    virtual int flattenIntIndex(int reg) = 0;
    virtual int flattenFloatIndex(int reg) = 0;

    virtual uint64_t
    readRegOtherThread(int misc_reg, ThreadID tid)
    {
        return 0;
    }

    virtual void
    setRegOtherThread(int misc_reg, const MiscReg &val, ThreadID tid)
    {
    }

    // Also not necessarily the best location for these two.  Hopefully will go
    // away once we decide upon where st cond failures goes.
    virtual unsigned readStCondFailures() = 0;

    virtual void setStCondFailures(unsigned sc_failures) = 0;

    // Only really makes sense for old CPU model.  Still could be useful though.
    virtual bool misspeculating() = 0;

    // Same with st cond failures.
    virtual Counter readFuncExeInst() = 0;

    virtual void syscall(int64_t callnum) = 0;

    // This function exits the thread context in the CPU and returns
    // 1 if the CPU has no more active threads (meaning it's OK to exit);
    // Used in syscall-emulation mode when a  thread calls the exit syscall.
    virtual int exit() { return 1; };

    /** function to compare two thread contexts (for debugging) */
    static void compare(ThreadContext *one, ThreadContext *two);
};

/**
 * ProxyThreadContext class that provides a way to implement a
 * ThreadContext without having to derive from it. ThreadContext is an
 * abstract class, so anything that derives from it and uses its
 * interface will pay the overhead of virtual function calls.  This
 * class is created to enable a user-defined Thread object to be used
 * wherever ThreadContexts are used, without paying the overhead of
 * virtual function calls when it is used by itself.  See
 * simple_thread.hh for an example of this.
 */
template <class TC>
class ProxyThreadContext : public ThreadContext
{
  public:
    ProxyThreadContext(TC *actual_tc)
    { actualTC = actual_tc; }

  private:
    TC *actualTC;

  public:

    BaseCPU *getCpuPtr() { return actualTC->getCpuPtr(); }

    int cpuId() { return actualTC->cpuId(); }

    int threadId() { return actualTC->threadId(); }

    void setThreadId(int id) { return actualTC->setThreadId(id); }

    int contextId() { return actualTC->contextId(); }

    void setContextId(int id) { actualTC->setContextId(id); }

    TheISA::TLB *getITBPtr() { return actualTC->getITBPtr(); }

    TheISA::TLB *getDTBPtr() { return actualTC->getDTBPtr(); }

    CheckerCPU *getCheckerCpuPtr() { return actualTC->getCheckerCpuPtr(); }

    TheISA::Decoder *getDecoderPtr() { return actualTC->getDecoderPtr(); }

    System *getSystemPtr() { return actualTC->getSystemPtr(); }

    TheISA::Kernel::Statistics *getKernelStats()
    { return actualTC->getKernelStats(); }

    PortProxy &getPhysProxy() { return actualTC->getPhysProxy(); }

    FSTranslatingPortProxy &getVirtProxy() { return actualTC->getVirtProxy(); }

    void initMemProxies(ThreadContext *tc) { actualTC->initMemProxies(tc); }

    SETranslatingPortProxy &getMemProxy() { return actualTC->getMemProxy(); }

    Process *getProcessPtr() { return actualTC->getProcessPtr(); }

    Status status() const { return actualTC->status(); }

    void setStatus(Status new_status) { actualTC->setStatus(new_status); }

    /// Set the status to Active.  Optional delay indicates number of
    /// cycles to wait before beginning execution.
    void activate(int delay = 1) { actualTC->activate(delay); }

    /// Set the status to Suspended.
    void suspend(int delay = 0) { actualTC->suspend(); }

    /// Set the status to Halted.
    void halt(int delay = 0) { actualTC->halt(); }

    void dumpFuncProfile() { actualTC->dumpFuncProfile(); }

    void takeOverFrom(ThreadContext *oldContext)
    { actualTC->takeOverFrom(oldContext); }

    void regStats(const std::string &name) { actualTC->regStats(name); }

    void serialize(std::ostream &os) { actualTC->serialize(os); }
    void unserialize(Checkpoint *cp, const std::string &section)
    { actualTC->unserialize(cp, section); }

    EndQuiesceEvent *getQuiesceEvent() { return actualTC->getQuiesceEvent(); }

    Tick readLastActivate() { return actualTC->readLastActivate(); }
    Tick readLastSuspend() { return actualTC->readLastSuspend(); }

    void profileClear() { return actualTC->profileClear(); }
    void profileSample() { return actualTC->profileSample(); }

    // @todo: Do I need this?
    void copyArchRegs(ThreadContext *tc) { actualTC->copyArchRegs(tc); }

    void clearArchRegs() { actualTC->clearArchRegs(); }

    //
    // New accessors for new decoder.
    //
    uint64_t readIntReg(int reg_idx)
    { return actualTC->readIntReg(reg_idx); }

    FloatReg readFloatReg(int reg_idx)
    { return actualTC->readFloatReg(reg_idx); }

    FloatRegBits readFloatRegBits(int reg_idx)
    { return actualTC->readFloatRegBits(reg_idx); }

    void setIntReg(int reg_idx, uint64_t val)
    { actualTC->setIntReg(reg_idx, val); }

    void setFloatReg(int reg_idx, FloatReg val)
    { actualTC->setFloatReg(reg_idx, val); }

    void setFloatRegBits(int reg_idx, FloatRegBits val)
    { actualTC->setFloatRegBits(reg_idx, val); }

    TheISA::PCState pcState() { return actualTC->pcState(); }

    void pcState(const TheISA::PCState &val) { actualTC->pcState(val); }

    void pcStateNoRecord(const TheISA::PCState &val) { actualTC->pcState(val); }

    TheISA::PCState bpPC() { return actualTC->bpPC(); }

    void bpPC(const TheISA::PCState &val) { actualTC->bpPC(val); }

    Addr instAddr() { return actualTC->instAddr(); }
    Addr nextInstAddr() { return actualTC->nextInstAddr(); }
    MicroPC microPC() { return actualTC->microPC(); }

    bool readPredicate() { return actualTC->readPredicate(); }

    void setPredicate(bool val)
    { actualTC->setPredicate(val); }

    MiscReg readMiscRegNoEffect(int misc_reg)
    { return actualTC->readMiscRegNoEffect(misc_reg); }

    MiscReg readMiscReg(int misc_reg)
    { return actualTC->readMiscReg(misc_reg); }

    void setMiscRegNoEffect(int misc_reg, const MiscReg &val)
    { return actualTC->setMiscRegNoEffect(misc_reg, val); }

    void setMiscReg(int misc_reg, const MiscReg &val)
    { return actualTC->setMiscReg(misc_reg, val); }

    int flattenIntIndex(int reg)
    { return actualTC->flattenIntIndex(reg); }

    int flattenFloatIndex(int reg)
    { return actualTC->flattenFloatIndex(reg); }

    unsigned readStCondFailures()
    { return actualTC->readStCondFailures(); }

    void setStCondFailures(unsigned sc_failures)
    { actualTC->setStCondFailures(sc_failures); }

    // @todo: Fix this!
    bool misspeculating() { return actualTC->misspeculating(); }

    void syscall(int64_t callnum)
    { actualTC->syscall(callnum); }

    Counter readFuncExeInst() { return actualTC->readFuncExeInst(); }
};

#endif
