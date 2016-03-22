/*
 * ElfObject.cc
 *
 *  Created on: Nov 19, 2013
 *      Author: wangl
 */
#include "elf_object.h"

#include <iostream>
#include <ar.h>
#include "libelf/include/libelf/libelf.h"
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
using namespace std;

ElfObject *
ElfObject::tryFile(const std::string &fname, int fd, Apc& apc) {
  Elf *elf;
  Elf32_Ehdr *p_ehdr, ehdr;
  Arch arch = UnknownArch;
  OpSys opSys = UnknownOpSys;

  if (fd < 0) fd = open(fname.c_str(), O_RDONLY);
  if (fd < 0) {
    std::cout << "Elf file is not loadable!\n";
    exit(-1);
  }
  size_t len = (size_t) lseek(fd, 0, SEEK_END);
  uint8_t *fileData = (uint8_t *) mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
  uint8_t *tmp = (uint8_t *) apc.cmalloc(len);

  memcpy(tmp, fileData, len);
  munmap(fileData, len);
  fileData = tmp;

  // check that header matches library version
  if (elf_version(EV_CURRENT) == EV_NONE) return NULL;

  // get a pointer to elf structure
  elf = elf_memory((char*) fileData, len);
  // will only fail if fd is invalid
  assert(elf != NULL);

  // Check that we actually have a elf file
  if ((p_ehdr = elf32_getehdr(elf)) == 0) {
    elf_end(elf);
    return NULL;
  } else {
    ehdr = *p_ehdr;
    //Detect the architecture
    //Since we don't know how to check for alpha right now, we'll
    //just assume if it wasn't something else and it's 64 bit, that's
    //what it must be.
    if (ehdr.e_machine == EM_MSPU) {
      arch = ElfObject::Mapu;
    } else if (ehdr.e_machine == EM_MMPU) {
      arch = ElfObject::Mapu;
    } else {
      arch = ElfObject::UnknownArch;
    }

    //Detect the operating system
    if (ehdr.e_machine == EM_MSPU) {
      opSys = ElfObject::MaPU_SPU;
    } else if (ehdr.e_machine == EM_MMPU) {
      opSys = ElfObject::MaPU_MPU;
    } else {
      opSys = ElfObject::UnknownOpSys;
    }

    ElfObject * result = new ElfObject(fname, fd, len, fileData, arch, opSys);

    //The number of headers in the file
    result->_programHeaderCount = ehdr.e_phnum;
    //Record the size of each entry
    result->_programHeaderSize = ehdr.e_phentsize;
    if (result->_programHeaderCount) //If there is a program header table
    {
      //Figure out the virtual address of the header table in the
      //final memory image. We use the program headers themselves
      //to translate from a file offset to the address in the image.
      Elf32_Phdr phdr;
      uint64_t e_phoff = ehdr.e_phoff;
      result->_programHeaderTable = 0;
      for (int hdrnum = 0; hdrnum < result->_programHeaderCount; hdrnum++) {
        phdr = elf32_getphdr(elf)[hdrnum];
        //Check if we've found the segment with the headers in it
        if (phdr.p_offset <= e_phoff && phdr.p_offset + phdr.p_filesz > e_phoff) {
          result->_programHeaderTable = phdr.p_paddr + e_phoff;
          break;
        }
      }
    } else result->_programHeaderTable = 0;

    elf_end(elf);
    return result;
  }
}

