/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Steve Reinhardt
 *          Ali Saidi
 */

#include <cassert>
#include <string>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>

#include "base/loader/csu_object.hh"
#include "base/bitfield.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "mem/port_proxy.hh"
#include "debug/MapuCSU.hh"
#include "sim/byteswap.hh"
#include "arch/vtophys.hh"
#include "sim/sim_exit.hh"
#include "sim/full_system.hh"

using namespace std;

int CsuObject::devfd = -1;

bool CsuObject::isArm = false;

/*
 * remote_connect()
 * Return's a socket connected to a remote host. Properly bind's to a local
 * port or source address if needed. Return's -1 on failure.
 */
int remote_connect(const char *host, const char *port, struct addrinfo hints) {
  struct addrinfo *res, *res0;
  int s, error;

  if ((error = getaddrinfo(host, port, &hints, &res)))
    panic("getaddrinfo: %s", gai_strerror(error));

  res0 = res;
  do {
    if ((s = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) < 0)
      continue;

    if (connect(s, res0->ai_addr, res0->ai_addrlen) == 0) break;

    close(s);
    s = -1;
  } while ((res0 = res0->ai_next) != NULL);

  freeaddrinfo(res);

  return (s);
}

ObjectFile *CsuObject::tryFile(const string &fname, int fd) {
  struct csu_external_if *csu = new struct csu_external_if;
  if (isArm) {
    if (devfd == -1) {
      int s;
      struct addrinfo hints;
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;

      s = remote_connect("localhost", fname.c_str(), hints);

      if (s == -1) return NULL;
      devfd = s;
    }
    return new CsuObject(fname, devfd, (uint8_t *)csu, ObjectFile::Mapu,
                         ObjectFile::MaPU_SPU);
  } else {
    char verify[4] = "";
    ioctl(fd, MAPU_IOCVERIFY, verify);
    if (!strcmp(verify, "APE")) {
      if (ioctl(fd, MAPU_IOCSIMREG, 0) < 0) {
        fatal("Simulator failed to proceed registration.\n");
        return NULL;
      }
      devfd = fd;
      return new CsuObject(fname, fd, (uint8_t *)csu, ObjectFile::Mapu,
                           ObjectFile::MaPU_SPU);
    }
  }
  delete csu;
  return NULL;
}

CsuObject::CsuObject(const string &_filename)
  : ObjectFile(_filename, 0, 0, NULL, ObjectFile::Mapu, ObjectFile::MaPU_SPU),
    extCsuCtx(new struct csu_external_if),
    intCsuCtx(new struct csu_internal_if) {
  init();
}

CsuObject::CsuObject(const string &_filename,
                     int _fd,
                     uint8_t *_data,
                     Arch _arch,
                     OpSys _opSys)
  : ObjectFile(_filename, _fd, 0, _data, _arch, _opSys),
    extCsuCtx((struct csu_external_if *)_data),
    intCsuCtx(new struct csu_internal_if) {
  init();
}

void CsuObject::init() {
  memset(intCsuCtx, 0, sizeof(struct csu_internal_if));
  intCsuCtx->MailboxIn = new struct inbox_if;
  memset(intCsuCtx->MailboxIn, 0, sizeof(struct inbox_if));
  intCsuCtx->dma_queue = new struct dma_if;
  memset(intCsuCtx->dma_queue, 0, sizeof(struct dma_if));
  memset(extCsuCtx, 0, sizeof(struct csu_external_if));
  extCsuCtx->dma_queue = new struct dma_if;
  memset(extCsuCtx->dma_queue, 0, sizeof(struct dma_if));
  intDMAStatus = 0xFFFFFFFF;
  extDMAStatus = 0xFFFFFFFF;
  pendingEventStatus = 0;
  entry = 0;
}

