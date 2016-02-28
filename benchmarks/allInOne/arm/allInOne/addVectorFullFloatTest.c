#include "test_common.h"
#include "addVectorFullFloat.h"
#include "common.h"
#include "exports.h"

#define SDA0DM0_START 0x400000
#define SDA1DM0_START 0x800000
#define SDA2DM0_START 0xc00000

static void armTest(float a, float b, int cnt) {
	float acc = 0;
	int i;
	float temp = a - b;
	for (i = 0; i < cnt; i++) {
		acc += temp * temp;
		temp -= b;
	}
	printf("ARM Output: 0x%x\n", *(int *) &acc);
}

int addVectorFullFloatTest() {
	long long i, j, k, repeat_cnt, op;
	long long time;
	int dataLength;

	volatile float *a, *b;
	a = (volatile float *) 0x40401000;
	b = (volatile float *) 0x40402000;
	*a = 1.5;
	*b = 0.1;

	printf(
			"****************************addVectorFullFloatTest****************************************\n");

	dataLength = 1000000;
	*(volatile int*) 0x40403000 = dataLength;
	*(volatile int*) 0x40404000 = 1;

	armTest(*a, *b, repeat_cnt);

	addVectorFullFloat(NULL, 0);
	ape_wait(0);
	printf("APE Output: 0x%x\n", *(int *) 0x40403000);

	for (dataLength = 0x100; dataLength <= 0x10000; dataLength *= 2) {
		for (repeat_cnt = 0x100; repeat_cnt <= 0x40000; repeat_cnt *= 2) {
			float mops;

			op = 3 * 16 * 8 * dataLength;
			*(volatile int*) 0x40403000 = dataLength;
			*(volatile int*) 0x40404000 = repeat_cnt;

			time = get_timer(0);

			ape_start(0);
			ape_wait(0);

			time = get_timer(time);

			armTest(*a, *b, dataLength);
			printf("APE Output: 0x%x\n", *(int *) 0x40403000);

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

