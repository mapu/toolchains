// RUN: clang -target mspu -S -o %t.s %s

// from newlib

typedef int error_t;
typedef long unsigned int size_t;
typedef long int ptrdiff_t;
error_t argz_add (char **argz, size_t *argz_len, const char *str);
size_t strlen (const char *);
void * realloc (void * __r, size_t __size) ;
void * memcpy (void *, const void *, size_t);
void * memmove (void *, const void *, size_t);



error_t
argz_insert(char **argz , size_t *argz_len , char *before , const char *entry)
{
  int len = 0;
  ptrdiff_t delta;

  if (before == ((void*)0))
    return argz_add(argz, argz_len, entry);

  if (before < *argz || before >= *argz + *argz_len)
    return 22;

  while (before != *argz && before[-1])
    before--;


  delta = before - *argz;

  len = strlen(entry) + 1;

  if(!(*argz = (char *)realloc(*argz, *argz_len + len)))
    return 12;

  memmove(*argz + delta + len, *argz + delta, *argz_len - delta);
  memcpy(*argz + delta, entry, len);

  *argz_len += len;

  return 0;
}

