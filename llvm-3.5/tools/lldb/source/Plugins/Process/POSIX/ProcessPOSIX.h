//===-- ProcessPOSIX.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_ProcessPOSIX_H_
#define liblldb_ProcessPOSIX_H_

// C Includes

// C++ Includes
#include <queue>
#include <set>

// Other libraries and framework includes
#include "lldb/Target/Process.h"
#include "ProcessMessage.h"

class ProcessMonitor;
class POSIXThread;

class ProcessPOSIX :
    public lldb_private::Process
{
public:

    //------------------------------------------------------------------
    // Constructors and destructors
    //------------------------------------------------------------------
    ProcessPOSIX(lldb_private::Target& target,
                 lldb_private::Listener &listener,
                 lldb_private::UnixSignalsSP &unix_signals_sp);

    virtual
    ~ProcessPOSIX();

    //------------------------------------------------------------------
    // Process protocol.
    //------------------------------------------------------------------
    virtual void
    Finalize() override;

    virtual bool
    CanDebug(lldb_private::Target &target, bool plugin_specified_by_name) override;

    virtual lldb_private::Error
    WillLaunch(lldb_private::Module *module) override;

    virtual lldb_private::Error
    DoAttachToProcessWithID(lldb::pid_t pid) override;

    virtual lldb_private::Error
    DoAttachToProcessWithID (lldb::pid_t pid, const lldb_private::ProcessAttachInfo &attach_info) override;

    virtual lldb_private::Error
    DoLaunch (lldb_private::Module *exe_module, 
              lldb_private::ProcessLaunchInfo &launch_info) override;

    virtual void
    DidLaunch() override;

    virtual lldb_private::Error
    DoResume() override;

    virtual lldb_private::Error
    DoHalt(bool &caused_stop) override;

    virtual lldb_private::Error
    DoDetach(bool keep_stopped) override = 0;

    virtual lldb_private::Error
    DoSignal(int signal) override;

    virtual lldb_private::Error
    DoDestroy() override;

    virtual void
    DoDidExec() override;

    virtual void
    RefreshStateAfterStop() override;

    virtual bool
    IsAlive() override;

    virtual size_t
    DoReadMemory(lldb::addr_t vm_addr,
                 void *buf,
                 size_t size,
                 lldb_private::Error &error) override;

    virtual size_t
    DoWriteMemory(lldb::addr_t vm_addr, const void *buf, size_t size,
                  lldb_private::Error &error) override;

    virtual lldb::addr_t
    DoAllocateMemory(size_t size, uint32_t permissions,
                     lldb_private::Error &error) override;

    virtual lldb_private::Error
    DoDeallocateMemory(lldb::addr_t ptr) override;

    virtual size_t
    GetSoftwareBreakpointTrapOpcode(lldb_private::BreakpointSite* bp_site);

    virtual lldb_private::Error
    EnableBreakpointSite(lldb_private::BreakpointSite *bp_site) override;

    virtual lldb_private::Error
    DisableBreakpointSite(lldb_private::BreakpointSite *bp_site) override;

    virtual lldb_private::Error
    EnableWatchpoint(lldb_private::Watchpoint *wp, bool notify = true) override;

    virtual lldb_private::Error
    DisableWatchpoint(lldb_private::Watchpoint *wp, bool notify = true) override;

    virtual lldb_private::Error
    GetWatchpointSupportInfo(uint32_t &num) override;

    virtual lldb_private::Error
    GetWatchpointSupportInfo(uint32_t &num, bool &after) override;

    virtual uint32_t
    UpdateThreadListIfNeeded();

    virtual bool
    UpdateThreadList(lldb_private::ThreadList &old_thread_list, 
                     lldb_private::ThreadList &new_thread_list) override = 0;

    virtual lldb::ByteOrder
    GetByteOrder() const;

    virtual lldb::addr_t
    GetImageInfoAddress() override;

    virtual size_t
    PutSTDIN(const char *buf, size_t len, lldb_private::Error &error) override;

    const lldb::DataBufferSP
    GetAuxvData () override;

    //--------------------------------------------------------------------------
    // ProcessPOSIX internal API.

    /// Registers the given message with this process.
    virtual void
    SendMessage(const ProcessMessage &message);

    ProcessMonitor &
    GetMonitor() { assert(m_monitor); return *m_monitor; }

    const char *GetFilePath(const lldb_private::FileAction *file_action, const char *default_path,
                            const char *dbg_pts_path);

    /// Stops all threads in the process.
    /// The \p stop_tid parameter indicates the thread which initiated the stop.
    virtual void
    StopAllThreads(lldb::tid_t stop_tid);

    /// Adds the thread to the list of threads for which we have received the initial stopping signal.
    /// The \p stop_tid parameter indicates the thread which the stop happened for.
    bool
    AddThreadForInitialStopIfNeeded(lldb::tid_t stop_tid);

    bool
    WaitingForInitialStop(lldb::tid_t stop_tid);

    virtual POSIXThread *
    CreateNewPOSIXThread(lldb_private::Process &process, lldb::tid_t tid);

protected:
    /// Target byte order.
    lldb::ByteOrder m_byte_order;

    /// Process monitor;
    ProcessMonitor *m_monitor;

    /// The module we are executing.
    lldb_private::Module *m_module;

    /// Message queue notifying this instance of inferior process state changes.
    lldb_private::Mutex m_message_mutex;
    std::queue<ProcessMessage> m_message_queue;

    /// Drive any exit events to completion.
    bool m_exit_now;

    /// Returns true if the process has exited.
    bool HasExited();

    /// Returns true if the process is stopped.
    bool IsStopped();

    /// Returns true if at least one running is currently running
    bool IsAThreadRunning();

    typedef std::map<lldb::addr_t, lldb::addr_t> MMapMap;
    MMapMap m_addr_to_mmap_size;

    typedef std::set<lldb::tid_t> ThreadStopSet;
    /// Every thread begins with a stop signal. This keeps track
    /// of the threads for which we have received the stop signal.
    ThreadStopSet m_seen_initial_stop;
};

#endif  // liblldb_MacOSXProcess_H_
