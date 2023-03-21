#include "main.h"
#include "extern.h"

extern void ScanCommandLineParams( int argc, char *argv[], char *name);
extern void Dynamics(System *, LIST ***);

FILE *fperr;
struct Input In;

int main( int argc, char *argv[])
{

System balls;/* set of balls */
LIST ***box; /* boxes tensor to speed up simulation */
char ProjName[128];
char inifile[128]="PALLETTE.INI";


 printf("\nProgram PALLETTE Version %4.2f is running.\n\n",VERSION);
 fperr = stderr;

 ScanCommandLineParams( argc, argv, inifile);
 InputParameters(inifile);

 sprintf(ProjName, "%s", In.projname);

 box = BoxAlloc();
 SystemAlloc( &balls, In.nc);
 SystemInit ( &balls);

 Dynamics(&balls, box);

 SystemPrint( &balls, In.projname);

 SystemFree ( &balls);
 BoxFree(box);

 printf("\n------------------ END -----------------\n\n");
 return 0;
}
