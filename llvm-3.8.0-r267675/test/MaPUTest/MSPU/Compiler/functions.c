// RUN: clang -target mspu -S -o %t.s %s

/*
int add(int num, ...)
{

}

int foo()
{

add(3, 11, 22, 33);

return 0;
}
*/


double bar(int s);

float foo(int s)
{
  float retval = (float) bar (s);
  return retval;
}


void
print(int ptr , int buf , int invalue, int b);

int
dcvt(int ptr , int buffer , int invalue, int b)
{
  print (ptr, buffer, invalue, b);
  return buffer;
}

// test any ext for float-point
char *
fcvt(double d);

char *
fcvtf(float d)

{
  return fcvt ((float) d);
}

// test null arg-list
char *__locale_charset(void);

void __mbtowc ( void )
{
  __locale_charset ();
}


// var arg list
typedef __builtin_va_list va_list;


int
_fiprintf_r(const char *fmt , ...)



{
  int ret;
  va_list ap;

  __builtin_va_start(ap, fmt);
  __builtin_va_end(ap);
  return ret;
}

// ordered and unordered.
struct exception
{
  double arg1;
  double arg2;
};

 int matherr(struct exception *x)
{
 int n=0;
 if(x->arg1==x->arg1) return 0;
 return n;
}

