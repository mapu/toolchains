#include "matmul.h"
#include "common.h"
#include "exports.h"

#include "test_common.h"

#define MAX_LENGTH 64

static	float DM0Data[MAX_LENGTH][MAX_LENGTH];
static	float DM1Data[MAX_LENGTH][MAX_LENGTH];
static	float DM2Data[MAX_LENGTH][MAX_LENGTH];


int matmulTest(void) {
  
	long long i, j, k, repeat_cnt, op;
	long long time;

	int dataLength, offset;
	char tempA, tempB;

	const int wa = MAX_LENGTH, ha = MAX_LENGTH, wb = MAX_LENGTH, hb = MAX_LENGTH;
	const int wc = MAX_LENGTH, hc = MAX_LENGTH;
	float tmpf;

	printf("****************************matmulTest****************************************\n");

   // initialize A&B Matrix data
   for(i=0; i<wa; i++) {
     for(j=0; j<ha; j++) {
       tmpf          = 1;
       DM0Data[i][j] = tmpf;
     }
   }
   for(i=0; i<wb; i++) {
     for(j=0; j<hb; j++) {
       tmpf          = 1;
       DM1Data[i][j] = tmpf;
     }
   }
  //matrix c
  for(i=0; i<wb; i++) {
     for(j=0; j<hb; j++) {
       tmpf          = 1;
       DM2Data[i][j] = tmpf;
     }
   }


  send_matrix(0x400000, (uint32_t)DM0Data, ha, wa, DT_WORD, 0);
  ape_dma_wait(DATA_E2I_GP, 0);
  send_matrix(0x800000, (uint32_t)DM1Data, hb, wb, DT_WORD, 0);
  ape_dma_wait(DATA_E2I_GP, 0);

  send_matrix(0xc00000, (uint32_t)DM2Data, hc, wc, DT_WORD, 0);
  ape_dma_wait(DATA_E2I_GP, 0);
  
	matmul(0, 0);
	trace("launch\n");
	ape_wait(0);
	trace("ape_wait\n");
	
  fetch_matrix((uint32_t)DM2Data, 0xC00000, hc, wc, DT_WORD, 0);
  ape_dma_wait(DATA_I2E_GP, 0);

	printf("result:\n");
	for (i = 0; i < hc; i++) {
		for (j = 0; j < wc; j++)
			printf("%d.%1d\t", (int) DM2Data[i][j], ((int) (10 * DM2Data[i][j]))%10);
		printf("\n");
	}
	
	return 0;
}
