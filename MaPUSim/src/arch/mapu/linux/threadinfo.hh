#ifndef __ARCH_MAPU_LINUX_LINUX_TREADNIFO_HH__
#define __ARCH_MAPU_LINUX_LINUX_TREADNIFO_HH__

#include "cpu/thread_context.hh"
#include "sim/system.hh"
#include "sim/vptr.hh"

namespace Linux {

class ThreadInfo
{
  private:
    ThreadContext *tc;
    System *sys;
    Addr pcbb;

    template <typename T>
    bool
    get_data(const char *symbol, T &data)
    {
        Addr addr = 0;
        if (!sys->kernelSymtab->findAddress(symbol, addr))
            return false;

        CopyOut(tc, &data, addr, sizeof(T));

        data = MapuISA::gtoh(data);

        return true;
    }

  public:
    ThreadInfo(ThreadContext *_tc, Addr _pcbb = 0)
        : tc(_tc), sys(tc->getSystemPtr()), pcbb(_pcbb)
    {

    }
    ~ThreadInfo()
    {}

    inline Addr
    curThreadInfo()
    {
        panic("curThreadInfo not implemented for MAPU");
        Addr addr = pcbb;
        Addr sp;

        if (!addr)
            addr = tc->readMiscRegNoEffect(0/*MapuISA::IPR_PALtemp23*/);

        PortProxy &p = tc->getPhysProxy();
        p.readBlob(addr, (uint8_t *)&sp, sizeof(Addr));

        return sp & ~ULL(0x3fff);
    }

    inline Addr
    curTaskInfo(Addr thread_info = 0)
    {
        int32_t offset;
        if (!get_data("thread_info_task", offset))
            return 0;

        if (!thread_info)
            thread_info = curThreadInfo();

        Addr addr;
        CopyOut(tc, &addr, thread_info + offset, sizeof(addr));

        return addr;
    }

    int32_t
    curTaskPID(Addr thread_info = 0)
    {
        Addr offset;
        if (!get_data("task_struct_pid", offset))
            return -1;

        int32_t pid;
        CopyOut(tc, &pid, curTaskInfo(thread_info) + offset, sizeof(pid));

        return pid;
    }

    int64_t
    curTaskStart(Addr thread_info = 0)
    {
        Addr offset;
        if (!get_data("task_struct_start_time", offset))
            return -1;

        int64_t data;
        // start_time is actually of type timespec, but if we just
        // grab the first long, we'll get the seconds out of it
        CopyOut(tc, &data, curTaskInfo(thread_info) + offset, sizeof(data));

        return data;
    }

    std::string
    curTaskName(Addr thread_info = 0)
    {
        int32_t offset;
        int32_t size;

        if (!get_data("task_struct_comm", offset))
            return "FailureIn_curTaskName";

        if (!get_data("task_struct_comm_size", size))
            return "FailureIn_curTaskName";

        char buffer[size + 1];
        CopyStringOut(tc, buffer, curTaskInfo(thread_info) + offset, size);

        return buffer;
    }
};

} // namespace Linux

#endif // __ARCH_MAPU_LINUX_LINUX_THREADINFO_HH__
