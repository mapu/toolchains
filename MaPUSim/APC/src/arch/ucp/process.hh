#ifndef __UCP_PROCESS_HH__
#define __UCP_PROCESS_HH__

#include <string>
#include <vector>

#include "sim/process.hh"
#include "base/loader/csu_object.hh"
#include "sim/full_system.hh"

class LiveProcess;
class ObjectFile;
class System;

class UcpLiveProcess : public LiveProcess
{
  CsuObject *localCsu;
  protected:
    UcpLiveProcess(LiveProcessParams * params, ObjectFile *objFile);

    void initState();

    template<class IntType>
    void argsInit(int pageSize);

  public:
    UcpISA::IntReg getSyscallArg(ThreadContext *tc, int &i);
    void setSyscallArg(ThreadContext *tc, int i, UcpISA::IntReg val);
    void setSyscallReturn(ThreadContext *tc, SyscallReturn return_value);
    CsuObject *getCsuPtr() const {
      if (MaPUSystem)
        return dynamic_cast<CsuObject *>(objFile);
      else
        return localCsu;
    }
};

#define TheLiveProcess UcpLiveProcess
#endif // __UCP_PROCESS_HH__
