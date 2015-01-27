#include <stdio.h>

struct bar {
  int m1;
  int m2;
};

void f1(int a, struct bar *b) __attribute__ ((noinline));
void f1(int a, struct bar *b)
{
  b->m2 = b->m1 + a; // set breakpoint here
}

void f2(struct bar *b) __attribute__ ((noinline));
void f2(struct bar *b)
{
  int c = b->m2;
  printf("%d\n", c); // set breakpoint here
}

int main()
{
  struct bar myBar = { 3, 4 };
  f1(2, &myBar);
  f2(&myBar);
  return 0;
}
