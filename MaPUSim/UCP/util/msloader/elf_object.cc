/*
 * ElfObject.cc
 *
 *  Created on: Nov 19, 2013
 *      Author: wangl
 */

#include <iostream>
#include <gelf.h>
#include <ar.h>
#include <elf.h>
#include <libelf.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>

#include "elf_object.hh"
#include "csu_object.hh"

ElfObject *
ElfObject::tryFile(const std::string &fname, int fd) {
  Elf *elf;
  GElf_Ehdr ehdr;
  Arch arch = UnknownArch;
  OpSys opSys = UnknownOpSys;

  if (fd < 0)
    fd = open(fname.c_str(), O_RDONLY);
  if (fd < 0) {
    std::cout << "Elf file is not loadable!\n";
    exit(-1);
  }
  size_t len = (size_t)lseek(fd, 0, SEEK_END);
  uint8_t *fileData =
      (uint8_t *)mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);

  // check that header matches library version
  if (elf_version(EV_CURRENT) == EV_NONE)
    return NULL;

  // get a pointer to elf structure
  elf = elf_memory((char*) fileData, len);
  // will only fail if fd is invalid
  assert(elf != NULL);

  // Check that we actually have a elf file
  if (gelf_getehdr(elf, &ehdr) == 0) {
    elf_end(elf);
    return NULL;
  } else {
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
      GElf_Phdr phdr;
      uint64_t e_phoff = ehdr.e_phoff;
      result->_programHeaderTable = 0;
      for (int hdrnum = 0; hdrnum < result->_programHeaderCount; hdrnum++) {
        gelf_getphdr(elf, hdrnum, &phdr);
        //Check if we've found the segment with the headers in it
        if (phdr.p_offset <= e_phoff
          && phdr.p_offset + phdr.p_filesz > e_phoff) {
          result->_programHeaderTable = phdr.p_paddr + e_phoff;
          break;
        }
      }
    } else
      result->_programHeaderTable = 0;

    elf_end(elf);
    return result;
  }
}

ElfObject::ElfObject(const std::string &_filename,
                     int _fd,
                     size_t _len,
                     uint8_t *_data,
                     Arch _arch,
                     OpSys _opSys)
  : filename(_filename), descriptor(_fd), fileData(_data), len(_len),
    arch(_arch), opSys(_opSys) {
  Elf *elf;
  GElf_Ehdr ehdr;

  Elf64_Addr v2poffset = 0;

  // check that header matches library version
  if (elf_version(EV_CURRENT) == EV_NONE)
    return;

  // get a pointer to elf structure
  elf = elf_memory((char*) fileData, len);
  // will only fail if fd is invalid
  assert(elf != NULL);

  // Check that we actually have a elf file
  if (gelf_getehdr(elf, &ehdr) == 0) {
    return;
  }

  if (ehdr.e_machine == EM_MMPU)
    v2poffset = 0x200000;

  entry = ehdr.e_entry;

  // initialize segment sizes to 0 in case they're not present
  text.size = data.size = bss.size = 0;

  int secIdx = 1;
  Elf_Scn *section;
  GElf_Shdr shdr;

  // The first address of some important sections.
  Addr textSecStart = 0;
  Addr dataSecStart = 0;
  Addr bssSecStart = 0;

  // Get the first section
  section = elf_getscn(elf, secIdx);

  // Find the beginning of the most interesting sections.
  while (section != NULL) {
    gelf_getshdr(section, &shdr);
    char * secName = elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name);

    if (secName) {
      if (!strcmp(".text", secName)) {
        textSecStart = shdr.sh_addr;
      } else if (!strcmp(".data", secName)) {
        dataSecStart = shdr.sh_addr;
      } else if (!strcmp(".bss", secName)) {
        bssSecStart = shdr.sh_addr;
      }
    }

    section = elf_getscn(elf, ++secIdx);
  }

  // Go through all the segments in the program, record them, and scrape
  // out information about the text, data, and bss areas needed by other
  // code.
  for (int i = 0; i < ehdr.e_phnum; ++i) {
    GElf_Phdr phdr;
    if (gelf_getphdr(elf, i, &phdr) == 0) {
      return;
    }

    // for now we don't care about non-loadable segments
    if (!(phdr.p_type & PT_LOAD))
      continue;

    // Check to see if this segment contains the bss section.
    if (phdr.p_paddr <= bssSecStart && phdr.p_paddr + phdr.p_memsz > bssSecStart
        && phdr.p_memsz - phdr.p_filesz > 0) {
      bss.baseAddr = phdr.p_paddr + phdr.p_filesz;
      bss.baseAddr += v2poffset;
      bss.size = phdr.p_memsz - phdr.p_filesz;
      bss.fileImage = NULL;
    }

    // Check to see if this is the text or data segment
    if (phdr.p_vaddr <= textSecStart
      && phdr.p_vaddr + phdr.p_filesz > textSecStart) {
      text.baseAddr = phdr.p_paddr;
      text.baseAddr += v2poffset;
      text.size = phdr.p_filesz;
      text.fileImage = fileData + phdr.p_offset;
    } else if (phdr.p_vaddr <= dataSecStart
      && phdr.p_vaddr + phdr.p_filesz > dataSecStart) {
      data.baseAddr = phdr.p_paddr;
      data.baseAddr += v2poffset;
      data.size = phdr.p_filesz;
      data.fileImage = fileData + phdr.p_offset;
    } else {
      // If it's none of the above but is loadable,
      // load the filesize worth of data
      Section extra;
      extra.baseAddr = phdr.p_paddr;
      extra.baseAddr += v2poffset;
      extra.size = phdr.p_filesz;
      extra.fileImage = fileData + phdr.p_offset;
      extraSegments.push_back(extra);
  }
  }

  // should have found at least one loadable segment
  assert(text.size != 0);

  elf_end(elf);

  // We will actually read the sections when we need to load them
}

