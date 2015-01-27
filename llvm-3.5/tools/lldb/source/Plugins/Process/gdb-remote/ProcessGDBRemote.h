//===-- ProcessGDBRemote.h --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_ProcessGDBRemote_h_
#define liblldb_ProcessGDBRemote_h_

// C Includes

// C++ Includes
#include <list>
#include <vector>

// Other libraries and framework includes
#include "lldb/Core/ArchSpec.h"
#include "lldb/Core/Broadcaster.h"
#include "lldb/Core/ConstString.h"
#include "lldb/Core/Error.h"
#include "lldb/Core/StreamString.h"
#include "lldb/Core/StringList.h"
#include "lldb/Core/StructuredData.h"
#include "lldb/Core/ThreadSafeValue.h"
#include "lldb/Host/HostThread.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/Thread.h"

#include "GDBRemoteCommunicationClient.h"
#include "Utility/StringExtractor.h"
#include "GDBRemoteRegisterContext.h"

class ThreadGDBRemote;

class ProcessGDBRemote : public lldb_private::Process
{
public:
    //------------------------------------------------------------------
    // Constructors and Destructors
    //------------------------------------------------------------------
    static lldb::ProcessSP
    CreateInstance (lldb_private::Target& target, 
                    lldb_private::Listener &listener,
                    const lldb_private::FileSpec *crash_file_path);

    static void
    Initialize();

    static void
    DebuggerInitialize (lldb_private::Debugger &debugger);

    static void
    Terminate();

    static lldb_private::ConstString
    GetPluginNameStatic();

    static const char *
    GetPluginDescriptionStatic();

    //------------------------------------------------------------------
    // Constructors and Destructors
    //------------------------------------------------------------------
    ProcessGDBRemote(lldb_private::Target& target, lldb_private::Listener &listener);

    virtual
    ~ProcessGDBRemote();

    //------------------------------------------------------------------
    // Check if a given Process
    //------------------------------------------------------------------
    virtual bool
    CanDebug (lldb_private::Target &target,
              bool plugin_specified_by_name) override;

    virtual lldb_private::CommandObject *
    GetPluginCommandObject() override;

    //------------------------------------------------------------------
    // Creating a new process, or attaching to an existing one
    //------------------------------------------------------------------
    virtual lldb_private::Error
    WillLaunch (lldb_private::Module* module) override;

    virtual lldb_private::Error
    DoLaunch (lldb_private::Module *exe_module, 
              lldb_private::ProcessLaunchInfo &launch_info) override;

    virtual void
    DidLaunch () override;

    virtual lldb_private::Error
    WillAttachToProcessWithID (lldb::pid_t pid) override;

    virtual lldb_private::Error
    WillAttachToProcessWithName (const char *process_name, bool wait_for_launch) override;

    virtual lldb_private::Error
    DoConnectRemote (lldb_private::Stream *strm, const char *remote_url) override;
    
    lldb_private::Error
    WillLaunchOrAttach ();

    virtual lldb_private::Error
    DoAttachToProcessWithID (lldb::pid_t pid) override;
    
    virtual lldb_private::Error
    DoAttachToProcessWithID (lldb::pid_t pid, const lldb_private::ProcessAttachInfo &attach_info) override;
    
    virtual lldb_private::Error
    DoAttachToProcessWithName (const char *process_name,
                               const lldb_private::ProcessAttachInfo &attach_info) override;

    virtual void
    DidAttach (lldb_private::ArchSpec &process_arch) override;

    //------------------------------------------------------------------
    // PluginInterface protocol
    //------------------------------------------------------------------
    virtual lldb_private::ConstString
    GetPluginName() override;

    virtual uint32_t
    GetPluginVersion() override;

    //------------------------------------------------------------------
    // Process Control
    //------------------------------------------------------------------
    virtual lldb_private::Error
    WillResume () override;

    virtual lldb_private::Error
    DoResume () override;

    virtual lldb_private::Error
    DoHalt (bool &caused_stop) override;

    virtual lldb_private::Error
    DoDetach (bool keep_stopped) override;
    
    virtual bool
    DetachRequiresHalt() override { return true; }

