#include "test_common.h"

int addVectorIntTest();
int addVectorFullFloatTest();
int addVectorFullIntTest();
int fullBandWidthTest();
int matmulTest();
char* error_str[] = {};

typedef int (*test)();
test tests[] = {
    addVectorIntTest, 
    addVectorFullFloatTest, 
    addVectorFullIntTest, 
    matmulTest,
    fullBandWidthTest, 
};   // testcase functions

int allInOne(int argc, char** argv) {
  int i;

  printf("Expected:\nAddVector:         7.2 MOPS\nFullInt:           288 MOPS\nFullFloat:         43.3 MOPS\nFullBandWidth:     172.8 MOPS, 86.4 GB/s\nMatMul:            30 GOPS\n");

  for (i = 0; i < sizeof(tests) / sizeof(test); ++i) {
    int result = tests[i]();
    if (result) {
      printf("!!!!!!!!!!!!!!!!!!!!!!  test %d failed, errorcode: %d, error: %s\n", i, result, error_str[result < 0 ? -result : result]);
    }
  }

  return 0;
}

