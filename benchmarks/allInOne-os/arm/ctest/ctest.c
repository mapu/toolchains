#include <stdio.h>
#include <stdlib.h>
#include "uboot_interface/ape.h"

#define min(a,b) (a > b? b : a)

#define task_num 1

unsigned long DataSize = min(256 * 1024 / task_num, 512);

int main() {
  int i;
  printf("try to aquire ape0: %d\n", ape_assert_acquire(0));
  printf("Is APE0 running? %d\n", ape_status(0));

  printf("sizeof(union csu_mmap): 0x%x\n", sizeof(union csu_mmap));
  printf("sizeof(struct dma_if): 0x%x\n" , sizeof(struct dma_if) );
  printf("sizeof(struct csu_if): 0x%x\n" , sizeof(struct csu_if) );

  printf("****testing mem\n" );
  printf("*******allocating mem\n" );
  volatile int *mem = (int*) dma_malloc(DataSize), *mmm = (int*) dma_malloc(DataSize);
  printf("mem: %xl, mmm: %xl\n", (unsigned long) mem, (unsigned long) mmm);

  printf("*******writing mem\n" );
  for (i = 0; i < DataSize / 4; ++i) {
    mem[i] = i;
  }
  printf("*******writing mem done\n" );

  printf("*******checking mem\n" );
  for (int i = 0; i < DataSize / 4; ++i) {
    if (mem[i] != i) printf("mem[%d] = %x\n", i, mem[i] );
  }
  printf("*******checking mem done\n" );
  printf("****testing mem done\n" );

  printf("****checking dma\n");
  printf("*******invoking send dma\n");
  send_vector(0x400000L, mem, DataSize/4, DT_WORD, 0);
  ape_dma_wait(0);
  printf("*******invoking read dma\n");
  fetch_vector(mmm, 0x400000L, DataSize/4, DT_WORD, 0);
  ape_dma_wait(0);
  printf("*******testing fetched data\n");
  for (int i = 0; i < DataSize / 4; ++i)
    if (mmm[i] != mem[i] || mmm[i] != i)
      printf("mem[%x] = %x, mmm[%x] = %x\n", i, mem[i], i, mmm[i] );
  printf("****checking dma done\n" );
  getchar();

  printf("****checking launch\n" );
  int paras[] = { 1, 1024 };

  printf("******preparing data\n" );
  int * buf = (int*) dma_malloc(4096);
  memset(buf, 1, 4096);
  send_vector(0x400000L, buf, 1024, DT_WORD, 0);
  send_vector(0x800000L, buf, 1024, DT_WORD, 0);
  ape_dma_wait(0);
  printf("******preparing data done\n" );

  printf("******launching kernel\n" );
  ape_load_and_start("addVectorInt", 0, paras);
  ape_dma_wait(0);
  ape_start(0);
  ape_wait(0);
  printf("******launching kernel done\n" );

  printf("*******cleanup\n" );
  dma_free(buf);
  ape_release(0);
  dma_free(mem);
  dma_free(mmm);
  printf("*******cleanup done\n" );
  
  return 0;
}
