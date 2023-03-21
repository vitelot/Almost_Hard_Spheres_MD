#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern FILE *fperr;

/* print on file *fperr opened before as global */
void perr(const char *fmt, ...)
{
va_list ap;
 va_start(ap, fmt);
 vfprintf( fperr, fmt, ap);
 va_end(ap);
}

void OpenError(char *errfilename)
{
  fperr = fopen( errfilename, "w");
  if(!fperr) {
    fprintf(stderr,"Error opening file %s as error log file \n", errfilename);
    exit(1);
  }
}

void CloseError(void)
{ fclose( fperr); }

void AllocError( char *s)
{
  perr("Allocation error in %s. Exit.\n", s);
  exit(1);
}

void FileOpenError( char *s)
{
  perr("Cannot open file \"%s\". Exit.\n", s);
  exit(1);
}
