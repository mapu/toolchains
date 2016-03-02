#include "matmul.h"
#include "common.h"
#include "exports.h"

#define  DEBUG
#include "test_common.h"

#define MAX_LENGTH 256
#define DMASeg     512

static int *mLength = (volatile int*) 0x40402000;

static float DM0Data[MAX_LENGTH][MAX_LENGTH];
static float DM1Data[MAX_LENGTH][MAX_LENGTH];
static float DM2Data[MAX_LENGTH][MAX_LENGTH];

int matmulTest(void) {

  long long i, j, k, repeat_cnt, op;
  long long time;

  int dataLength, offset;
  char tempA, tempB;

  const int wa = 128, ha = 128, wb = 128, hb = 128;
  const int wc = 128, hc = 128;
  float tmpf;

  printf("****************************matmulTest****************************************\n");

  // initialize A&B Matrix data
  for (i = 0; i < MAX_LENGTH; i++) {
    for (j = 0; j < MAX_LENGTH; j++) {
      tmpf = 1;
      DM0Data[i][j] = tmpf;
    }
  }
  for (i = 0; i < MAX_LENGTH; i++) {
    for (j = 0; j < MAX_LENGTH; j++) {
      tmpf = 1;
      DM1Data[i][j] = tmpf;
    }
  }

  for (offset = 0; offset < MAX_LENGTH * MAX_LENGTH; offset += DMASeg)
  {
    send_vector(0x400000 + offset * 4, ((float*) DM0Data) + offset, DMASeg, DT_WORD, 0);
    trace("send_vector 0x%x 0x%x\n", 0x400000 + offset * 4, ((float*) DM0Data) + offset);
    ape_dma_wait(DATA_E2I_GP, 0); //wait DMA to finish
    trace("ape_dma_wait\n");
    send_vector(0x800000 + offset * 4, ((float*) DM1Data) + offset, DMASeg, DT_WORD, 0);
    trace("send_vector 0x%x 0x%x\n", 0x800000 + offset * 4, ((float*) DM1Data) + offset);
    ape_dma_wait(DATA_E2I_GP, 0); //wait DMA to finish
    trace("ape_dma_wait\n");
  }

  *(volatile int*) 0x40401000 = 1;
  *mLength = 64;

  matmul(0, 0);
  trace("launch\n");
  ape_wait(0);
  trace("ape_wait\n");

  for (offset = 0; offset < *mLength * *mLength; offset += DMASeg)
  {
    fetch_vector((float*) DM2Data + offset, 0xC00000 + offset * 4, DMASeg, DT_WORD, 0);
    trace("fetch_vector 0x%x 0x%x\n", (float*) DM2Data + offset, 0xC00000 + offset * 4);
    ape_dma_wait(DATA_I2E_GP, 0); //wait DMA to finish
    trace("ape_dma_wait\n");
  }

  printf("result:\n");
  for (i = 0; i < *mLength; i++) {
    for (j = 0; j < *mLength; j++)
      printf("%d.%1d\t", (int) DM2Data[i][j], ((int) (10 * DM2Data[i][j])) % 10);
    printf("\n");
  }

  for (*mLength = 64; *mLength <= MAX_LENGTH; *mLength *= 2) {
    for (repeat_cnt = 0x1000; repeat_cnt <= 0x10000; repeat_cnt *= 4) {
      float mops;
      *(volatile int*) 0x40401000 = (int) repeat_cnt;
      op = (*mLength) * (*mLength) * (*mLength * 2 - 1);

      time = get_timer(0);

      ape_start(0);
      ape_wait(0);

      time = get_timer(time);

      mops = ((float) op) * repeat_cnt / time / 1000.f;

      if (time <= 0) printf("length: %d, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, N.A. MOPS\n", *mLength, repeat_cnt, repeat_cnt * op, time, 1000 * time / repeat_cnt, (int) ((10 * 1000 * time / repeat_cnt) % 10));
      else printf("length: %d, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, %lld.%1d MOPS\n", *mLength, repeat_cnt, repeat_cnt * op, time, 1000 * time / repeat_cnt, (int) ((10 * 1000 * time / repeat_cnt) % 10), (long long) mops, ((int) (10 * mops)) % 10);
    }
  }

  return 0;
}
