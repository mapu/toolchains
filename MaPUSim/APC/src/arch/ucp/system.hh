#ifndef __ARCH_UCP_SYSTEM_HH__
#define __ARCH_UCP_SYSTEM_HH__

#include <string>
#include <vector>

#include "base/loader/hex_file.hh"
#include "base/loader/symtab.hh"
#include "base/pollevent.hh"
#include "cpu/pc_event.hh"
#include "kern/system_events.hh"
#include "params/UcpSystem.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class UcpSystem: public System {
private:
  class CsuSkedEvent : public Event {
    UcpSystem *system;
  public:
    CsuSkedEvent(UcpSystem *_sys) :
      Event(Sim_Exit_Pri + 2), system(_sys) {}
    ~CsuSkedEvent() {}

    void process();
  };
  class DataEvent : public PollEvent {
    UcpSystem *system;
  public:
    DataEvent(UcpSystem *_sys, int fd, int e) :
      PollEvent(fd, e), system(_sys) {}
    ~DataEvent() {}

    void process(int revent);
  };
public:
  typedef UcpSystemParams Params;
  UcpSystem(Params *p);
  ~UcpSystem();
  CsuSkedEvent csuSkedEvent;
  DataEvent *dataEvent;
};

#endif

