
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>

#include "arch/mapu/system.hh"
#include "arch/mapu/cpu.hh"
#include "arch/vtophys.hh"
#include "base/loader/hex_file.hh"
#include "base/loader/object_file.hh"
#include "base/loader/csu_object.hh"
#include "base/loader/symtab.hh"
#include "base/trace.hh"
#include "mem/physical.hh"
#include "params/MapuSystem.hh"
#include "sim/byteswap.hh"
#include "sim/sim_exit.hh"
#include "sim/full_system.hh"
#include "debug/MapuCSU.hh"

using namespace LittleEndianGuest;

void MapuSystem::CsuSkedEvent::process() {
  assert(MaPUSystem);
  for (unsigned i = 0; i < system->numContexts(); i+=2) {
    MapuCPU *cpu = dynamic_cast<MapuCPU *>(system->getThreadContext(i)->getCpuPtr());
    if (cpu) {
      cpu->scheduleCTickEvent(0);
      if (CsuObject::isArm && !system->dataEvent) {
        system->dataEvent = new DataEvent(system, CsuObject::devfd, POLLIN);
        pollQueue.schedule(system->dataEvent);
        LiveProcess *p = dynamic_cast<LiveProcess *>(
          system->getThreadContext(i)->getProcessPtr());
        assert(p);
        CsuObject *csuObj = dynamic_cast<CsuObject *>(p->getObjFilePtr());
        csuObj->init(system->numContexts()/2);
      }
    }
  }
}

MapuSystem::MapuSystem(Params *p)
  : System(p), csuSkedEvent(this), dataEvent(NULL) {
  if (MaPUSystem) {
    schedule(csuSkedEvent, 0);
  }
}

MapuSystem::~MapuSystem() {
  if (dataEvent) delete dataEvent;
}

void MapuSystem::DataEvent::process(int revent) {
  CsuObject::csu_pkt *buf = new CsuObject::csu_pkt();
  DPRINTFR(MapuCSU, "Writing APC from ARM.\n");
  int ret = 0;
  uint32_t size = 0;
  char testbuf;
  if (recv(CsuObject::devfd, &testbuf, 1, MSG_PEEK | MSG_DONTWAIT) == 0)
    exitSimLoop("Remote ARM is disconnected.", 0);
  ::ioctl(CsuObject::devfd, FIONREAD, &size);
  do {
    ret = read(CsuObject::devfd, buf, sizeof(struct CsuObject::csu_pkt));
    if (ret == sizeof(CsuObject::csu_pkt)) {
      LiveProcess *p = dynamic_cast<LiveProcess *>(
        system->getThreadContext(buf->core_id * 2)->getProcessPtr());
      assert(p);
      CsuObject *csuObj = dynamic_cast<CsuObject *>(p->getObjFilePtr());
      assert(csuObj);
      csuObj->pkt_buf.push_back(buf);
      buf = new CsuObject::csu_pkt();
      size -= sizeof(CsuObject::csu_pkt);
    } else break;
  } while (((ret == -1) && (errno == EINTR)) || (size > 0));
  delete buf;
}

MapuSystem *
MapuSystemParams::create() {
  return new MapuSystem(this);
}

