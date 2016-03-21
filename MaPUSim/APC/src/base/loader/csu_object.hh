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
 */

#ifndef __CSU_OBJECT_HH__
#define __CSU_OBJECT_HH__

#include <set>
#include <vector>

#include "arch/mapu/registers.hh"
#include "base/loader/object_file.hh"
#include "config/the_isa.hh"

#include "mapu.hh"

class CsuObject: public ObjectFile {
  // Recording ARM side context
  struct csu_external_if *extCsuCtx;

  // Recording APE side context
  struct csu_internal_if *intCsuCtx;

  uint32_t pendingEventStatus;
  uint32_t intDMAStatus;
  uint32_t extDMAStatus;

  void updateStatus();

  void updateDMAStatus(bool isExternal, int gid);

  void init();

protected:
  CsuObject(const std::string &_filename,
            int _fd,
            uint8_t *_data,
            Arch _arch,
            OpSys _opSys);

  CsuObject(const std::string &_filename);

public:
  enum CSU_EVENT {
    DMAGroupEvent = 0,
    DMAListEvent,
    InMailEvent,
    OutMailEvent,
    MPUIdelEvent,
    RIO0Event,
    RIO1Event,
    TurboEvent,
  };

  // All CSUs share a unique device descriptor.
  static int devfd;

  static bool isArm;

  struct csu_pkt {
    uint32_t core_id;
    union csu_mmap ape;
  };

  std::vector<csu_pkt *> pkt_buf;

  virtual ~CsuObject() {
    if (extCsuCtx)
      delete extCsuCtx;
    if (intCsuCtx)
      delete intCsuCtx;
  }

  virtual bool loadGlobalSymbols(SymbolTable *symtab, Addr addrMask =
                                   std::numeric_limits<Addr>::max()) {
    return false;
  }
  virtual bool loadLocalSymbols(SymbolTable *symtab, Addr addrMask =
                                  std::numeric_limits<Addr>::max()) {
    return false;
  }

  virtual bool isDynamic() {
    return false;
  }
  virtual bool hasTLS() {
    return false;
  }

  static ObjectFile *tryFile(const std::string &fname, int fd);

  static CsuObject *createSingleSideCSU() {
    return new CsuObject("Single side CSU");
  }

  void close() {
    if (devfd != -1) {
      ObjectFile::close();
      devfd = -1;
    }
  }

  bool isAPEOn() const {
    return extCsuCtx->VPUStatus;
  }

  bool isCSUBusy() const {
    return extCsuCtx->dma_queue_len != 0;
  }

  void sendData(int cpuid, const void *data);

  void getData(int cpuid, void *data);

  void update(int cpuid);

  void ChangeStatus(int cpuid, Addr addr, uint32_t val) const;

  bool readByAPE(int cpuid, Addr chid, TheISA::IntReg &val);

  bool setByAPE(int cpuid, Addr chid, const TheISA::IntReg val);

  uint32_t readCSUNum(Addr chid);

  void processFrontDMA(int cpuid, PortProxy &memProxy,
                       PortProxy &intProxy, bool isExternal = false);

  void ShutDown(int cpuid);

  void init(int cpuid);

  void sendEvent(enum CSU_EVENT e);

  void clearEvent(enum CSU_EVENT e);
};

#endif // __CSU_OBJECT_HH__
