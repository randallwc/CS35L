//randlibhw.c
//should contain the hardware implementation of the random number generator.
//It should start by including randlib.h and should implement the interface described by randlib.h.

#include "randlib.h"
#include <immintrin.h>

/* Return a random value, using hardware operations.  */
extern unsigned long long
rand64 (void)//changed from hardware_rand64
{
  unsigned long long int x;
  while (! _rdrand64_step (&x))
    continue;
  return x;
}

