//============================================================================
// Name        : driver_test.cpp
// Author      : Lei Wang
// Version     :
// Copyright   :
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "elf_object.hh"
#include "csu_object.hh"

int main(void) {

  std::string spuname = "../FIRFixTest/app.s.out";

  ElfObject *spuobj = ElfObject::tryFile(spuname);

  std::string mpuname = "../FIRFixTest/app.m.out";

  ElfObject *mpuobj = ElfObject::tryFile(mpuname);
  
  std::string apename = "/dev/ape";
  
  CsuObject *csuobj = CsuObject::tryFile(apename);

  csuobj->setID(0);

  spuobj->loadSections(csuobj->get_fd(), 0);

  mpuobj->loadSections(csuobj->get_fd(), 0);

  int32_t *result = new int32_t[1024];

  /*csuobj->Wait();

  csuobj->Startup();
  do {
    csuobj->update();
  } while (csuobj->isAPEOn());

*/

  spuname = "../TLB256Test/app.s.out";

  spuobj = ElfObject::tryFile(spuname);

  spuobj->loadSections(csuobj->get_fd(), 1);

  mpuname = "../TLB256Test/app.m.out";

  mpuobj = ElfObject::tryFile(mpuname);

  mpuobj->loadSections(csuobj->get_fd(), 1);


  csuobj->Startup();

  csuobj->setID(1);
  csuobj->Startup();
  csuobj->setID(0);
  do {
    csuobj->update();
  } while (csuobj->isAPEOn());
  csuobj->setID(1);
  do {
    csuobj->update();
  } while (csuobj->isAPEOn());

  std::fstream a("../m5out/ms_test/ms.out", std::ios::out | std::ios::trunc);
  csuobj->setID(0);
  if (csuobj->getData(result, 4096, 0xC00000)) {
    for (unsigned i = 0; i < 1024; i++) {
      a << ".int 0x" << std::hex << result[i] << "\n";
    }
  }
  csuobj->setID(1);
  if (csuobj->getData(result, 4096, 0xC00000)) {
    for (unsigned i = 0; i < 1024; i++) {
      a << ".int 0x" << std::hex << result[i] << "\n";
    }
  }

  csuobj->setID(0);
  csuobj->Stop();
  csuobj->setID(1);
  csuobj->Stop();

  return EXIT_SUCCESS;
}