void CsuObject::init(int cpuid) {
  for (int i = 0; i < cpuid; i++) {
    ChangeStatus(i, 0xA0, extDMAStatus);
    ChangeStatus(i, 0x88, 4-extCsuCtx->dma_queue_len);
  }
}

void CsuObject::sendData(int cpuid, const void *_data) {
  lseek(descriptor, cpuid << 24, SEEK_SET);
  while (ioctl(descriptor, MAPU_IOCAPE2DDR, _data) < 0) {
    if (errno == EBUSY) continue;
    fatal("Transmitting from CSU to DDR failed.\n");
  }
}

void CsuObject::getData(int cpuid, void *_data) {
  lseek(descriptor, cpuid << 24, SEEK_SET);
  while (ioctl(descriptor, MAPU_IOCDDR2APE, _data) < 0) {
    if (errno == EBUSY) continue;
    fatal("Transmitting from DDR to CSU failed.\n");
  }
}

void CsuObject::update(int cpuid) {
  union csu_mmap income;
  struct dma_if *p;
  if (isArm) {
    if (pkt_buf.empty()) return;
    income = pkt_buf.front()->ape;
    delete pkt_buf.front();
    pkt_buf.erase(pkt_buf.begin());
  } else {
    lseek(descriptor, cpuid << 24, SEEK_SET);
    while (ioctl(descriptor, MAPU_IOCSTATUS, &income) < 0) {
      if (errno == EBUSY) continue;
      fatal("Updating CSU status failed.\n");
    }
  }
  // parse extCsuCtx
  if (income.csu_if.VPUControl == CMD_START) {
    extCsuCtx->VPUStatus = 1;
    ChangeStatus(cpuid, 0x8, extCsuCtx->VPUStatus);
  } else if (income.csu_if.VPUControl == CMD_STOP) {
    extCsuCtx->VPUStatus = 0;
    ChangeStatus(cpuid, 0x8, extCsuCtx->VPUStatus);
  } else if (income.csu_if.VPUControl == CMD_SHUTDOWN) {
    DPRINTF(MapuCSU, "CSU Shutdown command.\n");
    exitSimLoop("Shutdown APC is invoked by ARM.", 0);
  }
  if (income.csu_if.dma.DMACommandStatus == 1) {
    // New DMA request
    if (extCsuCtx->dma_queue_len < 4) {
      DPRINTF(MapuCSU, "Put a CSU %s request to DMA queue.\n", income.csu_if.dma.DMACmd ? "Get" : "Send");
      p = extCsuCtx->dma_queue;
      while (p->Next != NULL) p = p->Next;
      p->Next = new struct dma_if;
      memcpy(p->Next, &income.csu_if.dma, sizeof(struct dma_if));
      p->Next->Next = NULL;
      extCsuCtx->dma_queue_len++;
      extDMAStatus &= ~(1 << income.csu_if.dma.DMAGroupNum);
      ChangeStatus(cpuid, 0x88, 4 - extCsuCtx->dma_queue_len);
      ChangeStatus(cpuid, 0xA0, extDMAStatus);
      ChangeStatus(cpuid, 0x80, 0);
    }
  }
  uint8_t *num = (uint8_t*)(&income.csu_if.MailNum) + 1;
  if (*num != intCsuCtx->MailboxIn->queue_len) {
    if (intCsuCtx->MailboxIn->queue_len < 4) {
      intCsuCtx->MailboxIn
        ->Message[(intCsuCtx->MailboxIn->ptr +
                  intCsuCtx->MailboxIn->queue_len) % 4] = income.csu_if.MailBoxIn;
      if (intCsuCtx->MailboxIn->queue_len == 0) {
        pendingEventStatus |= 1 << InMailEvent;
      }
      intCsuCtx->MailboxIn->queue_len++;
    }
  }
  num = (uint8_t*)(&income.csu_if.MailNum);
  if (num == 0 && intCsuCtx->MailboxOutInt) {
    delete intCsuCtx->MailboxOutInt;
    intCsuCtx->MailboxOutInt = NULL;
    pendingEventStatus |= 1 << OutMailEvent;
  }
  num = (uint8_t*)(&income.csu_if.MailNum) + 2;
  if (num == 0 && intCsuCtx->MailboxOut) {
    delete intCsuCtx->MailboxOut;
    intCsuCtx->MailboxOut = NULL;
    //pendingEventStatus |= 1 << 3;
  }
  updateStatus();
}

