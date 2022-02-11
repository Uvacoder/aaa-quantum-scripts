#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include "global.h"
#include "rand.h"

long double evaluate(long double *x, int n) {
  static int initialized_dimension = -1;
  char *olddir;

  if (initialized_dimension != n) {
    initialized_dimension = n;
    
#if defined(DEBUG) && DEBUG != 0
    printf("initializing CEC05 benchmark function for dimension %d\n", n);
#endif

    nreal = n;
#if f15 || f16 || f17 || f18 || f19 || f20 || f21 || f22 || f23 || f24 || f25
    nfunc = 10;
#else
    nfunc = 2;
#endif
    olddir = getcwd(0, 0);

#if defined(DEBUG) && DEBUG != 0
    printf("chdir %s\n", ABSDIR);
#endif

    chdir(ABSDIR);
    
    randomize();
    initrandomnormaldeviate();
    initialize();
#if f15 || f16 || f17 || f18 || f19 || f20 || f21 || f22 || f23 || f24 || f25
    calc_benchmark_norm();
#endif

#if defined(DEBUG) && DEBUG != 0
    printf("\nchdir back to %s\n", olddir);
#endif
    chdir(olddir);
    free(olddir);
  }

  return calc_benchmark_func(x)
}