    virtual lldb_private::Error
    DoSignal (int signal) override;

    virtual lldb_private::Error
    DoDestroy () override;

    virtual void
    RefreshStateAfterStop() override;

    //------------------------------------------------------------------
    // Process Queries
    //------------------------------------------------------------------
    virtual bool
    IsAlive () override;

    virtual lldb::addr_t
    GetImageInfoAddress() override;

    //------------------------------------------------------------------
    // Process Memory
    //------------------------------------------------------------------
    virtual size_t
    DoReadMemory (lldb::addr_t addr, void *buf, size_t size, lldb_private::Error &error) override;

    virtual size_t
    DoWriteMemory (lldb::addr_t addr, const void *buf, size_t size, lldb_private::Error &error) override;

    virtual lldb::addr_t
    DoAllocateMemory (size_t size, uint32_t permissions, lldb_private::Error &error) override;

    virtual lldb_private::Error
    GetMemoryRegionInfo (lldb::addr_t load_addr, 
                         lldb_private::MemoryRegionInfo &region_info) override;
    
    virtual lldb_private::Error
    DoDeallocateMemory (lldb::addr_t ptr) override;

    //------------------------------------------------------------------
    // Process STDIO
    //------------------------------------------------------------------
    virtual size_t
    PutSTDIN (const char *buf, size_t buf_size, lldb_private::Error &error) override;

    //----------------------------------------------------------------------
    // Process Breakpoints
    //----------------------------------------------------------------------
    virtual lldb_private::Error
    EnableBreakpointSite (lldb_private::BreakpointSite *bp_site) override;

    virtual lldb_private::Error
    DisableBreakpointSite (lldb_private::BreakpointSite *bp_site) override;

    //----------------------------------------------------------------------
    // Process Watchpoints
    //----------------------------------------------------------------------
    virtual lldb_private::Error
    EnableWatchpoint (lldb_private::Watchpoint *wp, bool notify = true) override;

    virtual lldb_private::Error
    DisableWatchpoint (lldb_private::Watchpoint *wp, bool notify = true) override;

    virtual lldb_private::Error
    GetWatchpointSupportInfo (uint32_t &num) override;
    
    virtual lldb_private::Error
    GetWatchpointSupportInfo (uint32_t &num, bool& after) override;
    
    virtual bool
    StartNoticingNewThreads() override;

    virtual bool
    StopNoticingNewThreads() override;

    GDBRemoteCommunicationClient &
    GetGDBRemote()
    {
        return m_gdb_comm;
    }
    
    virtual lldb_private::Error
    SendEventData(const char *data) override;

    //----------------------------------------------------------------------
    // Override SetExitStatus so we can disconnect from the remote GDB server
    //----------------------------------------------------------------------
    virtual bool
    SetExitStatus (int exit_status, const char *cstr) override;

    void
    SetUserSpecifiedMaxMemoryTransferSize (uint64_t user_specified_max);

protected:
    friend class ThreadGDBRemote;
    friend class GDBRemoteCommunicationClient;
    friend class GDBRemoteRegisterContext;

    //----------------------------------------------------------------------
    // Accessors
    //----------------------------------------------------------------------
    bool
    IsRunning ( lldb::StateType state )
    {
        return    state == lldb::eStateRunning || IsStepping(state);
    }

    bool
    IsStepping ( lldb::StateType state)
    {
        return    state == lldb::eStateStepping;
    }
    bool
    CanResume ( lldb::StateType state)
    {
        return state == lldb::eStateStopped;
    }

    bool
    HasExited (lldb::StateType state)
    {
        return state == lldb::eStateExited;
    }

    bool
    ProcessIDIsValid ( ) const;

    void
    Clear ( );

    lldb_private::Flags &
    GetFlags ()
    {
        return m_flags;
    }

    const lldb_private::Flags &
    GetFlags () const
    {
        return m_flags;
    }

    virtual bool
    UpdateThreadList (lldb_private::ThreadList &old_thread_list, 
                      lldb_private::ThreadList &new_thread_list) override;

