#include "fullBandWidth.h"
#include "common.h"
#include "exports.h"
#define SDA0DM0_START 0x400000
#define SDA1DM0_START 0x800000
#define SDA2DM0_START 0xc00000
#define DEBUG
#include "test_common.h"

#define MaxDataLength 1024*256 
#define DMASeg        512

char a[DMASeg], b[DMASeg];
char Result[DMASeg];
unsigned volatile char acc[MaxDataLength / 64];

static void printResult(int dataLength) {
  int i, offset;
  printf("**** dataLength:%d, APE Output:\n", dataLength);
  for (offset = 0; offset < dataLength; offset += DMASeg)
  {
    fetch_vector((uint32_t)(Result), SDA2DM0_START + offset, DMASeg, DT_BYTE, 0);
    trace("fetch_vector 0x%x 0x%x\n", (uint32_t)(Result),
    SDA2DM0_START + offset);
    ape_dma_wait(DATA_I2E_GP, 0); //wait DMA to finish
    trace("ape_dma_wait\n");

    for (i = 0; i < DMASeg; i++) {
      printf("%d  ", *(Result + i));
      if (i % 16 == 15) printf("\n");
    }
  }
}

int fullBandWidthTest() {
  long long i, j, k, repeat_cnt, op;
  long long time;

  int dataLength, offset;
  char tempA, tempB;

  printf("****************************fullBandWidthTest****************************************\n");

  for (i = 0; i < DMASeg; i++) {
    a[i] = 1;
    b[i] = 32;
  }

  for (offset = 0; offset < MaxDataLength; offset += DMASeg)
  {
    send_vector(SDA0DM0_START + offset, (uint32_t) a, DMASeg, DT_BYTE, 0);
    trace("send_vector 0x%x 0x%x\n", SDA0DM0_START + offset, (uint32_t) a);
    ape_dma_wait(DATA_E2I_GP, 0); //wait DMA to finish
    trace("ape_dma_wait\n");
    send_vector(SDA1DM0_START + offset, (uint32_t) b, DMASeg, DT_BYTE, 0);
    trace("send_vector 0x%x 0x%x\n", SDA1DM0_START + offset, (uint32_t) b);
    ape_dma_wait(DATA_E2I_GP, 0); //wait DMA to finish
    trace("ape_dma_wait\n");
  }

  *(volatile int*) 0x40401000 = MaxDataLength;
  *(volatile int*) 0x40401004 = 1;
  fullBandWidth(NULL, 0);
  trace("launch\n");
  ape_wait(0);
  trace("ape_wait\n");

  //printResult(MaxDataLength);

  for (dataLength = 0x400; dataLength <= MaxDataLength; dataLength <<= 1) {
    *(volatile int*) 0x40401000 = dataLength;
    op = dataLength * 2 * 3;

    for (repeat_cnt = 0x10000; repeat_cnt <= 0x100000; repeat_cnt *= 4) {
      float mops;
      *(volatile int*) 0x40401004 = (int) repeat_cnt;

      time = get_timer(0);

      ape_start(0);
      ape_wait(0);

      time = get_timer(time);

      //printResult(dataLength);

      mops = repeat_cnt * op / 1000.f / time;

      if (time <= 0) printf("DataLength: 0x%x, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, N.A. MOPS, bandwidth: N.A.\n", dataLength, repeat_cnt, repeat_cnt * op, time, 1000 * time / repeat_cnt, (int) ((10 * 1000 * time / repeat_cnt) % 10));
      else printf("DataLength: 0x%x, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, %lld.%1d MOPS, bandwidth: %lld KB/s\n", dataLength, repeat_cnt, repeat_cnt * op, time, 1000 * time / repeat_cnt, (int) ((10 * 1000 * time / repeat_cnt) % 10), (long long) mops, ((int) (10 * mops)) % 10, 3 * dataLength * repeat_cnt / time);
    }
  }

  return 0;
}

