#ifndef __UCP_LINUX_PROCESS_HH__
#define __UCP_LINUX_PROCESS_HH__

#include "arch/ucp/linux/linux.hh"
#include "arch/ucp/process.hh"
#include "sim/eventq.hh"

/// A process with emulated Ucp/Linux syscalls.
class UcpSPUProcess : public UcpLiveProcess
{
  bool block;
  public:
    /// Constructor.
    UcpSPUProcess(LiveProcessParams * params, ObjectFile *objFile);

    virtual SyscallDesc* getDesc(int callnum);

    /// The target system's hostname.
    static const char *hostname;

    /// ID of the thread group leader for the process
    uint64_t __tgid;

    /// Array of syscall descriptors, indexed by call number.
    static SyscallDesc syscallDescs[];
    const int Num_Syscall_Descs;

    bool isBlockedByMPU() { return block; }
    void setBlockedByMPU(bool b) { block = b; }
};

/// A process with emulated Ucp/Linux syscalls.
class UcpMPUProcess : public LiveProcess
{
  public:
    /// Constructor.
    UcpMPUProcess(LiveProcessParams * params, ObjectFile *objFile);

    /// The target system's hostname.
    static const char *hostname;

    /// ID of the thread group leader for the process
    uint64_t __tgid;

    UcpISA::IntReg getSyscallArg(ThreadContext *tc, int &i);
    void setSyscallArg(ThreadContext *tc, int i, UcpISA::IntReg val);
    void setSyscallReturn(ThreadContext *tc, SyscallReturn return_value);
    virtual SyscallDesc* getDesc(int callnum);

    void initState();
};

#endif // __UCP_LINUX_PROCESS_HH__
