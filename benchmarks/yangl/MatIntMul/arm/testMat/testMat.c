#include "MatMul.h"
#include "common.h"
#include "exports.h"

#define MAX_LENGTH 256

static	int DM0Data[MAX_LENGTH][MAX_LENGTH];
static	int DM1Data[MAX_LENGTH][MAX_LENGTH];
static	int DM2Data[MAX_LENGTH][MAX_LENGTH];


int main(void)
{  
  int i, j;
  const int wa = MAX_LENGTH, ha = MAX_LENGTH, wb = MAX_LENGTH, hb = MAX_LENGTH;
  const int wc = MAX_LENGTH, hc = MAX_LENGTH;
  int temp;

  printf("****************************MatIntMulTest****************************************\n");

   // initialize A&B Matrix data
   for(i = 0; i < wa; i++) {
     for(j = 0; j < ha; j++) {
       temp = 1;
       DM0Data[i][j] = temp;
     }
   }
   for(i = 0; i < wb; i++) {
     for(j = 0; j < hb; j++) {
       temp = 1;
       DM1Data[i][j] = temp;
     }
   }

  send_matrix(0x400000, (uint32_t)DM0Data, ha, wa, DT_WORD, 0);
  ape_dma_wait(DATA_E2I_GP, 0);
  send_matrix(0x800000, (uint32_t)DM1Data, hb, wb, DT_WORD, 0);
  ape_dma_wait(DATA_E2I_GP, 0);
  
  MatMul(NULL, 0);
  printf("launch\n");
  ape_wait(0);
  printf("ape_wait\n");
	
  fetch_matrix((uint32_t)DM2Data, 0xC00000, hc, wc, DT_WORD, 0);
  ape_dma_wait(DATA_I2E_GP, 0);

	/*printf("result:\n");
	for (i = 0; i < hc; i++) {
		for (j = 0; j < wc; j++)
			printf("%d\t", DM2Data[i][j]);
		printf("\n");
	}*/
	
	return 0;
}
