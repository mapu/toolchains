
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

#include "arch/ucp/system.hh"
#include "arch/ucp/cpu.hh"
#include "arch/ucp/process.hh"
#include "arch/vtophys.hh"
#include "base/loader/hex_file.hh"
#include "base/loader/object_file.hh"
#include "base/loader/csu_object.hh"
#include "base/loader/symtab.hh"
#include "base/trace.hh"
#include "mem/physical.hh"
#include "params/UcpSystem.hh"
#include "sim/byteswap.hh"
#include "sim/sim_exit.hh"
#include "sim/full_system.hh"
#include "debug/MapuCSU.hh"

using namespace LittleEndianGuest;

void UcpSystem::CsuSkedEvent::process() {
  assert(MaPUSystem);
  for (unsigned i = 0; i < system->numContexts(); i+=2) {
    TheCPU *cpu = dynamic_cast<TheCPU *>(system->getThreadContext(i)->getCpuPtr());
    if (cpu) {
      cpu->scheduleCTickEvent(0);
      if (CsuObject::isArm && !system->dataEvent) {
        system->dataEvent = new DataEvent(system, CsuObject::devfd, POLLIN);
        pollQueue.schedule(system->dataEvent);
        UcpLiveProcess *p = dynamic_cast<UcpLiveProcess *>(
          system->getThreadContext(i)->getProcessPtr());
        assert(p);
        CsuObject *csuObj = p->getCsuPtr();
        assert(csuObj);
        csuObj->init(system->numContexts()/2);
      }
    }
  }
}

UcpSystem::UcpSystem(Params *p)
  : System(p), csuSkedEvent(this), dataEvent(NULL) {
  if (MaPUSystem) {
    schedule(csuSkedEvent, 0);
  }
}

UcpSystem::~UcpSystem() {
  if (dataEvent) delete dataEvent;
}

void UcpSystem::DataEvent::process(int revent) {
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
      UcpLiveProcess *p = dynamic_cast<UcpLiveProcess *>(
        system->getThreadContext(buf->core_id * 2)->getProcessPtr());
      assert(p);
      CsuObject *csuObj = p->getCsuPtr();
      assert(csuObj);
      csuObj->pkt_buf.push_back(buf);
      buf = new CsuObject::csu_pkt();
      size -= sizeof(CsuObject::csu_pkt);
    } else break;
  } while (((ret == -1) && (errno == EINTR)) || (size > 0));
  delete buf;
}

UcpSystem *
UcpSystemParams::create() {
  return new UcpSystem(this);
}