void CsuObject::ChangeStatus(int cpuid, Addr addr, uint32_t val) const {
  if (isArm) {
    uint32_t buf[3] = {(uint32_t)cpuid, (uint32_t)addr, val};
    write(devfd, buf, 3 * 4);
  } else {
    lseek(descriptor, (cpuid << 24) + addr, SEEK_SET);
    while (ioctl(descriptor, MAPU_IOCSTATUS_CHANGE, val) < 0) {
      if (errno == EBUSY) continue;
      fatal("Changing CSU status failed.\n");
    }
  }
}

bool CsuObject::readByAPE(int cpuid, Addr chid, TheISA::IntReg &val) {
  switch (chid) {
  case 0:
    if (intCsuCtx->EventStatus == NULL) {
      return false;
    } else {
      assert(*intCsuCtx->EventStatus);
      val = *intCsuCtx->EventStatus;
      return true;
    }

  case 3:
    val = intCsuCtx->EventMaskWrite;
    return true;
  case 0x13:
    val = intCsuCtx->GroupMaskWrite;
    return true;
  case 0x15:
    if (intCsuCtx->GroupStatus == NULL) {
      return false;
    } else {
      val = *intCsuCtx->GroupStatus;
      delete intCsuCtx->GroupStatus;
      intCsuCtx->GroupStatus = NULL;
      return true;
    }
  case 0x16:
    if (intCsuCtx->GroupListStalled == NULL) {
      return false;
    } else {
      val = *intCsuCtx->GroupListStalled;
      delete intCsuCtx->GroupListStalled;
      intCsuCtx->GroupListStalled = NULL;
      return true;
    }
  case 0x19:
    if (intCsuCtx->MailboxIn == NULL) {
      fatal("Mail Inbox does not exist.\n");
      return false;
    } else if (intCsuCtx->MailboxIn->queue_len) {
      val = intCsuCtx->MailboxIn->Message[intCsuCtx->MailboxIn->ptr];
      intCsuCtx->MailboxIn->ptr++;
      intCsuCtx->MailboxIn->queue_len--;
      if (MaPUSystem)
        ChangeStatus(cpuid, 0xC0, intCsuCtx->MailboxIn->queue_len);
      if (intCsuCtx->MailboxIn->queue_len == 0) {
        pendingEventStatus &= ~(1 << InMailEvent);
        updateStatus();
      }
      return true;
    } else {
      return false;
    }
  default:
    fatal("Read a write-only CSU register.\n");
    return false;
  }
}

uint32_t CsuObject::readCSUNum(Addr chid) {
  switch (chid) {
  case 0:
    return (intCsuCtx->EventStatus != NULL);
  case 0x11:
    return 8-intCsuCtx->dma_queue_len;
  case 0x15:
    return (intCsuCtx->GroupStatus != NULL);
  case 0x16:
    return (intCsuCtx->GroupListStalled != NULL);
  case 0x18:
    return (intCsuCtx->MailboxOut == NULL);
  case 0x19:
    return 4 - intCsuCtx->MailboxIn->queue_len;
  case 0x1A:
    return (intCsuCtx->MailboxOutInt == NULL);
  default:
    return 1;
  }
}

