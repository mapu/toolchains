// Test various levels of coverage
//
// RUN: %clangxx_asan -O1 -fsanitize-coverage=1  %s -o %t
// RUN: ASAN_OPTIONS=coverage=1:coverage_bitset=1:verbosity=1 %run %t 2>&1 | FileCheck %s --check-prefix=CHECK1
// RUN: %clangxx_asan -O1 -fsanitize-coverage=2  %s -o %t
// RUN: ASAN_OPTIONS=coverage=1:coverage_bitset=1:verbosity=1 %run %t 2>&1 | FileCheck %s --check-prefix=CHECK2
// RUN: %clangxx_asan -O1 -fsanitize-coverage=3  %s -o %t
// RUN: ASAN_OPTIONS=coverage=1:coverage_bitset=1:verbosity=1 %run %t 2>&1 | FileCheck %s --check-prefix=CHECK3

// RUN: ASAN_OPTIONS=coverage=1:coverage_bitset=0:verbosity=1 %run %t 2>&1 | FileCheck %s --check-prefix=CHECK3_NOBITSET
// RUN: ASAN_OPTIONS=coverage=1:verbosity=1 %run %t 2>&1 | FileCheck %s --check-prefix=CHECK3_NOBITSET
//
// REQUIRES: asan-64-bits

volatile int sink;
int main(int argc, char **argv) {
  if (argc == 0)
    sink = 0;
}

// CHECK1: CovDump: bitset of 1 bits written, 1 bits are set
// CHECK1:  1 PCs written
// CHECK2: CovDump: bitset of 3 bits written, 2 bits are set
// CHECK2:  2 PCs written
// CHECK3: CovDump: bitset of 4 bits written, 3 bits are set
// CHECK3:  3 PCs written
// CHECK3_NOBITSET-NOT: bitset of