ElfObject::ElfObject(const std::string &_filename, int _fd, size_t _len, uint8_t *_data, Arch _arch, OpSys _opSys)
    : filename(_filename), descriptor(_fd), fileData(_data), len(_len), arch(_arch), opSys(_opSys) {
  Elf *elf;
  Elf32_Ehdr *p_ehdr, ehdr;

  Elf32_Addr v2poffset = 0;

  // check that header matches library version
  if (elf_version(EV_CURRENT) == EV_NONE) return;

  // get a pointer to elf structure
  elf = elf_memory((char*) fileData, len);
  // will only fail if fd is invalid
  assert(elf != NULL);

  // Check that we actually have a elf file
  if ((p_ehdr = elf32_getehdr(elf)) == 0) return;

  ehdr = *p_ehdr;

  if (ehdr.e_machine == EM_MMPU) v2poffset = 0x200000;

  entry = ehdr.e_entry;

  // initialize segment sizes to 0 in case they're not present
  text.size = data.size = bss.size = 0;

  int secIdx = 0;
  Elf32_Shdr *p_shdr, shdr;

  // Find the beginning of the most interesting sections.
  for (Elf_Scn *section = elf_getscn(elf, ++secIdx); section; section = elf_getscn(elf, ++secIdx)) {
    p_shdr = elf32_getshdr(section);
    shdr = *p_shdr;
    char * secName = elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name);

    if (secName) {
      if (!strcmp(".text", secName)) {
        text.baseAddr = shdr.sh_addr;
        text.fileImage = fileData + shdr.sh_offset;
        text.size = shdr.sh_size;
      } else if (!strcmp(".data", secName)) {
        data.baseAddr = shdr.sh_addr;
        data.fileImage = fileData + shdr.sh_offset;
        data.size = shdr.sh_size;
      } else if (!strcmp(".bss", secName)) {
        bss.baseAddr = shdr.sh_addr;
        bss.fileImage = 0;
      } else if (!strcmp(".param", secName)) {
        param.baseAddr = shdr.sh_addr;
        param.fileImage = fileData + shdr.sh_offset;
        param.size = shdr.sh_size;
      }
    }
  }

  // Go through all the segments in the program, record them, and scrape
  // out information about the text, param, data, and bss areas needed by other code.
  for (int i = 0; i < ehdr.e_phnum; ++i) {
    Elf32_Phdr *p_phdr, phdr;
    p_phdr = elf32_getphdr(elf);
    if (p_phdr == 0) {
      return;
    }

    phdr = p_phdr[i];

    // for now we don't care about non-loadable segments
    if (!(phdr.p_type & PT_LOAD)) continue;

    // Check to see if this segment contains the bss section.
    if (phdr.p_paddr <= bss.baseAddr && phdr.p_paddr + phdr.p_memsz > bss.baseAddr && phdr.p_memsz > phdr.p_filesz) {
      bss.baseAddr = phdr.p_paddr + phdr.p_filesz;
      bss.baseAddr += v2poffset;
      bss.size = phdr.p_memsz - phdr.p_filesz;
      bss.fileImage = NULL;
      loadableSegments.push_back(bss);
    } else {
      // load the filesize worth of data
      Section extra;
      extra.baseAddr = phdr.p_paddr;
      extra.baseAddr += v2poffset;
      extra.size = phdr.p_filesz;
      extra.fileImage = fileData + phdr.p_offset;
      loadableSegments.push_back(extra);
    }
  }

  // should have found at least one loadable segment
  assert(text.size != 0);

  elf_end(elf);

  // We will actually read the sections when we need to load them
}

bool ElfObject::setParameters(void* data) {
  memcpy(param.fileImage, data, param.size);
  return true;
}

bool ElfObject::loadSections(Csu& csu) {
  std::vector<Section>::iterator extraIt;
  for (extraIt = loadableSegments.begin(); extraIt != loadableSegments.end(); extraIt++)
    if (!loadSection(&(*extraIt), csu)) return false;

  return true;
}

bool ElfObject::loadSection(Section *sec, Csu& csu) {
  if (sec->size != 0) {
    if (!sec->fileImage) { // bss
      sec->fileImage = (uint8_t *) csu.cmalloc(sec->size);
      memset(sec->fileImage, 0, sec->size);
    }

    return csu.sendData(sec->fileImage, sec->size, sec->baseAddr);
  }
  return true;
}

bool ElfObject::copySections(uint8_t* start, Csu& csu) {
  if (text.size != 0) memcpy(start, text.fileImage, text.size);

  return true;
}
