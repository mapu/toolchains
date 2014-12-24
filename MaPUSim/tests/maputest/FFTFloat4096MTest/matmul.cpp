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
  while(in && i < 32768) {
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

  i = 32768;

  while(in && i < 163840) {
    in >> t;
    buf[i++] = std::stoi(t, 0, 16);
  }
  in.close();
  in.clear();
  std::cout << "SDA1DM0 is loaded, size is " << i - 32768 << std::endl;

  struct dma_if conf;

  conf.DMAGlobalAddr = 0x70000000;
  conf.DMAGlobalXNum = 4096;
  conf.DMAGlobalYAllNum = 8;
  conf.DMAGlobalYNum = 8;
  conf.DMAGlobalYStep = 4096;
  conf.DMAGlobalZStep = 4096 * 8;
  conf.DMALocalAddr = 0x400000;
  conf.DMALocalXNum = 4096;
  conf.DMALocalYAllNum = 8;
  conf.DMALocalYNum = 8;
  conf.DMALocalYStep = 0x8000;
  conf.DMALocalZStep = 4096;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 0;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  conf.DMAGlobalAddr = 0x70000000 + 32768;
  conf.DMAGlobalXNum = 64;
  conf.DMAGlobalYAllNum = 2560;
  conf.DMAGlobalYNum = 10;
  conf.DMAGlobalYStep = 64;
  conf.DMAGlobalZStep = 64 * 10;
  conf.DMALocalAddr = 0x800000;
  conf.DMALocalXNum = 64;
  conf.DMALocalYAllNum = 2560;
  conf.DMALocalYNum = 10;
  conf.DMALocalYStep = 64;
  conf.DMALocalZStep = 64 * 10;
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
  conf.DMAGlobalYAllNum = 4096;
  conf.DMAGlobalYNum = 8;
  conf.DMAGlobalYStep = 8;
  conf.DMAGlobalZStep = 64;
  conf.DMALocalAddr = 0xe00000;
  conf.DMALocalXNum = 8;
  conf.DMALocalYAllNum = 4096;
  conf.DMALocalYNum = 8;
  conf.DMALocalYStep = 0x8000;
  conf.DMALocalZStep = 8;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 1;

  while (!csuobj->update() || csuobj->isCSUFull()) ;
  csuobj->DMARequest(&conf);

  unsigned char *result = buf;
  csuobj->Wait();

  dmname = "StdEp3Res.dat";
  in.open(dmname);

  std::fstream a("../m5out/FFTFloat4096MTest/FFTFloat4096M.out", std::ios::out | std::ios::trunc);
  i = 0;
  union {
    uint8_t ub[4];
    float fp;
  } res;
  while (in && i < 32768) {
	  in >> t;
	  unsigned int code = std::stoul(t, 0, 16);
	  res.ub[i % 4] = code;
	  if (code != result[i++])
		   a << i << ": " << code << " : " << (int)(result[i-1]) << std::endl;
	  if (i % 4 == 0) {
            if ( (res.fp - *(float *)(&result[i - 4])) / res.fp > 1e-5 )
              a << "diff:" << (res.fp - *(float *)(&result[i - 4])) / res.fp << std::endl;
	  }
  }

  csuobj->Stop();

  csuobj->dfree(buf);

  return 0;
}
