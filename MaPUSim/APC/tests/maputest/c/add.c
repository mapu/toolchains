#include <stdio.h>
#include <stdlib.h>

/*
//test1:
#include<stdio.h>
int main() {
  static int a[8] = {1,2,3,4,5,6,7,8}, b[8] = {1,2,3,4,5,6,7,8}, c = 0;
  for(int i =0;i<8;i++){
    c += b[i] * a[i];
  }
  return 0;
}
*/

//#include <stdio.h>
/*
int foo(volatile *mem, int val, int c) {
  int oldval = __sync_fetch_and_add(mem, val);
  return oldval + c;
}
*/



/*
//test2:
#include<stdio.h>
int main() {
  int a;
  for(int i =0;i<8;i++){
    a = a +1;
  }
  return 0;
}*/
/*
#include<stdio.h>
int main() {
  long long a = 2ULL;
  long long b = 2ULL;
  long long c ;
  c = a + b;
  
  return 0;
}*/


/*
//test3:
unsigned  ufoo(unsigned a, unsigned b)
{
  unsigned c = a/b;
  return  c;
}

int main()
{
  unsigned c = ufoo(88, 5);

  return  c;
}
*/



//
//test4
int main()
{
  int a, b, c, d, e;
  long long g;
  a = 1;
  b = 2;
  c = a + b;
  d = 536870911;
  e = -6870911;
  char f[] = "what a nice day!";
  g = -16ULL;

  printf("1234\n");
  printf("abcd\n");
  printf("************hello************\n");
  return  c;
}


/*
//test 5
int main(int argc,char *argv[])
{

//  int m = 1, n = 2;
//  int j = m + n;
  

//  float a = 0.1, b = 0.2,c;
//  int i = 1;
//  unsigned int j =2;
//  float m = i;
//  float n = j;
//  double i = 1, j =2, m;
//  m = i + j;
  //unsigned int i = -1;
  //long long j = -1U;
  //a = i;
  //b = j;
//  c = a + b;
//  printf("%d %d\n", i, j);
  double a = 2, b = 1, c, d, e, f;
  int i;
  unsigned int j;
  float m;
  
  c = a + b;
  d = a - b;
  e = a * b;
  f = a / b;
  
  i = a;
  j = a;
  m = a;
  
  int t = 20000;
  printf("%d\n",t);
return 0;
}
*/
/*
#include <stdio.h>
int  main()
{
  printf("fdasfd\n");
  return 0;
}
*/
/*
int main()
{
  float a = 20.3;
  float b = 2;
  int d;
  if(a*b == 40.6) {
    d = 1;
  }
  return  d;
}
*/
