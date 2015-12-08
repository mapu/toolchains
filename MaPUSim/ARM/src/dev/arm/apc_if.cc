/*
 * apc_if.cc
 *
 *  Created on: Mar 13, 2014
 *      Author: wangl
 */


#include <sys/ioctl.h>
#include <sys/termios.h>
#include <poll.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <cctype>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "base/atomicio.hh"
#include "base/misc.hh"
#include "base/output.hh"
#include "base/socket.hh"
#include "base/trace.hh"
#include "dev/platform.hh"
#include "debug/APCInterface.hh"
#include "dev/arm/amba_device.hh"
#include "dev/arm/apc_if.hh"

using namespace std;

/*
 * APCInterface code
 */
APCInterface::APCInterface(const Params *p) :
  AmbaIntDevice(p, 4096 * 1024 * NUM_APES),
  listenEvent(NULL), dataEvent(NULL), data_fd(-1), syncEvent(this, &data_fd) {
  if (p->port) listen(p->port);
  for (unsigned i = 0; i < NUM_APES; i++)
    memset(&ape[i], 0, sizeof(ape[i]));
  for (unsigned i = 0; i < NUM_APES; i++)
    ape[i].csu_if.MailNum = 0x400;
}

APCInterface::~APCInterface() {
  if (data_fd != -1) {
    struct csu_pkt cpkt;
    cpkt.core_id = 0;
    cpkt.ape.csu_if.VPUControl = CMD_SHUTDOWN;
    atomic_write(data_fd, &cpkt, sizeof(struct csu_pkt));
    ::close(data_fd);
    data_fd = -1;
  }

  if (listenEvent) delete listenEvent;

  if (dataEvent) delete dataEvent;
}


///////////////////////////////////////////////////////////////////////
// socket creation and APC attach
//

void APCInterface::listen(int port) {
  if (ListenSocket::allDisabled()) {
    warn_once("Sockets disabled, not accepting terminal connections");
    return;
  }

  while (!listener.listen(port, true)) {
    DPRINTF(APCInterface, ": can't bind address terminal port %d inuse PID %d\n",
            port, getpid());
    port++;
  }

  //int p1, p2;
  //p2 = name().rfind('.') - 1;
  //p1 = name().rfind('.', p2);
  ccprintf(cerr, "Listening for %s connection on port %d\n",
           name()/*.substr(p1+1,p2-p1)*/, port);

  listenEvent = new ListenEvent(this, listener.getfd(), POLLIN);
  pollQueue.schedule(listenEvent);

  schedule(syncEvent, 0);
}

void APCInterface::accept() {
  if (!listener.islistening())
    panic("%s: cannot accept a connection if not listening!", name());

  int fd = listener.accept(true);
  if (data_fd != -1) {
    DPRINTFN("APC is already attached.\n");
    ::close(fd);
    return;
  }

  data_fd = fd;
  dataEvent = new DataEvent(this, data_fd, POLLIN);
  pollQueue.schedule(dataEvent);

  DPRINTF(APCInterface, "attach APC.\n");
}

void APCInterface::detach() {
  if (data_fd != -1) {
    ::close(data_fd);
    data_fd = -1;
  }

  pollQueue.remove(dataEvent);
  delete dataEvent;
  dataEvent = NULL;

  DPRINTF(APCInterface, "detach APC.\n");
}

void APCInterface::data() {
  if (data_fd < 0)
      panic("APC not properly attached.\n");

  size_t ret;
  uint32_t pkt[3];
  uint32_t size = 0;
  ::ioctl(data_fd, FIONREAD, &size);
  do {
    ret = ::read(data_fd, pkt, 4 * 3);
    if (ret == 12) {
      DPRINTF(APCInterface, "Updating from APC %d at offset: %#x\n", pkt[0], pkt[1]);
      assert(pkt[0] < NUM_APES);
      assert(pkt[1] < sizeof(union csu_mmap));
      ape[pkt[0]].mem[pkt[1] / 4] = pkt[2];
      if ((pkt[1]) == 0xB0) {
        ape[pkt[0]].csu_if.MailNum |= 0x1;
      }
      else if ((pkt[1]) == 0xB8) {
        ape[pkt[0]].csu_if.MailNum |= 0x10000;
      }
      size -= 4 * 3;
    } else break;
  } while (((ret == -1) && (errno == EINTR)) || size > 0);

  if (ret < 0)
      DPRINTFN("Read failed.\n");
}

