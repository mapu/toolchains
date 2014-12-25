#ifndef __ARCH_MAPU_STACKTRACE_HH__
#define __ARCH_MAPU_STACKTRACE_HH__

#include "base/trace.hh"
#include "cpu/static_inst.hh"
#include "debug/Stack.hh"

class ThreadContext;

namespace MapuISA
{

class ProcessInfo
{
  private:
    ThreadContext *tc;

    int thread_info_size;
    int task_struct_size;
    int task_off;
    int pid_off;
    int name_off;

  public:
    ProcessInfo(ThreadContext *_tc);

    Addr task(Addr ksp) const;
    int pid(Addr ksp) const;
    std::string name(Addr ksp) const;
};

class StackTrace
{
  protected:
    typedef MapuISA::MachInst MachInst;
  private:
    ThreadContext *tc;
    std::vector<Addr> stack;

  private:
    bool isEntry(Addr addr);
    bool decodePrologue(Addr sp, Addr callpc, Addr func, int &size, Addr &ra);
    bool decodeSave(MachInst inst, int &reg, int &disp);
    bool decodeStack(MachInst inst, int &disp);

    void trace(ThreadContext *tc, bool is_call);

  public:
    StackTrace();
    StackTrace(ThreadContext *tc, StaticInstPtr inst);
    ~StackTrace();

    void clear()
    {
        tc = 0;
        stack.clear();
    }

    bool valid() const { return tc != NULL; }
    bool trace(ThreadContext *tc, StaticInstPtr inst);

  public:
    const std::vector<Addr> &getstack() const { return stack; }

    static const int user = 1;
    static const int console = 2;
    static const int unknown = 3;

#if TRACING_ON
  private:
    void dump();

  public:
    void dprintf() { if (DTRACE(Stack)) dump(); }
#else
  public:
    void dprintf() {}
#endif
};

inline bool
StackTrace::trace(ThreadContext *tc, StaticInstPtr inst)
{
    if (!inst->isCall() && !inst->isReturn())
        return false;

    if (valid())
        clear();

    trace(tc, !inst->isReturn());
    return true;
}

}

#endif // __ARCH_MAPU_STACKTRACE_HH__