bool CsuObject::setByAPE(int cpuid, Addr chid, const TheISA::IntReg val) {
  struct dma_if *p = NULL;
  switch (chid) {
  case 0x1:
    intCsuCtx->EventMaskWrite = val;
    updateStatus();
    return true;
  case 0x2:
    intCsuCtx->EventAck = val;
    pendingEventStatus &= ~val;
    updateStatus();
    return true;
  case 0x11:
    if (intCsuCtx->dma_queue == NULL) {
      fatal("DMA queue does not exist.\n");
      return false;
    } else if (intCsuCtx->dma_queue_len == 8) {
      return false;
    } else {
      intCsuCtx->dma.DMACmd = val;
      p = intCsuCtx->dma_queue;
      while (p->Next != NULL) p = p->Next;
      p->Next = new struct dma_if;
      memcpy(p->Next, &intCsuCtx->dma, sizeof(struct dma_if));
      intCsuCtx->dma_queue_len++;
      intDMAStatus &= ~(1 << intCsuCtx->dma.DMAGroupNum);
      return true;
    }
  case 0x12:
    intCsuCtx->GroupMaskWrite = val;
    return true;
  case 0x14:
    assert(val);
    intCsuCtx->GroupStatusUpdate = val;
    if (val == 0x1) {
      if (intCsuCtx->GroupMaskWrite & intDMAStatus) {
        if (intCsuCtx->GroupStatus)
          *intCsuCtx->GroupStatus = intCsuCtx->GroupMaskWrite & intDMAStatus;
        else
          intCsuCtx->GroupStatus = new uint32_t(intCsuCtx->GroupMaskWrite &
                                                intDMAStatus);
        pendingEventStatus |= 1 << DMAGroupEvent;
        intCsuCtx->GroupStatusUpdate = 0;
      } else {
        if (intCsuCtx->GroupStatus) {
          delete intCsuCtx->GroupStatus;
          intCsuCtx->GroupStatus = NULL;
        }
        pendingEventStatus &= ~(1 << DMAGroupEvent);
      }
    } else if (val == 0x2) {
      if ((intCsuCtx->GroupMaskWrite & intDMAStatus) == intCsuCtx->GroupMaskWrite) {
        if (intCsuCtx->GroupStatus)
          *intCsuCtx->GroupStatus = intCsuCtx->GroupMaskWrite;
        else
          intCsuCtx->GroupStatus = new uint32_t(intCsuCtx->GroupMaskWrite);
        pendingEventStatus |= 1 << DMAGroupEvent;
        intCsuCtx->GroupStatusUpdate = 0;
      } else {
        if (intCsuCtx->GroupStatus) {
          delete intCsuCtx->GroupStatus;
          intCsuCtx->GroupStatus = NULL;
        }
        pendingEventStatus &= ~(1 << DMAGroupEvent);
      }
    } else {
      fatal("Unknown GroupStatusUpdate command.\n");
    }
    updateStatus();
    return true;
  case 0x17:
    intCsuCtx->GroupListAck = val;
    if (intCsuCtx->GroupListStalled) {
      delete intCsuCtx->GroupListStalled;
      intCsuCtx->GroupListStalled = NULL;
      return true;
    } else {
      warn("Chained DMA Stall acknowledge is received, "
           "but no stall Event is pending.\n");
      return true;
    }
  case 0x18:
    if (intCsuCtx->MailboxOut)
      return false;
    else {
      intCsuCtx->MailboxOut = new uint32_t(val);
      if (MaPUSystem)
        ChangeStatus(cpuid, 0xB0, val);
      //pendingEventStatus &= ~(1 << 3);
      updateStatus();
      return true;
    }
  case 0x1A:
    if (intCsuCtx->MailboxOutInt)
      return false;
    else {
      intCsuCtx->MailboxOutInt = new uint32_t(val);
      if (MaPUSystem)
        ChangeStatus(cpuid, 0xB8, val);
      pendingEventStatus &= ~(1 << OutMailEvent);
      updateStatus();
      return true;
    }

  default:
    if (chid >= 0x4 && chid <= 0x10) {
      ((uint32_t *)(&intCsuCtx->dma))[(chid - 4) * 2] = val;
      return true;
    }
    fatal("Write a read-only CSU register.\n");
  }
  return true;
}

