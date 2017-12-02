// RUN: clang -target mspu -S -o %t.s %s


int  f(int a)
{
  if(a) a+=5;
  
  return  99;
}


int
ff(int mode)
{
  int i;
  switch(mode)
  {
    case 0:
    case 1:
      i = 276;
      break;
    case 2:
      i = 321;
      break;
    case 3:
      i = 43254;
      break;
    case 4:
      i = 325;
      break;
    default:
      break;
  }
  return i;
}
