#include "test_common.h"

int addVectorIntTest();
int addVectorFullFloatTest();
int addVectorFullIntTest();
int fullBandWidthTest();
int matmulTest();
char* error_str[] = { };

typedef int (*test)();
test tests[] = { 
      matmulTest, 
      //fullBandWidthTest, 
      //addVectorIntTest,
		  //addVectorFullFloatTest, 
		  //addVectorFullIntTest, 
		};   // testcase functions

int allInOne(int argc, char** argv) {
	int i;

	for (i = 0; i < sizeof(tests) / sizeof(test); ++i) {
		int result = tests[i]();
		if (result) {
			printf(
					"!!!!!!!!!!!!!!!!!!!!!!  test %d failed, errorcode: %d, error: %s\n",
					i, result, error_str[result < 0 ? -result : result]);
		}
	}

	return 0;
}