void CsuObject::processFrontDMA(int cpuid, PortProxy &extProxy,
                                PortProxy &intProxy, bool isExternal) {
  PortProxy *gp = &extProxy;
  PortProxy *lp = &intProxy;
  struct dma_if *queue_head;
  if (isExternal) {
    assert(extCsuCtx->dma_queue && extCsuCtx->dma_queue_len);
    queue_head = extCsuCtx->dma_queue;
  } else {
    assert(intCsuCtx->dma_queue && intCsuCtx->dma_queue_len);
    queue_head = intCsuCtx->dma_queue;
  }

  struct dma_if *dma_conf = queue_head->Next;

  std::cerr << "DMA conf.:\n"
            << "Cmd: " << std::hex << dma_conf->DMACmd << "\n"
            << "LocalAddr: " << dma_conf->DMALocalAddr << "\n"
            << "LocalXNum: " << dma_conf->DMALocalXNum << "\n"
            << "LocalYNum: " << dma_conf->DMALocalYNum << "\n"
            << "LocalYStep: " << dma_conf->DMALocalYStep << "\n"
            << "LocalYAllNum: " << dma_conf->DMALocalYAllNum << "\n"
            << "LocalZStep: " << dma_conf->DMALocalZStep << "\n"
            << "GlobalAddr: " << dma_conf->DMAGlobalAddr << "\n"
            << "GlobalXNum: " << dma_conf->DMAGlobalXNum << "\n"
            << "GlobalYNum: " << dma_conf->DMAGlobalYNum << "\n"
            << "GlobalYStep: " << dma_conf->DMAGlobalYStep << "\n"
            << "GlobalYAllNum: " << dma_conf->DMAGlobalYAllNum << "\n"
            << "GlobalZStep: " << dma_conf->DMAGlobalZStep << "\n"
            << "GroupNum: " << dma_conf->DMAGroupNum << "\n";
  bool isSend = dma_conf->DMACmd & 0x1;
  bool isLXExt = dma_conf->DMALocalXNum % 64 ? (dma_conf->DMACmd & 0x4) : false;
  bool isLYExt = dma_conf->DMALocalYAllNum % dma_conf->DMALocalYNum ? (dma_conf->DMACmd & 0x8) : false;
  bool isGXExt = dma_conf->DMAGlobalXNum % 64 ? (dma_conf->DMACmd & 0x10) :false;
  bool isGYExt = dma_conf->DMAGlobalYAllNum ? (dma_conf->DMACmd & 0x20) : false;
  Addr curLStart = TheISA::vtophys(cpuid, dma_conf->DMALocalAddr);
  Addr curGStart = dma_conf->DMAGlobalAddr;

  uint32_t actLXNum = isLXExt || (dma_conf->DMALocalAddr == 0x200000) ?
                        64 : dma_conf->DMALocalXNum;
  uint32_t actLYNum = dma_conf->DMALocalYAllNum / dma_conf->DMALocalYNum + isLYExt;
  uint32_t lenL = dma_conf->DMALocalYAllNum * actLXNum;
  uint32_t actGXNum = isGXExt ? 64 : dma_conf->DMAGlobalXNum;
  uint32_t actGYNum = dma_conf->DMAGlobalYAllNum / dma_conf->DMAGlobalYNum + isGYExt;
  uint32_t lenG = dma_conf->DMAGlobalYAllNum * actGXNum;

  uint32_t lenA = lenL > lenG ? lenG : lenL;

  assert(lenA);
  uint8_t *data = NULL;

  uint64_t LXCnt = 0;
  uint64_t LYCnt = 0;
  uint64_t GXCnt = 0;
  uint64_t GYCnt = 0;
  Addr curLYBase = curLStart;
  Addr curGYBase = curGStart;

  enum Mode {
    Symmetric,
    Continuous,
    Abnormal
  };

  Mode m;
  if ((dma_conf->DMAGlobalYNum * dma_conf->DMAGlobalYStep ==
       dma_conf->DMAGlobalZStep) &&
      (dma_conf->DMALocalYNum * dma_conf->DMALocalYStep ==
       dma_conf->DMALocalZStep) &&
      (dma_conf->DMAGlobalXNum == dma_conf->DMAGlobalYStep) &&
      (dma_conf->DMALocalXNum == dma_conf->DMALocalYStep)) {
    m = Continuous;
  } else if (actLXNum == actGXNum) {
      m = Symmetric;
  } else {
    m = Abnormal;
  }

  if (dma_conf->DMALocalAddr >= DDR0START) lp = gp;

  if (isExternal && !CsuObject::isArm) {
    DPRINTF(MapuCSU, "Received a CSU transmission request, from %#08x to %#08x.\n",
            curLStart, curLStart + lenA - 1);
    if (MaPUSystem && m == Continuous && curGStart >= DDR0START && curGStart < DDR0RESV) {
      uint8_t *data = new uint8_t[lenA];
      if (isSend) {
        DPRINTF(MapuCSU, "Transmitting from CSU to DDR.\n");
        lp->readTimingBlob(curLStart, data, lenA);
        sendData(cpuid, data);
      } else {
        DPRINTF(MapuCSU, "Transmitting from DDR to CSU.\n");
        getData(cpuid, data);
        lp->writeTimingBlob(curLStart, data, lenA);
      }
      delete [] data;
      goto dma_end;
    }
  }

  switch (m) {
  case Continuous: {
    data = new uint8_t[lenA];
    if (isSend) {
      lp->readTimingBlob(curLStart, data, lenA);
      gp->writeTimingBlob(curGStart, data, lenA);
    } else {
      gp->readTimingBlob(curGStart, data, lenA);
      lp->writeTimingBlob(curLStart, data, lenA);
    }
    delete [] data;
    break;
  }
  case Symmetric: {
    data = new uint8_t[actLXNum];
    while (lenA) {
      if (isLXExt) memset(data, 0, 64);
      if (isSend) {
        lp->readTimingBlob(curLStart, data, dma_conf->DMALocalXNum);
        gp->writeTimingBlob(curGStart, data, actLXNum);
      } else {
        gp->readTimingBlob(curGStart, data, dma_conf->DMAGlobalXNum);
        lp->writeTimingBlob(curLStart, data, actLXNum);
      }
      if (++LYCnt == dma_conf->DMALocalYNum) {
        LYCnt = 0;
        curLYBase += dma_conf->DMALocalZStep;
        curLStart = curLYBase;
      } else {
        curLStart += dma_conf->DMALocalYStep;
      }
      if (++GYCnt == dma_conf->DMAGlobalYNum) {
        GYCnt = 0;
        curGYBase += dma_conf->DMAGlobalZStep;
        curGStart = curGYBase;
      } else {
        curGStart += dma_conf->DMAGlobalYStep;
      }
      lenA -= actLXNum;
    }
    delete [] data;
    break;
  }
  default:{
    data = new uint8_t[1];
    while (lenA) {
      if (isSend) {
        if (LXCnt >= actLXNum)
          *data = 0;
        else
          lp->readTimingBlob(curLStart + LXCnt, data, 1);
        gp->writeTimingBlob(curGStart + GXCnt, data, 1);
      } else {
        if (GXCnt >= actGXNum)
          *data = 0;
        else
          gp->readTimingBlob(curGStart + GXCnt, data, 1);
        lp->writeTimingBlob(curLStart + LXCnt, data, 1);
        //std::cout << std::hex << curLStart + LXCnt << ":" << int(*data) << std::endl;
      }

      if (++LXCnt == actLXNum) {
        LXCnt = 0;
        if (++LYCnt == dma_conf->DMALocalYNum) {
          curLYBase += dma_conf->DMALocalZStep;
          curLStart = curLYBase;
          LYCnt = 0;
        } else
          curLStart += dma_conf->DMALocalYStep;
      }

      if (++GXCnt == actGXNum) {
        GXCnt = 0;
        if (++GYCnt == dma_conf->DMAGlobalYNum) {
          curGYBase += dma_conf->DMAGlobalZStep;
          curGStart = curGYBase;
          GYCnt = 0;
        } else
          curGStart += dma_conf->DMAGlobalYStep;
      }
      lenA--;// lenL > lenG ? actGXNum : actLXNum;
    }
    delete [] data;
    break;
  }
  }

  if ((lenL > lenG && isGYExt) || isLYExt) {
    uint32_t tail = lenL > lenG ? actGYNum * actGXNum - lenA
                                : actLYNum * actLXNum - lenA;
    data = new uint8_t[tail];
    memset(data, 0, tail);
    if (isSend)
      gp->writeTimingBlob(curGStart, data, tail);
    else
      lp->writeTimingBlob(curLStart, data, tail);
    delete [] data;
  }

dma_end:
  queue_head->Next = dma_conf->Next;
  updateDMAStatus(isExternal, dma_conf->DMAGroupNum);
  if (isExternal) {
    extCsuCtx->dma_queue_len--;
    ChangeStatus(cpuid, 0x88, 4-extCsuCtx->dma_queue_len);
    ChangeStatus(cpuid, 0xA0, extDMAStatus);
  } else {
    intCsuCtx->dma_queue_len--;
    if (intCsuCtx->GroupStatusUpdate != 0)
      setByAPE(cpuid, 0x14, intCsuCtx->GroupStatusUpdate);
  }
  delete dma_conf;
}

