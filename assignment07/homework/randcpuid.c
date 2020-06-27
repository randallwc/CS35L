//randcpuid.c
//should contain the code that determines whether the current CPU has the RDRAND instruction
//It should start by including randcpuid.h and should implement the interface described by randcpuid.h.
//doesn't need to do I/O, it shouldn't include stdio.h
//randcpuid does not need to export the cpuid function, that function should be static and not extern

#include "randcpuid.h"
#include <cpuid.h>
#include <stdbool.h>

/* Description of the current CPU.  */
struct cpuid { unsigned eax, ebx, ecx, edx; };

/* Return information about the CPU.  See <http://wiki.osdev.org/CPUID>.  */
static struct cpuid
cpuid (unsigned int leaf, unsigned int subleaf)
{
  struct cpuid result;
  asm ("cpuid"
       : "=a" (result.eax), "=b" (result.ebx),
         "=c" (result.ecx), "=d" (result.edx)
       : "a" (leaf), "c" (subleaf));
  return result;
}

/* Return true if the CPU supports the RDRAND instruction.  */
extern _Bool
rdrand_supported (void)
{
  struct cpuid extended = cpuid (1, 0);
  return (extended.ecx & bit_RDRND) != 0;
}
