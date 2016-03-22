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

#ifndef __ELF_OBJECT_HH__
#define __ELF_OBJECT_HH__

#include <set>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdint.h>
#include <cstdlib>

#include "../kernel/drivers/apc/apc.h"
#include "types.h"
#include "csu.h"
#include "apc.h"

#define EM_SPARC64  11  /* SPARC 64-bit */
#define EM_MSPU   0x288  /* MaPU Processor SPU program */
#define EM_MMPU   0x460  /* MaPU Processor MPU program */

class ElfObject {
public:
  enum Arch {
    UnknownArch,
    Alpha,
    SPARC64,
    SPARC32,
    Mips,
    X86_64,
    I386,
    Arm,
    Thumb,
    Power,
    Mapu
  };

  enum OpSys {
    UnknownOpSys, Tru64, Linux, Solaris, LinuxArmOABI, MaPU_SPU, MaPU_MPU
  };

protected:

  //The global definition of a "Section" is closest to elf's segments.
  struct Section {
    Addr baseAddr;
    uint8_t *fileImage;
    size_t size;
  };

  Addr entry;

  Section text;
  Section data;
  Section bss;
  Section param;

  //These values are provided to a linux process by the kernel, so we
  //need to keep them around.
  Addr _programHeaderTable;
  uint16_t _programHeaderSize;
  uint16_t _programHeaderCount;
  std::set<std::string> sectionNames;
  const std::string filename;
  int descriptor;
  uint8_t *fileData;
  size_t len;

  Arch  arch;
  OpSys opSys;

public:
  ElfObject(const std::string &_filename,
            int _fd,
            size_t _len,
            uint8_t *_data,
            Arch _arch,
            OpSys _opSys);

  void getSections();
  bool sectionExists(std::string sec);
  std::vector<Section> loadableSegments;

public:
  virtual ~ElfObject() {
    if (fileData){
      munmap(fileData, len);
    }
    if (descriptor >= 0)
      close(descriptor);
  }

  bool loadSections(Csu& csu);
  bool loadSection(Section *sec, Csu& csu);
  bool copySections(uint8_t* start, Csu& csu);

  bool setParameters(void* data);

  const std::string& getFileName() const{
    return filename;
  }

  static ElfObject *tryFile(const std::string &fname, int fd, Apc& apc);

  Addr programHeaderTable() {
    return _programHeaderTable;
  }

  uint16_t programHeaderSize() {
    return _programHeaderSize;
  }

  uint16_t programHeaderCount() {
    return _programHeaderCount;
  }

  unsigned long  getTextStart() const {
    return (unsigned long)text.fileImage;
  }

  unsigned long getTextSize() const {
    return text.size;
  }
};


#endif // __ELF_OBJECT_HH__
