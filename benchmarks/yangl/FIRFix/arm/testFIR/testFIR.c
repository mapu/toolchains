#include "FIRFix.h"
#include "common.h"
#include "exports.h"


int main()
{

  printf("****************************FIRFixTest****************************************\n");
  
	FIRFix(0, 0);
	printf("launch\n");
	ape_wait(0);
	printf("ape_wait ends\n");
	
	return 0;
}
