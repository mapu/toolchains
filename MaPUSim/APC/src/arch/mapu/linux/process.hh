#ifndef __MAPU_LINUX_PROCESS_HH__
#define __MAPU_LINUX_PROCESS_HH__

#include "arch/mapu/linux/linux.hh"
#include "arch/mapu/process.hh"
#include "sim/eventq.hh"

/// A process with emulated Mapu/Linux syscalls.
class MapuSPUProcess : public MapuLiveProcess
{
  bool block;
  public:
    /// Constructor.
    MapuSPUProcess(LiveProcessParams * params, ObjectFile *objFile);

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

/// A process with emulated Mapu/Linux syscalls.
class MapuMPUProcess : public LiveProcess
{
  public:
    /// Constructor.
    MapuMPUProcess(LiveProcessParams * params, ObjectFile *objFile);

    /// The target system's hostname.
    static const char *hostname;

    /// ID of the thread group leader for the process
    uint64_t __tgid;

    MapuISA::IntReg getSyscallArg(ThreadContext *tc, int &i);
    void setSyscallArg(ThreadContext *tc, int i, MapuISA::IntReg val);
    void setSyscallReturn(ThreadContext *tc, SyscallReturn return_value);
    virtual SyscallDesc* getDesc(int callnum);

    void initState();
};

#endif // __MAPU_LINUX_PROCESS_HH__
