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
  while(in && i < 512) {
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

  i = 512;

  while(in && i < 3584) {
    in >> t;
    buf[i++] = std::stoi(t, 0, 16);
  }
  in.close();
  in.clear();
  std::cout << "SDA1DM0 is loaded, size is " << i - 512 << std::endl;

  struct dma_if conf;

  conf.DMAGlobalAddr = 0x70000000;
  conf.DMAGlobalXNum = 64;
  conf.DMAGlobalYAllNum = 8;
  conf.DMAGlobalYNum = 8;
  conf.DMAGlobalYStep = 64;
  conf.DMAGlobalZStep = 64 * 8;
  conf.DMALocalAddr = 0x400000;
  conf.DMALocalXNum = 64;
  conf.DMALocalYAllNum = 8;
  conf.DMALocalYNum = 8;
  conf.DMALocalYStep = 0x8000;
  conf.DMALocalZStep = 64;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 0;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  conf.DMAGlobalAddr = 0x70000000 + 512;
  conf.DMAGlobalXNum = 64;
  conf.DMAGlobalYAllNum = 48;
  conf.DMAGlobalYNum = 12;
  conf.DMAGlobalYStep = 64;
  conf.DMAGlobalZStep = 64 * 12;
  conf.DMALocalAddr = 0x800000;
  conf.DMALocalXNum = 64;
  conf.DMALocalYAllNum = 48;
  conf.DMALocalYNum = 12;
  conf.DMALocalYStep = 64;
  conf.DMALocalZStep = 64 * 12;
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
  conf.DMAGlobalXNum = 8;
  conf.DMAGlobalYAllNum = 64;
  conf.DMAGlobalYNum = 8;
  conf.DMAGlobalYStep = 8;
  conf.DMAGlobalZStep = 64;
  conf.DMALocalAddr = 0xe00000;
  conf.DMALocalXNum = 8;
  conf.DMALocalYAllNum = 64;
  conf.DMALocalYNum = 8;
  conf.DMALocalYStep = 0x8000;
  conf.DMALocalZStep = 8;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 1;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  unsigned char *result = buf;
  csuobj->Wait();

  dmname = "Epoch1.dat";
  in.open(dmname);

  std::fstream a("../m5out/FFTFloat64Test/FFTFloat64.out", std::ios::out | std::ios::trunc);
  i = 0;
  union {
    uint8_t ub[4];
    float fp;
  } res;
  if (!in) {
    a << "Error: there is no result data to diff! " << std::endl;
    return 0;
  }
  while (in && i < 512) {
	  in >> t;
	  unsigned int code = std::stoul(t, 0, 16);
	  res.ub[i % 4] = code;
	  i++;   
	  if (i % 4 == 0) {
            if ( (res.fp - *(float *)(&result[i - 4])) / res.fp > 1e-5 )
	      a << "diff:" << (res.fp - *(float *)(&result[i - 4])) / res.fp << std::endl;
	  }
  }

  csuobj->Stop();

  csuobj->dfree(buf);

  return 0;
}