inline void CsuObject::updateStatus() {
  if (!(pendingEventStatus & intCsuCtx->EventMaskWrite)) {
    if (intCsuCtx->EventStatus) {
      delete intCsuCtx->EventStatus;
      intCsuCtx->EventStatus = NULL;
    }
  } else {
    if (intCsuCtx->EventStatus)
      *intCsuCtx->EventStatus = pendingEventStatus & intCsuCtx->EventMaskWrite;
    else
      intCsuCtx->EventStatus = new uint32_t(pendingEventStatus);
  }
}

void CsuObject::updateDMAStatus(bool isExternal, int gid) {
  assert(gid < 32);
  struct dma_if *p;
  if (isExternal) p = extCsuCtx->dma_queue->Next;
  else p = intCsuCtx->dma_queue->Next;

  while (p) {
    if (p->DMAGroupNum == gid) break;
    p = p->Next;
  }
  if (!p) {
    if (isExternal) extDMAStatus |= 1 << gid;
    else intDMAStatus |= 1 << gid;
  }
}

void CsuObject::ShutDown(int cpuid) {
  extCsuCtx->VPUStatus = 0;
  if (isArm) {
    uint32_t buf[3] = {(uint32_t)cpuid, 8, (uint32_t)extCsuCtx->VPUStatus};
    write(devfd, buf, 4 * 3);
  } else {
    lseek(descriptor, (cpuid << 24) + 8, SEEK_SET);
    while (ioctl(descriptor, MAPU_IOCSTATUS_CHANGE, extCsuCtx->VPUStatus) < 0) {
      if (errno == EBUSY) continue;
      fatal("Changing CSU status failed.\n");
    }
  }
}

void CsuObject::sendEvent(enum CsuObject::CSU_EVENT e) {
  pendingEventStatus |= 1 << e;
  updateStatus();
}

void CsuObject::clearEvent(enum CsuObject::CSU_EVENT e) {
  pendingEventStatus &= ~(1 << e);
  updateStatus();
}
