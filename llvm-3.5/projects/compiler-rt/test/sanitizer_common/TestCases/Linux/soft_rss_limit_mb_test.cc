// Check soft_rss_limit_mb. Not all sanitizers implement it yet.
// RUN: %clangxx -O2 %s -o %t
//
// Run with limit should fail:
// RUN: %tool_options=soft_rss_limit_mb=400:quarantine_size=1:allocator_may_return_null=1     %run %t 2>&1 | FileCheck %s -check-prefix=CHECK_MAY_RETURN_1
// RUN: %tool_options=soft_rss_limit_mb=400:quarantine_size=1:allocator_may_return_null=0 not %run %t 2>&1 | FileCheck %s -check-prefix=CHECK_MAY_RETURN_0

// FIXME: make it work for other sanitizers.
// XFAIL: lsan
// XFAIL: tsan
// XFAIL: msan
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const int kMaxNumAllocs = 1 << 10;
static const int kAllocSize = 1 << 20;  // Large enough to go vi mmap.

static char *allocs[kMaxNumAllocs];

int main() {
  int num_allocs = kMaxNumAllocs / 4;
  for (int i = 0; i < 3; i++, num_allocs *= 2) {
    fprintf(stderr, "[%d] allocating %d times\n", i, num_allocs);
    int zero_results = 0;
    for (int j = 0; j < num_allocs; j++) {
      if ((j % (num_allocs / 4)) == 0) {
        usleep(100000);
        fprintf(stderr, "  [%d]\n", j);
      }
      allocs[j] = (char*)malloc(kAllocSize);
      if (allocs[j])
        memset(allocs[j], -1, kAllocSize);
      else
        zero_results++;
    }
    if (zero_results)
      fprintf(stderr, "Some of the malloc calls returned null: %d\n",
              zero_results);
    if (zero_results != num_allocs)
      fprintf(stderr, "Some of the malloc calls returned non-null: %d\n",
              num_allocs - zero_results);
    for (int j = 0; j < num_allocs; j++) {
      free(allocs[j]);
    }
  }
}

// CHECK_MAY_RETURN_1: allocating 256 times
// CHECK_MAY_RETURN_1: Some of the malloc calls returned non-null: 256
// CHECK_MAY_RETURN_1: allocating 512 times
// CHECK_MAY_RETURN_1: Some of the malloc calls returned null:
// CHECK_MAY_RETURN_1: Some of the malloc calls returned non-null:
// CHECK_MAY_RETURN_1: allocating 1024 times
// CHECK_MAY_RETURN_1: Some of the malloc calls returned null:
// CHECK_MAY_RETURN_1: Some of the malloc calls returned non-null:

// CHECK_MAY_RETURN_0: allocating 256 times
// CHECK_MAY_RETURN_0: Some of the malloc calls returned non-null: 256
// CHECK_MAY_RETURN_0: allocating 512 times
// CHECK_MAY_RETURN_0: allocator is terminating the process instead of returning
