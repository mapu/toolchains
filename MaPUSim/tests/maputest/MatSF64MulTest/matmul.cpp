//
//  main.c
//  Bicubic
//
//  Created by Lei Wang on 13-4-27.
//  Copyright (c) 2013年 Lei Wang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include "csu_object.hh"
#include "elf_object.hh"

int main() {
  std::string apename = "/dev/ape";
  CsuObject *csuobj = CsuObject::tryFile(apename);

  csuobj->setID(0);

  std::string spuname = "app.s.out";

  ElfObject *spuobj = ElfObject::tryFile(spuname);

  spuobj->loadSections(csuobj->get_fd(), 0);

  std::string mpuname = "app.m.out";

  ElfObject *mpuobj = ElfObject::tryFile(mpuname);

  mpuobj->loadSections(csuobj->get_fd(), 0);

  uint8_t *buf = (uint8_t *)csuobj->dmalloc(1024*1024);

  std::string dmname = "SDA0DM0.dat";

  std::ifstream in(dmname.c_str());

  if (!in)
    std::cout << "SDA0DM0 is not found.\n";

  int i = 0;
  std::string t;
  while(in && i < 0x40000) {
    in >> t;
    buf[i++] = std::stoi(t, 0, 16);
  }
  in.close();
  in.clear();
  std::cout << "SDA0DM0 is loaded, size is " << i << std::endl;

  dmname = "SDA1DM0.dat";

  in.open(dmname.c_str());

  if (!in)
    std::cout << "SDA1DM0 is not found.\n";

  while(in && i < 0x80000) {
    in >> t;
    buf[i++] = std::stoi(t, 0, 16);
  }
  in.close();
  in.clear();
  std::cout << "SDA1DM0 is loaded, size is " << i << std::endl;

  struct dma_if conf;

  conf.DMAGlobalAddr = 0x70000000;
  conf.DMAGlobalXNum = 1024;
  conf.DMAGlobalYAllNum = 256;
  conf.DMAGlobalYNum = 256;
  conf.DMAGlobalYStep = 1024;
  conf.DMAGlobalZStep = 256 * 1024;
  conf.DMALocalAddr = 0x400000;
  conf.DMALocalXNum = 1024;
  conf.DMALocalYAllNum = 256;
  conf.DMALocalYNum = 256;
  conf.DMALocalYStep = 1024;
  conf.DMALocalZStep = 256 * 1024;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 0;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  conf.DMAGlobalAddr = 0x70000000 + 256 * 1024;
  conf.DMAGlobalXNum = 1024;
  conf.DMAGlobalYAllNum = 256;
  conf.DMAGlobalYNum = 256;
  conf.DMAGlobalYStep = 1024;
  conf.DMAGlobalZStep = 256 * 1024;
  conf.DMALocalAddr = 0x800000;
  conf.DMALocalXNum = 1024;
  conf.DMALocalYAllNum = 256;
  conf.DMALocalYNum = 256;
  conf.DMALocalYStep = 1024;
  conf.DMALocalZStep = 256 * 1024;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 0;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  csuobj->Wait();

  csuobj->Startup();
  do {
    csuobj->update();
  } while (csuobj->isAPEOn());

  conf.DMAGlobalAddr = 0x70000000;
  conf.DMAGlobalXNum = 256;
  conf.DMAGlobalYAllNum = 64;
  conf.DMAGlobalYNum = 16;
  conf.DMAGlobalYStep = 256;
  conf.DMAGlobalZStep = 256 * 16;
  conf.DMALocalAddr = 0xc00000;
  conf.DMALocalXNum = 256;
  conf.DMALocalYAllNum = 64;
  conf.DMALocalYNum = 16;
  conf.DMALocalYStep = 4096 * 4;
  conf.DMALocalZStep = 256;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 1;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  unsigned int *result = (unsigned int *)buf;
  csuobj->Wait();

  dmname = "StdRes.dat";
  in.open(dmname);

  std::fstream a("../m5out/MatSF64MulTest/MatSF64Mul.out", std::ios::out | std::ios::trunc);

  if (!in) {
    a << "Error: there is no result data to diff! " << std::endl;
    return 0;
  }
  i = 0;
  while (in && i < 256 * 16) {
	  in >> t;
	  unsigned int code = std::stoul(t, 0, 16);
	  if (code != result[i++])
		  std::cout << i << code << " : " << result[i-1] << std::endl;
  }

  csuobj->Stop();

  csuobj->dfree(buf);

  return 0;
}
