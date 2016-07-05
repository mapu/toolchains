/* RUN: clang -target mspu -S -o %t.s %s */

// dummy
void  dummp(void) { }

// add 
int add(int a, int b)
{
  // CHECK:  ;;
  return a+b;
}

int mod(int a, int r){
  
    return a%r;
}

int div(int a, int r){
  
    return a/r;
}

// big integers
void f(unsigned * a, unsigned * d){
  if ((a[0]) & ((unsigned)0x80000000L))
    {

      *d = 1;
      d[2] &= ~((unsigned)0x80000000L);
    }
  else
    *d = 0;
}

long long int
 llrint(int i0, int j0)
{

  return (long long int) i0 << (j0);

}

