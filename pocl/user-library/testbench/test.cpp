#include <string>
#include <cstdlib>
#include <iostream>
extern "C" {
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
}
#include "csu.h"
#include "apc.h"
#include "elf_object.h"

using namespace std;
#define task_num 10

unsigned long DataSize = min(256 * 1024 / task_num, 512);

void *thread(void *threadid) {
  int tid;
  tid = *(int*) threadid;
  unsigned long dataoffset = DataSize * tid;

  cout << "tid: " << tid << endl;
  cout << "DataSize: " << DataSize << endl;
  cout << "dataoffset: " << dataoffset << endl;

  Apc *apc = Apc::tryFile("/dev/apc", -1);

  while (!apc->acquireApe(0)) ;

  Csu *csu = apc->getCsu(0);

  int* mem = (int*) apc->cmalloc(DataSize), *mmm = (int*) apc->cmalloc(DataSize);
  for (int i = 0; i < DataSize / 4; ++i)
    mem[i] = (tid << 24) | i;

  cout << "begin send: " << tid << endl;
  csu->sendData(mem, DataSize, 0x400000L + dataoffset);
  cout << "end send: " << tid << endl;

  for (int j = 0; j < 10; ++j)
    if (j % 2) csu->sendData(mem, DataSize, 0x400000L + dataoffset);
    else csu->getData(mmm, DataSize, 0x400000L + dataoffset);

  cout << "begin cfree(mem): " << tid << endl;
  apc->cfree(mem);
  cout << "begin cfree(mmm): " << tid << endl;
  apc->cfree(mmm);

  cout << "begin releaseApe: " << tid << endl;
  apc->releaseApe(0);

  cout << "begin delete: " << tid << endl;
  delete apc;

  cout << "begin pthread_exit: " << tid << endl;
  pthread_exit(NULL);
}

int main() {
  int i;

  Apc *apc = Apc::tryFile("/dev/apc", -1);
  Csu *csu = apc->getCsu(0);
  cout << "acquireApe: " << apc->acquireApe(0) << endl;
  cout << "possessApe: " << apc->possessApe(0) << endl;

  cout << "isAPEOn: " << csu->isAPEOn() << endl;

  cout << "csu->base: 0x" << hex << csu->getBase() << endl;
  cout << "sizeof(union csu_mmap): 0x" << hex << sizeof(union csu_mmap) << endl;
  cout << "sizeof(struct dma_if): 0x" << hex << sizeof(struct dma_if) << endl;
  cout << "sizeof(struct csu_if): 0x" << hex << sizeof(struct csu_if) << endl;

  cout << "****testing mem" << endl;
  cout << "*******allocating mem" << endl;
  volatile int *mem = (int*) apc->cmalloc(DataSize), *mmm = (int*) apc->cmalloc(DataSize);
  cout << (unsigned long) mem << ' ' << (unsigned long) mmm << ' ' << endl;

  cout << "*******writing mem" << endl;
  for (i = 0; i < DataSize / 4; ++i) {
    mem[i] = i;
  }
  cout << "*******writing mem done" << endl;

  cout << "*******checking mem" << endl;
  for (int i = 0; i < DataSize / 4; ++i) {
    if (mem[i] != i) cout << "mem[" << i << "] = " << mem[i] << endl;
  }
  cout << "*******checking mem done" << endl;
  cout << "****testing mem done" << endl;

  cout << "****checking dma"<< endl;
  cout << "*******invoking send dma"<< endl;
  csu->sendData(mem, DataSize, 0x400000L);
  cout << "*******invoking read dma"<< endl;
  csu->getData(mmm, DataSize, 0x400000L);
  cout << "*******testing fetched data"<< endl;
  for (int i = 0; i < DataSize / 4; ++i)
    if (mmm[i] != i) cout << "mem[" <<i << "] = " << mem[i] << ", mmm[" << i << "] = " << mmm[i] << endl;
  cout << "****checking dma done" << endl;
  getchar();

  cout << "****testing multi-task dma" << endl;
  apc->releaseApe(0);
  pthread_t id[task_num];
  int ids[task_num];
  int retv;

  for (int t = 0; t < task_num; ++t) {
	ids[t] = t;
    retv = pthread_create(id + t, NULL, thread, ids+t);
    if (retv != 0) {
      printf("Create pthread error!/n");
      break;
    }
  }

  for (int t = 0; t < task_num; ++t)
    pthread_join(id[t], NULL);

  cout << "multi-task done!" << endl;
  getchar();

  cout << "******checking data of thread " << endl;
  if (!apc->acquireApe(0)) cerr << "cannot acquire" << endl;

  for (int t = 0; t < task_num; ++t) {
    csu->getData(mmm, DataSize, 0x400000L + DataSize * t);

    for (int i = 0; i < DataSize / 4; ++i)
      if (mmm[i] != (t << 24) + i) cout << "tid: " << t << ", mmm[" << i << "] = " << mmm[i] << endl;
  }
  cout << "******checking data of thread done" << endl;
  getchar();

  cout << "****checking db" << endl;
  apc->parseGlobalDB("fs_root/apclib_custom.db");
  cout << "****checking db done" << endl;

  cout << "****checking launch" << endl;

  cout << "******loading elf" << endl;
  int paras[] = { 1, 1024 };
  ElfObject *elf = ElfObject::tryFile("fs_root/slib/addVectorInt.s.elf", -1, *apc);
  cout << "******loading elf done" << endl;

  cout << "******preparing data" << endl;
  int * buf = (int*) csu->cmalloc(4096);
  memset(buf, 1, 4096);
  csu->sendData(buf, 4096, 0x400000);
  csu->sendData(buf, 4096, 0x800000);
  cout << "******preparing data done" << endl;

  cout << "******launching kernel" << endl;
  csu->launchAPE(elf, 0, paras);
  cout << "******launching kernel done" << endl;

  cout << "*******cleanup" << endl;
  delete elf;
  csu->cfree(buf);
  apc->releaseApe(0);
  apc->cfree(mem);
  apc->cfree(mmm);
  delete apc;
  cout << "*******cleanup done" << endl;

  return 0;
}
