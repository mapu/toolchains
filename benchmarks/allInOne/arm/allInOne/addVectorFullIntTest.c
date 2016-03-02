#include "test_common.h"
#include "addVectorFullInt.h"
#include "common.h"
#include "exports.h"

#define SDA0DM0_START 0x400000
#define SDA1DM0_START 0x800000
#define SDA2DM0_START 0xc00000

static void armTest(int cnt) {
  short count1 = 0;
  int sign = -1;
  int i;
  int temp1;

  int LoopNum = cnt * 64;

  for (i = 0; i < LoopNum; i++) {
    temp1 = (i % 64 + i % 4) * sign;
    sign = -1 * sign;
    count1 += temp1;
  }
  printf("LoopNum:%d\n", LoopNum);
  printf("ARM Output: %d, 0x%x\n", count1, count1);
}

int addVectorFullIntTest(int argc, char * argv[]) {
  long long i, j, k, repeat_cnt, op;
  long long time;
  int dataLength;

  printf("****************************addVectorFullIntTest****************************************\n");

  dataLength = 0x1000;
  *(volatile int*) 0x40401000 = dataLength;
  *(volatile int*) 0x40402000 = 1;

  armTest(dataLength);

  addVectorFullInt(NULL, 0);
  ape_wait(0);
  printf("APE Output: %d, 0x%x\n", *(int *) 0x40403000, *(int *) 0x40403000);

  for (dataLength = 0x100; dataLength <= 0x10000; dataLength *= 2) {
    for (repeat_cnt = 0x100; repeat_cnt <= 0x4000; repeat_cnt *= 2) {
      float mops;

      op = dataLength * 64 * 64 * 5;

      *(volatile int*) 0x40401000 = dataLength;
      *(volatile int*) 0x40402000 = repeat_cnt;

      time = get_timer(0);

      ape_start(0);
      ape_wait(0);

      time = get_timer(time);

      armTest(dataLength);
      printf("APE Output: %d, 0x%x\n", *(int *) 0x40403000, *(int *) 0x40403000);

      mops = repeat_cnt * op / 1000.f / time;

      if (time <= 0) printf("DataLength: 0x%x, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, N.A. MOPS\n", dataLength, repeat_cnt, repeat_cnt * op, time, 1000 * time / repeat_cnt, (int) ((10 * 1000 * time / repeat_cnt) % 10));
      else printf("DataLength: 0x%x, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, %lld.%1d MOPS\n", dataLength, repeat_cnt, repeat_cnt * op, time, 1000 * time / repeat_cnt, (int) ((10 * 1000 * time / repeat_cnt) % 10), (long long) mops, ((int) (10 * mops)) % 10);
    }
  }

  return 0;
}

