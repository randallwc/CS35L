#include <gmp.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int
err (char const *format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  return EXIT_FAILURE;
}

int
main (int argc, char **argv)
{
  if (argc != 2)
    return err ("%s: usage: %s exponent\n", argv[0], argv[0]);
  errno = 0;
  char *end;
  unsigned long exponent = strtoul (argv[1], &end, 10);
  if (argv[1] == end || *end || errno == ERANGE)
    return err ("%s: %s: invalid exponent\n", argv[0], argv[1]);
  mpz_t z;
  mpz_init (z);
  mpz_ui_pow_ui (z, 2, exponent);
  if (! (mpz_out_str (stdout, 10, z)
	 && printf ("\n") == 1
	 && fflush (stdout) == 0))
    return err ("%s: output error\n", argv[0]);
  return EXIT_SUCCESS;
}