size_t APCInterface::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  Addr daddr = pkt->getAddr() - pioAddr;
  if (data_fd == -1) accept();
  pkt->allocate();
  uint32_t data;

  uint32_t core_id = daddr / (4096 * 1024);
  uint32_t offset = daddr % (4096 * 1024);
  DPRINTF(APCInterface, "Reading from APC %d at offset: %#x\n", core_id, offset);
  assert(core_id < NUM_APES);
  assert(offset < sizeof(union csu_mmap));

  data = ape[core_id].mem[offset / 4];

  switch(pkt->getSize()) {
    case 1:
      pkt->set<uint8_t>(data);
      break;
    case 2:
      pkt->set<uint16_t>(data);
      break;
    case 4:
      pkt->set<uint32_t>(data);
      break;
    default:
      panic("APC read size too big?\n");
      break;
  }
  if (offset == 0xB0) {
    ape[core_id].csu_if.MailNum &= 0xFFFFFF00;
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (data_fd < 0)
      warn("APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = atomic_write(data_fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF(APCInterface, "Send csu package to core %d.\n", core_id);
    if (ret < sizeof(struct csu_pkt)) {
      warn("APC is incompatible, accept size is %d.\n", ret);
      detach();
    }
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
  } else if (offset == 0xB8) {
    ape[core_id].csu_if.MailNum &= 0xFF00FFFF;
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (data_fd < 0)
      warn("APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = atomic_write(data_fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF(APCInterface, "Send csu package to core %d.\n", core_id);
    if (ret < sizeof(struct csu_pkt)) {
      warn("APC is incompatible, accept size is %d.\n", ret);
      detach();
    }
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

// Terminal output.
size_t APCInterface::write(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  Addr daddr = pkt->getAddr() - pioAddr;
  if (data_fd == -1) accept();

  uint32_t core_id = daddr / (4096 * 1024);
  uint32_t offset = daddr % (4096 * 1024);
  DPRINTF(APCInterface, "Writing to APC %d at offset: %#x\n", core_id, offset);
  assert(core_id < NUM_APES);
  assert(offset < sizeof(union csu_mmap));

  uint32_t data = 0;

  switch(pkt->getSize()) {
    case 1:
      data = pkt->get<uint8_t>();
      break;
    case 2:
      data = pkt->get<uint16_t>();
      break;
    case 4:
      data = pkt->get<uint32_t>();
      break;
    default:
      panic("APC write size too big?\n");
      break;
  }

  ape[core_id].mem[offset / 4] = data;

  //if (data_fd < 0) panic("APC not properly attached.\n");
  if (offset == 0x0) {
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (ape[core_id].csu_if.VPUControl == 0) {
      ape[core_id].csu_if.VPUControl = CMD_START;
      ape[core_id].csu_if.VPUStatus = 1;
    } else if (ape[core_id].csu_if.VPUControl == 1) {
      ape[core_id].csu_if.VPUControl = CMD_STOP;
    } else if (ape[core_id].csu_if.VPUControl == CMD_SHUTDOWN) {
      ape[core_id].csu_if.VPUControl = CMD_SHUTDOWN;
    } else {
      warn("Unknown APC command.");
      ape[core_id].csu_if.VPUControl = CMD_NONE;
    }
    if (data_fd < 0)
      warn("APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = atomic_write(data_fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF(APCInterface, "Send csu package to core %d.\n", core_id);
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
    if (ret < sizeof(struct csu_pkt)) {
      warn("APC is incompatible, accept size is %d.\n", ret);
      detach();
    } else {
      ape[core_id].csu_if.VPUControl = CMD_NONE;
    }
  } else if (offset == 0x78) {
    ape[core_id].csu_if.dma.DMACommandStatus = 1;
    if (data_fd < 0)
      warn("APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = atomic_write(data_fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF(APCInterface, "Send csu package to core %d.\n", core_id);
    if (ret < sizeof(struct csu_pkt)) {
      warn("APC is incompatible, accept size is %d.\n", ret);
      detach();
    }
  } else if (offset == 0xA8) {
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (ape[core_id].csu_if.MailNum & 0xFF00) {
      ape[core_id].csu_if.MailNum = ape[core_id].csu_if.MailNum - 0x100;
      if (data_fd < 0)
        warn("APC not properly attached.\n");
      struct csu_pkt cpkt = {core_id, ape[core_id]};
      ssize_t ret = atomic_write(data_fd, &cpkt, sizeof(struct csu_pkt));
      DPRINTF(APCInterface, "Send csu package to core %d.\n", core_id);
      if (ret < sizeof(struct csu_pkt)) {
        warn("APC is incompatible, accept size is %d.\n", ret);
        detach();
      }
    } else if (offset == 0xC8) {

    }
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
  }
  pkt->makeAtomicResponse();
  return pioDelay;
}

/*
 * Poll event for the listen socket
 */
APCInterface::ListenEvent::ListenEvent(APCInterface *a, int fd, int e) :
  PollEvent(fd, e), apc(a) {
}

void APCInterface::ListenEvent::process(int revent) {
  apc->accept();
}

/*
 * Poll event for the data socket
 */
APCInterface::DataEvent::DataEvent(APCInterface *a, int fd, int e) :
  PollEvent(fd, e), apc(a) {
}

void APCInterface::DataEvent::process(int revent) {
  if (revent & POLLIN) apc->data();
  else if (revent & POLLNVAL) apc->detach();
}

extern int shmid;
void APCInterface::SyncEvent::process() {
  if (*fd_p == -1) apc->schedule(this, 0);
  else if (shmid) shmctl(shmid, IPC_RMID, NULL);
}

APCInterface *APCInterfaceParams::create() {
  return new APCInterface(this);
}
