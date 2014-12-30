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
#include <cstdio>
#include <string.h>
#include "../../ext/mapudrv/mapu.h"
#include "../../src/base/types.hh"

class CsuObject {
  union csu_mmap *CsuCtx;
  int descriptor;
  Addr base;
protected:
  CsuObject(const std::string &_filename, int _fd,
            union csu_mmap *_data);

public:
  ~CsuObject() {
    if (descriptor >= 0)
      close(descriptor);

    if (CsuCtx)
      delete CsuCtx;
  }

  static CsuObject *tryFile(const std::string &fname, int fd = -1);

  bool isAPEOn() const {
    return CsuCtx->csu_if.VPUStatus;
  }

  bool isCSUBusy() const {
    return CsuCtx->csu_if.DMAQueueNum;
  }

  bool isCSUFull() const {
    return CsuCtx->csu_if.DMAQueueNum >= 4;
  }

  bool sendData(const void *_data, size_t len, long to);

  bool getData(void *_data, size_t len, long from);

  bool DMARequest(struct dma_if *conf);

  bool update();

  int get_fd() const { return descriptor; }

  void Startup();

  void Stop();

  void Wait();

  void setID(int id);

  void *dmalloc(unsigned int size);

  void dfree(void *p);
};

#endif // __CSU_OBJECT_HH__
