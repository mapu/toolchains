#ifndef __MAPU_PROCESS_HH__
#define __MAPU_PROCESS_HH__

#include <string>
#include <vector>

#include "sim/process.hh"
#include "base/loader/csu_object.hh"
#include "sim/full_system.hh"

class LiveProcess;
class ObjectFile;
class System;

class MapuLiveProcess : public LiveProcess
{
  CsuObject *localCsu;
  protected:
    MapuLiveProcess(LiveProcessParams * params, ObjectFile *objFile);

    void initState();

    template<class IntType>
    void argsInit(int pageSize);

  public:
    MapuISA::IntReg getSyscallArg(ThreadContext *tc, int &i);
    void setSyscallArg(ThreadContext *tc, int i, MapuISA::IntReg val);
    void setSyscallReturn(ThreadContext *tc, SyscallReturn return_value);
    CsuObject *getCsuPtr() const {
      if (MaPUSystem)
        return dynamic_cast<CsuObject *>(objFile);
      else
        return localCsu;
    }
};

#define TheLiveProcess MapuLiveProcess
#endif // __MAPU_PROCESS_HH__