    lldb_private::Error
    LaunchAndConnectToDebugserver (const lldb_private::ProcessInfo &process_info);

    void
    KillDebugserverProcess ();

    void
    BuildDynamicRegisterInfo (bool force);

    void
    SetLastStopPacket (const StringExtractorGDBRemote &response);

    bool
    ParsePythonTargetDefinition(const lldb_private::FileSpec &target_definition_fspec);
    
    bool
    ParseRegisters(lldb_private::ScriptInterpreterObject *registers_array);

    const lldb::DataBufferSP
    GetAuxvData() override;

    lldb_private::StructuredData::ObjectSP
    GetExtendedInfoForThread (lldb::tid_t tid);

    void
    GetMaxMemorySize();

    //------------------------------------------------------------------
    /// Broadcaster event bits definitions.
    //------------------------------------------------------------------
    enum
    {
        eBroadcastBitAsyncContinue                  = (1 << 0),
        eBroadcastBitAsyncThreadShouldExit          = (1 << 1),
        eBroadcastBitAsyncThreadDidExit             = (1 << 2)
    };
    
    lldb_private::Flags m_flags;            // Process specific flags (see eFlags enums)
    GDBRemoteCommunicationClient m_gdb_comm;
    std::atomic<lldb::pid_t> m_debugserver_pid;
    StringExtractorGDBRemote m_last_stop_packet;
    lldb_private::Mutex m_last_stop_packet_mutex;
    GDBRemoteDynamicRegisterInfo m_register_info;
    lldb_private::Broadcaster m_async_broadcaster;
    lldb_private::HostThread m_async_thread;
    lldb_private::Mutex m_async_thread_state_mutex;
    typedef std::vector<lldb::tid_t> tid_collection;
    typedef std::vector< std::pair<lldb::tid_t,int> > tid_sig_collection;
    typedef std::map<lldb::addr_t, lldb::addr_t> MMapMap;
    tid_collection m_thread_ids; // Thread IDs for all threads. This list gets updated after stopping
    tid_collection m_continue_c_tids;                  // 'c' for continue
    tid_sig_collection m_continue_C_tids; // 'C' for continue with signal
    tid_collection m_continue_s_tids;                  // 's' for step
    tid_sig_collection m_continue_S_tids; // 'S' for step with signal
    uint64_t m_max_memory_size;       // The maximum number of bytes to read/write when reading and writing memory
    uint64_t m_remote_stub_max_memory_size;    // The maximum memory size the remote gdb stub can handle
    MMapMap m_addr_to_mmap_size;
    lldb::BreakpointSP m_thread_create_bp_sp;
    bool m_waiting_for_attach;
    bool m_destroy_tried_resuming;
    lldb::CommandObjectSP m_command_sp;
    int64_t m_breakpoint_pc_offset;

    bool
    StartAsyncThread ();

    void
    StopAsyncThread ();

    static lldb::thread_result_t
    AsyncThread (void *arg);

    static bool
    MonitorDebugserverProcess (void *callback_baton,
                               lldb::pid_t pid,
                               bool exited,
                               int signo,
                               int exit_status);

    lldb::StateType
    SetThreadStopInfo (StringExtractor& stop_packet);

    void
    ClearThreadIDList ();

    bool
    UpdateThreadIDList ();

    void
    DidLaunchOrAttach (lldb_private::ArchSpec& process_arch);

    lldb_private::Error
    ConnectToDebugserver (const char *host_port);

    const char *
    GetDispatchQueueNameForThread (lldb::addr_t thread_dispatch_qaddr,
                                   std::string &dispatch_queue_name);

    lldb_private::DynamicLoader *
    GetDynamicLoader () override;

private:
    //------------------------------------------------------------------
    // For ProcessGDBRemote only
    //------------------------------------------------------------------
    static bool
    NewThreadNotifyBreakpointHit (void *baton,
                         lldb_private::StoppointCallbackContext *context,
                         lldb::user_id_t break_id,
                         lldb::user_id_t break_loc_id);

    DISALLOW_COPY_AND_ASSIGN (ProcessGDBRemote);

};

#endif  // liblldb_ProcessGDBRemote_h_
