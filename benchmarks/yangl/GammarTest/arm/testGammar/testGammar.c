#include "gammar.h"
#include "common.h"
#include "exports.h"

int main(void) {

	printf("****************************Gammar Test****************************************\n");
  
	printf("launch\n");
	gammar(NULL, 0);
	
	ape_wait(0);
	printf("finished\n");
	
	return 0;
}
