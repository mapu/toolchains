// RUN: %clangxx_asan -O0 -DSHARED_LIB %s -fPIC -shared -o %t-so.so
// RUN: %clangxx_asan -O0 %s %t-so.so -o %t

// Check that without suppressions, we catch the issue.
// RUN: not %run %t 2>&1 | FileCheck --check-prefix=CHECK-CRASH %s

// RUN: echo "interceptor_via_lib:%t-so.so" > %t.supp
// RUN: ASAN_OPTIONS="suppressions='%t.supp'" %run %t 2>&1 | FileCheck --check-prefix=CHECK-IGNORE %s

// XFAIL: android

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(SHARED_LIB)

void crash_function();

int main(int argc, char *argv[]) {
  crash_function();
  return 0;
}

#else // SHARED_LIB

void crash_function() {
  char *a = (char *)malloc(6);
  free(a);
  size_t len = strlen(a); // BOOM
  fprintf(stderr, "strlen ignored, %zu\n", len);
}

#endif // SHARED_LIB

// CHECK-CRASH: AddressSanitizer: heap-use-after-free
// CHECK-CRASH-NOT: strlen ignored
// CHECK-IGNORE-NOT: AddressSanitizer: heap-use-after-free
// CHECK-IGNORE: strlen ignored
