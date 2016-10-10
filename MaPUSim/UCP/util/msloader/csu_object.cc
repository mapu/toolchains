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
 *          Ali Saidi
 */

#include <string>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "csu_object.hh"

using namespace std;

CsuObject *
CsuObject::tryFile(const string &fname, int fd) {
  if (fd < 0)
    fd = open(fname.c_str(), O_RDWR);
  if (fd < 0) {
    std::cout << "CSU device is inaccessable!\n";
    exit(-1);
  }
  char verify[4] = "";
  ioctl(fd, MAPU_IOCVERIFY, verify);
  union csu_mmap *csu = new union csu_mmap;
  if (!strcmp(verify, "APE")) {
    if (ioctl(fd, MAPU_IOCCHECKPAIR, 0) < 0) {
      std::cout << "Waiting for allocating simulator.\n";
      while (ioctl(fd, MAPU_IOCCHECKPAIR, 0) < 0);
    }
    std::cout << "User program and simulator are paired.\n";
    return new CsuObject(fname, fd, csu);
  }
  std::cout << "Failed to create APE Object.\n";
  return NULL;
}

CsuObject::CsuObject(const string &_filename,  int _fd,
                     union csu_mmap *_data)
  : CsuCtx(_data), descriptor(_fd) {
  ioctl(_fd, MAPU_IOCSTATUS, _data); // wait until later
}

bool CsuObject::sendData(const void *_data, size_t len, long to) {
  do {
    if (!update()) return false;
  } while (CsuCtx->csu_if.dma.DMACommandStatus != 0 ||
           (CsuCtx->csu_if.DMAQueryStatus & 0x1) == 0);
  //std::cout << "Load sec: " << std::hex << to << " " << len << " " << (long)(_data) << std::endl;
  CsuCtx->csu_if.dma.DMALocalAddr = to;
  CsuCtx->csu_if.dma.DMALocalXNum = 1;
  CsuCtx->csu_if.dma.DMALocalYStep = 1;
  CsuCtx->csu_if.dma.DMALocalYNum = len;
  CsuCtx->csu_if.dma.DMALocalZStep = len;
  CsuCtx->csu_if.dma.DMALocalYAllNum = len;
  CsuCtx->csu_if.dma.DMAGlobalAddr = (long)(_data);
  CsuCtx->csu_if.dma.DMAGlobalXNum = 1;
  CsuCtx->csu_if.dma.DMAGlobalYStep = 1;
  CsuCtx->csu_if.dma.DMAGlobalYNum = len;
  CsuCtx->csu_if.dma.DMAGlobalZStep = len;
  CsuCtx->csu_if.dma.DMAGlobalYAllNum = len;
  CsuCtx->csu_if.dma.DMAGroupNum = 0; // Group 0 is reserved for sending user space data to APE
  CsuCtx->csu_if.dma.DMACmd = 0;
  lseek(descriptor, base + ((uint8_t *)(&CsuCtx->csu_if.dma) - (uint8_t *)CsuCtx), SEEK_SET);
  int result = write(descriptor, &CsuCtx->csu_if.dma, sizeof(struct dma_if));
  if (result <= 0){
      std::cout << "Writing to APE failed.\n";
    return false;
  }
  return true;
}

bool CsuObject::getData(void *_data, size_t len, long from) {
  do {
    update();
  } while (CsuCtx->csu_if.dma.DMACommandStatus != 0 ||
           (CsuCtx->csu_if.DMAQueryStatus & 0x2) == 0);
  //std::cout << "Load sec: " << std::hex << from << " " << len << " " << (long)(_data) << std::endl;
  CsuCtx->csu_if.dma.DMALocalAddr = from;
  CsuCtx->csu_if.dma.DMALocalXNum = 1;
  CsuCtx->csu_if.dma.DMALocalYStep = 1;
  CsuCtx->csu_if.dma.DMALocalYNum = len;
  CsuCtx->csu_if.dma.DMALocalZStep = len;
  CsuCtx->csu_if.dma.DMALocalYAllNum = len;
  CsuCtx->csu_if.dma.DMAGlobalAddr = (long)(_data);
  CsuCtx->csu_if.dma.DMAGlobalXNum = 1;
  CsuCtx->csu_if.dma.DMAGlobalYStep = 1;
  CsuCtx->csu_if.dma.DMAGlobalYNum = len;
  CsuCtx->csu_if.dma.DMAGlobalZStep = len;
  CsuCtx->csu_if.dma.DMAGlobalYAllNum = len;
  CsuCtx->csu_if.dma.DMAGroupNum = 1; // Group 1 is reserved for sending user space data to APE
  CsuCtx->csu_if.dma.DMACmd = 1;
  lseek(descriptor, base + ((uint8_t *)(&CsuCtx->csu_if.dma) - (uint8_t *)CsuCtx), SEEK_SET);
  int result = write(descriptor, &CsuCtx->csu_if.dma, sizeof(struct dma_if));
  if (result <= 0){
    std::cout << "Reading from APE failed.\n";
    return false;
  }
  do {
    update();
  } while (CsuCtx->csu_if.dma.DMACommandStatus != 0 ||
           (CsuCtx->csu_if.DMAQueryStatus & 0x2) == 0);
  return true;
}

bool CsuObject::DMARequest(struct dma_if *conf) {
  do {
    update();
  } while (CsuCtx->csu_if.dma.DMACommandStatus != 0);
  CsuCtx->csu_if.dma = *conf;
  lseek(descriptor, base + ((uint8_t *)(&CsuCtx->csu_if.dma) - (uint8_t *)CsuCtx), SEEK_SET);
  int result = write(descriptor, &CsuCtx->csu_if.dma, sizeof(struct dma_if));
  if (result <= 0){
    std::cout << "APE DMA Request failed.\n";
    return false;
  }
  do {
    update();
  } while (CsuCtx->csu_if.dma.DMACommandStatus != 0);
  return true;
}

bool CsuObject::update() {
  lseek(descriptor, base, SEEK_SET);
  int result = read(descriptor, CsuCtx->mem, sizeof(union csu_mmap));
  if (result <= 0) {
    std::cout << "Check status failed.\n";
    return false;
  }
  return true;
}

void CsuObject::Startup() {
  lseek(descriptor, base, SEEK_SET);
  int cmd[1] = {0};
  write(descriptor, cmd, 4);
}

void CsuObject::Stop() {
  lseek(descriptor, base, SEEK_SET);
  int cmd[1] = {CMD_SHUTDOWN};
  write(descriptor, cmd, 4);
}

void CsuObject::Wait() {
  do {
    if (!update()) continue;
  } while (CsuCtx->csu_if.dma.DMACommandStatus != 0 ||
           (CsuCtx->csu_if.DMAQueryStatus != 0xFFFFFFFF));
}

void CsuObject::setID(int id) {
  base = 0x41000000 + id * 0x400000;
}

void *CsuObject::dmalloc(unsigned int size) {
  return mmap((void *)(0x70000000), size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
}

void CsuObject::dfree(void *p) {
  munmap(p, 4096);
}
