#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hexlib.h"

int main (int argc, char *argv[])
{
  size_t i;
  const size_t n = _n;
  struct rparams p;

  if(argc < 7)
      return 1;

  init_p(&p);
  for(i = 0; i < n; i++) 
      p.x[i] = atof(argv[i + 1]); 
    
  dir(&p);
  // Direct: 
  printf("\nDirect (from virtual positions to real elongations)\n");
  for (i = 0; i < n; i++)
      printf("d(%d) = %f\n", i, p.d[i]);

  inv(&p);
  // Inverse: 
  printf("\nInverse (from real elongations to virtal positions)\n");
  for (i = 0; i < n; i++)
      printf("x(%d) = %f\n", i, p.x[i]);
  
  return 0;
}

