//randmain.c
//should contain the main program that glues together everything else.
//It should include randcpuid.h (as the corresponding module should be linked statically)
//but not randlib.h (as the corresponding module should be linked after main starts up)
//Depending on whether randcpuid reports that the hardware supports the RDRAND instruction,
//this main program should dynamically link the hardware-oriented or software-oriented implementation of randlib,
//doing the dynamic linking via dlopen and dlsym.
//Also, the main program should call dlclose to clean up before exiting.
//Like randall, if any function called by the main program fails, the main program should report an error and exit with nonzero status.

#include "randcpuid.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static bool
writebytes (unsigned long long x, int nbytes)
{
  int ndigits = nbytes * 2;
  do
    {
      if (putchar ("0123456789abcdef"[x & 0xf]) < 0)
        return false;
      x >>= 4;
      ndigits--;
    }
  while (0 < ndigits);

  return 0 <= putchar ('\n');
}

/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  if (argc == 2)
    { 
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
        perror (argv[1]);
      else
        valid = !*endptr && 0 <= nbytes;
    }
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
	void* handle;
	
	if(rdrand_supported ()){
		handle = dlopen("./randlibhw.so", RTLD_LAZY);
	}
	else{
		handle = dlopen("./randlibsw.so", RTLD_LAZY);
	}

	if(! handle){
		fprintf(stderr, "dlopen() error - %s\n", dlerror());	
		return 1;
	}

	//get the rand64 function
	unsigned long long (*rand64) (void);
	rand64 = dlsym(handle, "rand64");

	//check if dlsym worked			
	 char *error = dlerror();
	 if (error != NULL){
	 	fprintf(stderr, "dlsym() error with rand64 - %s\n", error);
		if(dlclose(handle) != 0){
				fprintf(stderr, "dlclose() error\n");
		}
	 	return 1;
	 }

	int wordsize = sizeof (* rand64) ();
	int output_errno = 0;

	do
	{
		//use the imported rand64 function and dereference
		unsigned long long x = (* rand64) ();
		int outbytes = nbytes < wordsize ? nbytes : wordsize;
		if (!writebytes (x, outbytes))
		{
		  output_errno = errno;
		  break;
		}
		nbytes -= outbytes;
	}
	while (0 < nbytes);

	if (fclose (stdout) != 0)
		output_errno = errno;

	if (output_errno)
	{
		errno = output_errno;
		perror ("output");
		//close handle
		if(dlclose(handle) != 0){
			fprintf(stderr, "dlclose() error\n");
		}
		return 1;
	}

	//close handle
	if(dlclose(handle) != 0){
		fprintf(stderr, "dlclose() error\n");
		return 1;
	}
		
	return 0;
}
