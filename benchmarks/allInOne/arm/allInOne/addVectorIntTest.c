#include "addVectorInt.h"
#include "common.h"
#include "exports.h"
#include "test_common.h"

#define SDA0DM0_START 0x400000
#define SDA1DM0_START 0x800000
#define SDA2DM0_START 0xc00000

#define DEBUG

int addVectorIntTest() {
	long long i, j, k, repeat_cnt, op;
	long long time;

	const int MaxDataLength = 2048;
	int dataLength;
	int Src0[MaxDataLength], Src1[MaxDataLength];
	volatile int Result[MaxDataLength];

	printf(
			"****************************addVectorIntTest****************************************\n");

	/* initialze the source data */
	for (i = 0; i < MaxDataLength; i++) {
		Src0[i] = i;
		Src1[i] = MaxDataLength - i;
	}
	trace("fill_src\n");
	send_vector(SDA0DM0_START, (const uint32_t) Src0, MaxDataLength, DT_WORD,
			0);
	trace("send_vector\n");
	ape_dma_wait(DATA_E2I_GP, 0); //wait DMA to finish
	trace("ape_dma_wait\n");
	send_vector(SDA1DM0_START, (const uint32_t) Src1, MaxDataLength, DT_WORD,
			0);
	trace("send_vector\n");
	ape_dma_wait(DATA_E2I_GP, 0); //wait DMA to finish
	trace("ape_dma_wait\n");

	*(volatile int*) 0x40401004 = 1024;
	*(volatile int*) 0x40401000 = 1;

	addVectorInt(NULL, 0);
	trace("launch\n");
	ape_wait(0);
	trace("ape_wait\n");

	fetch_vector((uint32_t) Result, SDA2DM0_START, 1024, DT_WORD, 0);
	trace("fetch_vector\n");
	ape_dma_wait(DATA_I2E_GP, 0); //wait DMA to finish
	trace("ape_dma_wait\n");

	printf("Output:\n");
	for (i = 0; i < 1024; i++) {
		printf("%d  ", *(Result + i));
		if (i % 16 == 15)
			printf("\n");
	}

	for (dataLength = 0x8000; dataLength <= 0x100000; dataLength <<= 1) {
		*(volatile int*) 0x40401004 = dataLength;
		op = dataLength;

		for (repeat_cnt = 0x100; repeat_cnt <= 0x4000; repeat_cnt <<= 1) {
			float mops;
			*(volatile int*) 0x40401000 = repeat_cnt;

			time = get_timer(0);

			ape_start(0);
			ape_wait(0);

			time = get_timer(time);

			mops = repeat_cnt * op / 1000.f / time;

			if (time <= 0)
				printf(
						"DataLength: 0x%x, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, N.A. MOPS\n",
						dataLength, 
						repeat_cnt, 
						repeat_cnt * op, 
						time,
						1000 * time / repeat_cnt,
						(int) ((10 * 1000 * time / repeat_cnt) % 10));
			else
				printf(
						"DataLength: 0x%x, 0x%llx executions, 0x%llx operations, total time: %lld ms, average time: %lld.%1d us, %lld.%1d MOPS\n",
						dataLength, 
						repeat_cnt, 
						repeat_cnt * op, 
						time,
						1000 * time / repeat_cnt,
						(int) ((10 * 1000 * time / repeat_cnt) % 10),
						(long long) mops,
						((int) (10 * mops)) % 10);
		}
	}

	return 0;
}
