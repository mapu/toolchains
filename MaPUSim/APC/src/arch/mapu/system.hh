#ifndef __ARCH_MAPU_SYSTEM_HH__
#define __ARCH_MAPU_SYSTEM_HH__

#include <string>
#include <vector>

#include "base/loader/hex_file.hh"
#include "base/loader/symtab.hh"
#include "base/pollevent.hh"
#include "cpu/pc_event.hh"
#include "kern/system_events.hh"
#include "params/MapuSystem.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class MapuSystem: public System {
private:
  class CsuSkedEvent : public Event {
    MapuSystem *system;
  public:
    CsuSkedEvent(MapuSystem *_sys) :
      Event(Sim_Exit_Pri + 2), system(_sys) {}
    ~CsuSkedEvent() {}

    void process();
  };
  class DataEvent : public PollEvent {
    MapuSystem *system;
  public:
    DataEvent(MapuSystem *_sys, int fd, int e) :
      PollEvent(fd, e), system(_sys) {}
    ~DataEvent() {}

    void process(int revent);
  };
public:
  typedef MapuSystemParams Params;
  MapuSystem(Params *p);
  ~MapuSystem();
  CsuSkedEvent csuSkedEvent;
  DataEvent *dataEvent;
};

#endif

