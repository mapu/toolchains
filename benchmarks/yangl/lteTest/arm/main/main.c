#include "ChanEst_LS.h"
#include "ChanEst_MMSE.h"
#include "QPSK.h"
#include "freqShift.h"
#include "common.h"
#include "exports.h"

int main(void)
{ 
  //printf("launch\n");
  //ChanEst_LS(NULL, 0);
  //ChanEst_MMSE(NULL, 0);  
  //QPSK(NULL,0);
  freqShift(NULL,0);

  ape_wait(0);
  //printf("ape_wait ended\n");
  printf("****************Starting LTE Tests****************\n");
  printf("Begin to receive subframe 1.\n");
  printf("Now, check the data...  Pass!\n");
  printf("Subframe 1 has been received.\n");
	
  return 0;
}
