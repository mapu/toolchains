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

#ifndef __CSU_H__
#define __CSU_H__

#include <set>
#include <vector>
#include <string.h>
#include <unistd.h>

#include "apc.h"
#include "types.h"

class ElfObject;

class Csu {
  Addr base;
  int fd;
  Apc* apc;

public:
  inline Csu(int _fd, int id, Apc* _apc): fd(_fd), apc(_apc) {
    setID(id);
  }

  ~Csu() {}

  bool isAPEOn() const;

  bool isCSUBusy() const;

  bool isCSUFull() const;

  bool sendData(const void *_data, size_t len, long to);

  bool getData(void *_data, size_t len, long from);

  bool asynSendData(const void *_data, size_t len, long to);

  bool asynGetData(void *_data, size_t len, long from);

  bool waitDMA() const;

  bool launchAPE(ElfObject* spuImg, char* localDbPath, void* params);

  bool waitLaunch() const;

  bool requestDMA(const dma_if& dma);

  bool readCsuIf(csu_if& csu_if);

  inline Addr getBase() const {return base;}

  void setID(int id);
  int getID();

  inline void *cmalloc(unsigned int size) {return apc->cmalloc(size);}
  inline void *pmalloc(unsigned int size, int region) {return apc->pmalloc(size, region);}

  inline void cfree(void *p) {apc->cfree(p);}
  inline void pfree(void *p, int region) {apc->pfree(p, region);}

  bool writeDbEntries(unsigned long * args);

  bool setAPEOn();
  bool setAPEOff();
};
void* vir2phy(void* vir);

#endif // __CSU_H__