bool
ElfObject::loadSections(int csufd, int cpuid)
{
    if (!(loadSection(&text, csufd, cpuid)
            && loadSection(&data, csufd, cpuid)
            && loadSection(&bss, csufd, cpuid)))
      return false;

    std::vector<Section>::iterator extraIt;
    for (extraIt = extraSegments.begin();
            extraIt != extraSegments.end(); extraIt++) {
        if (!loadSection(&(*extraIt), csufd, cpuid)) {
            return false;
        }
    }
    return true;
}

bool
ElfObject::loadSection(Section *sec, int csufd, int cpuid) {
  union csu_mmap csu;
  //std::cout << "Begin to load a sec." << std::endl;
  do {
    lseek(csufd, 0x41000000 + cpuid * 0x400000, SEEK_SET);
    if (read(csufd, csu.mem, sizeof(union csu_mmap)) < 0)
      return false;
  } while (csu.csu_if.dma.DMACommandStatus != 0 ||
           (csu.csu_if.DMAQueryStatus & 0x1) == 0);
  if (sec->size != 0) {
    if (!sec->fileImage) { // bss
      sec->fileImage = new uint8_t[sec->size];
      memset(sec->fileImage, 0, sec->size);
    }
    //std::cout << "Load sec: " << std::hex << sec->baseAddr << " " << sec->size << " " << (long)(sec->fileImage) << std::endl;
    csu.csu_if.dma.DMALocalAddr = sec->baseAddr;
    csu.csu_if.dma.DMALocalXNum = 1;
    csu.csu_if.dma.DMALocalYStep = 1;
    csu.csu_if.dma.DMALocalYNum = sec->size;
    csu.csu_if.dma.DMALocalZStep = sec->size;
    csu.csu_if.dma.DMALocalYAllNum = sec->size;
    csu.csu_if.dma.DMAGlobalAddr = (long)(sec->fileImage);
    csu.csu_if.dma.DMAGlobalXNum = 1;
    csu.csu_if.dma.DMAGlobalYStep = 1;
    csu.csu_if.dma.DMAGlobalYNum = sec->size;
    csu.csu_if.dma.DMAGlobalZStep = sec->size;
    csu.csu_if.dma.DMAGlobalYAllNum = sec->size;
    csu.csu_if.dma.DMAGroupNum = 0; // Group 0 is reserved for sending user space data to APE
    csu.csu_if.dma.DMACmd = 0;
    lseek(csufd, 0x41000000 + cpuid * 0x400000 + ((uint8_t *)(&csu.csu_if.dma) - (uint8_t *)&csu), SEEK_SET);
    int result = write(csufd, &csu.csu_if.dma, sizeof(struct dma_if));
    if (result <= 0){
        std::cout << "Writing to APE failed. ENO:" << errno << "\n";
      return false;
    }
  }
  return true;
}

bool
ElfObject::copySections(uint8_t* start, int csufd, int cpuid) {
  if (text.size != 0) {
    memcpy(start, text.fileImage, text.size);
  }
  return true;
}
