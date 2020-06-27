#include <stdio.h>
int
main (void)
{
  int c = getchar ();
  if (c < 0)
    {
      if (ferror (stdin))
        perror ("stdin");
      else
        fprintf (stderr, "EOF on input\n");
      return 1;
    }
  if (putchar (c) < 0 || fclose (stdout) != 0)
    {
      perror ("stdout");
      return 1;
    }
  return 0;
}
