// RUN: clang -target mspu -S -o %t.s %s

// pointer add
int  f(int *a, int * b)
{
  int * p = a + 6;
  return  99;
}

// get address of a global
int ga;

int * f2() 
{
	return &ga;
}

int
insert(char *before, char* after)
{
 	*after = *(before+1);
  return 0;
}

int
argz_insert(char *before, char a)
{  
	while (before && a )
    break;
  return 0;
}

