#include "main.h"
#include "extern.h"

static void create_ini( char *);
static void printinfo(Bool);
static void create_ball_type_ini(char *file);

void InputParameters( char *ini_file)
{
FILE *fp;
char line[128], *p;
int totlin=21;
double version;

    fp = fopen( ini_file, "r");
    if( !fp) {
      printf("Error opening file %s\n", ini_file);
      printf("Creating a default one.\n\n");
      create_ini( ini_file);
    }

    while( !feof(fp)) {
      fgets( line, 126, fp);
      if( line[0] == '#') continue; /* lines starting with # are comments */
      /* read up to # or line end */
      for( p=line; *p!='#' && *p!='\0'; p++) ;
      *p = '\0'; /* skip inline comments */

      if(sscanf(line, " VERSION %lf", &version)           ==1) {
	totlin--;
	if(version != VERSION) {
	  perr("The initialization file \"%s\" belongs to an old release %.2lf.\n", ini_file, version);
	  perr("Please delete it and rerun the program to create its new version.\n");
	  exit(1);
	}
	continue;
      }
      /*###################################################################*/
      if(sscanf(line, " NC         =   %d", &In.nc)      ==1) {totlin--; continue;}
      if(sscanf(line, " L          =   %lg",&In.l)       ==1) {totlin--; continue;}
      if(sscanf(line, " LZ         =   %lg",&In.lz)      ==1) {totlin--; continue;}
      if(sscanf(line, " DT         =   %lg",&In.dt)      ==1) {totlin--; continue;}
      if(sscanf(line, " TMAX       =   %lg",&In.tmax)    ==1) {totlin--; continue;}
      if(sscanf(line, " RADIUS     =   %lg",&In.radius)  ==1) {totlin--; continue;}
      if(sscanf(line, " V0         =   %lg",&In.v0)      ==1) {totlin--; continue;}
      if(sscanf(line, " GRAVITY    =   %lg",&In.gravacc) ==1) {totlin--; continue;}
      if(sscanf(line, " PINLEVEL   =   %lg",&In.pinlvl)  ==1) {totlin--; continue;}
      if(sscanf(line, " FRICTION   =   %lg",&In.friction)==1) {totlin--; continue;}
      if(sscanf(line, " FRICTTIME  =   %lg",&In.frctime) ==1) {totlin--; continue;}
      if(sscanf(line, " COOLING    =   %lg",&In.cooling) ==1) {totlin--; continue;}
      if(sscanf(line, " COOLINGTIME=   %lg",&In.cooltime)==1) {totlin--; continue;}

      if(sscanf(line, " BOXN       =   %d", &In.boxn)    ==1) {totlin--; continue;}
      if(sscanf(line, " PROJNAME   =   %s", In.projname) ==1) {totlin--; continue;}

      if(sscanf(line, " SAVESTEP   =   %d", &In.savestep)==1) {totlin--; continue;}
      if(sscanf(line, " TRACK      =   %d", &In.track   )==1) {totlin--; continue;}
      if(sscanf(line, " KINETIC_E  =   %d", &In.kinE    )==1) {totlin--; continue;}
      if(sscanf(line, " PRINT      =   %d", &In.printdat)==1) {totlin--; continue;}
      if(sscanf(line, " RASMOL     =   %d", &In.rasmol)==1)   {totlin--; continue;}
      
      /* if you reach here, the line is not recognized */
      perr("The following line in %s was not recognized:\n%s\n"
	   "Exiting.\n", ini_file, line);
      exit(1);
    }
     fclose(fp);

     if(totlin > 0) {
       printf("There are %d lines less in your %s file. Check for missing ones. Exiting.\n",
	      totlin, ini_file);
       exit(1);
     }
     if(totlin < 0) {
       printf("There are %d lines more in your %s file. Check for duplicates. Exiting.\n",
	      -totlin, ini_file);
       exit(1);
     }

     if(In.radius <= 0 && !FileExists("+positions")) { 
       /* read file with ball specifications */
       SetBallTypes();
     }

     printinfo(FALSE);
     printinfo( TRUE);
}

#define  prbool(x) ((x)? "TRUE": "FALSE")

static void printinfo(Bool tofile)
{
FILE *fp=stdout;

  if(tofile) {
    char file[128];
    sprintf( file, "+%s.init", In.projname);
    fp = fopen( file, "w");
    if(!fp) FileOpenError( file);
  }

  fprintf(fp, "#IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, " VERSION %4.2f\n", VERSION);
  fprintf(fp, "#IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, "#IIIIIIII Main computational parameters IIIIIIIIIII\n");
  fprintf(fp, " NC         = %d\n",                           In.nc);
  fprintf(fp, " L          = %g\n",                           In.l);
  fprintf(fp, " LZ         = %g\n",                           In.lz);
  fprintf(fp, " DT         = %g\n",                           In.dt);
  fprintf(fp, " TMAX       = %g\n",                           In.tmax);
  fprintf(fp, " RADIUS     = %g\n",                           In.radius);
  fprintf(fp, " V0         = %g\n",                           In.v0);
  fprintf(fp, " GRAVITY    = %g\n",                           In.gravacc);
  fprintf(fp, " PINLEVEL   = %g\n",                           In.pinlvl);
  fprintf(fp, " FRICTION   = %g\n",                           In.friction);
  fprintf(fp, " FRICTTIME  = %g\n",                           In.frctime);
  fprintf(fp, " COOLING    = %g\n",                           In.cooling);
  fprintf(fp, " COOLINGTIME= %g\n",                           In.cooltime);
  fprintf(fp, "#IIIIIIII Basic control  IIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, " BOXN       = %d\n",                           In.boxn);
  fprintf(fp, " PROJNAME   = \"%s\"\n",                       In.projname);
  fprintf(fp, "#IIIIIIII Output control IIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, " SAVESTEP   = %d\n",                           In.savestep);
  fprintf(fp, " TRACK      = %d\n",                           In.track);
  fprintf(fp, " KINETIC_E  = %d\n",                           In.kinE);
  fprintf(fp, " PRINT      = %s\n",                  prbool(In.printdat));
  fprintf(fp, " RASMOL     = %s\n",                  prbool(In.rasmol));
  fprintf(fp, "#IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n\n");

  if(tofile) {
    fclose( fp);
    return;
  }

  fflush(stdout);
} 

static void create_ini( char *file)
{
  FILE *fel;

    fel = fopen( file, "w");
    if( !fel) {
      perr("Cannot create default %s ini-file. Aborting.\n", file);
      exit(1);
    }
    fprintf( fel, 
     "############################################################################\n"
     "### PALLETTE'S default initialization file #################################\n"
     "############################################################################\n");
    fprintf( fel, 
     " VERSION %4.2f                         ######################################\n", VERSION);
    fprintf( fel, 
     "############################################################################\n"
     "# Use the flag -h to get a list of command line options\n"
     "################### Master parameters ######################################\n"
     "#\n"
     " NC         =    40		# Number of balls\n"
     " L          =    1.0		# X,Y Edge of the box\n"
     " LZ         =    2.0		# Z Edge of the box\n");
    fprintf( fel, 
     " DT         =    1e-2		# time integration step\n"
     " TMAX       =    100.0		# final integration time\n"
     " RADIUS     =    0.1 	        # ball radius. read file if RADIUS=0\n"
     " V0         =    0.0 	        # initial max ball speed, null average\n"
     " GRAVITY    =    0.2		# gravity acceleration\n");
    fprintf( fel, 
     " PINLEVEL   =    0.0		# pin balls with initial z less than this value\n"
     " FRICTION   =    1.0		# intensity of friction\n"
     " FRICTTIME  =    3.0		# time to switch on friction\n"
     " COOLING    =    0.9		# cooling factor (multiplies all velocities)\n"
     " COOLINGTIME=    300.0		# time to switch on cooling\n"
     "#\n");
    fprintf( fel, 
     "################### Basic control  #########################################\n"
     "#\n"
     " BOXN       =    0		# nr of boxes per edge. 0=no boxes. -1 optimal choice.\n"
     " PROJNAME   =    %s		# Project name\n"
     "#\n", In.projname);
    fprintf( fel,
     "################### Output control #########################################\n"
     "#\n"
     " SAVESTEP       =   1000          # Save cell positions on file each SAVESTEP time steps\n"
     " TRACK          =   0             # Save cell trajectories in only one HUGE file each TRACK steps\n"
     " KINETIC_E      =   0             # Save total kinetic energy each given steps\n");
    fprintf( fel,
     " PRINT          =   1             # Save cell positions on file\n"
     " RASMOL         =   0             # Run rasmol to show cells in box\n");
    fprintf( fel, 
     "######################## END ###############################################\n"
     );
    fclose( fel);
    printf("Default \"%s\" initialization file created. You might edit it.\n", file);
    printf("Exiting...\n");
    exit(1);
}

void SetBallTypes(void) {

  FILE *fp;
  char line[128], *p;
  char *ini_file="BALL_TYPES.INI";
  int ns;
  Real tmp;

  fp = fopen( ini_file, "r");
    if( !fp) {
      printf("Error opening file %s\n", ini_file);
      printf("Creating a default one.\n\n");
      create_ball_type_ini( ini_file);
    }
    while( !feof(fp)) {
      fgets( line, 126, fp);
      if( line[0] == '#') continue; /* lines starting with # are comments */
      /* read up to # or line end */
      for( p=line; *p!='#' && *p!='\0'; p++) ;
      *p = '\0'; /* skip inline comments */

      if(sscanf(line, "   NUMBER OF SPECIES: %d", &In.ball_t.nt)==1) break;
    }

    printf("Going to read %d ball specifications from file \"%s\"\n", In.ball_t.nt, ini_file);

    In.ball_t.type      = (char *) malloc( sizeof(char)*In.ball_t.nt);
    In.ball_t.abundance = (Real *) malloc( sizeof(Real)*In.ball_t.nt);
    In.ball_t.radius    = (Real *) malloc( sizeof(Real)*In.ball_t.nt);
    In.ball_t.mass      = (Real *) malloc( sizeof(Real)*In.ball_t.nt);

    ns = 0;

    while( !feof(fp)) {
      fgets( line, 126, fp);
      if( line[0] == '#') continue; /* lines starting with # are comments */
      /* read up to # or line end */
      for( p=line; *p!='#' && *p!='\0'; p++) ;
      *p = '\0'; /* skip inline comments */


      if(sscanf(line, "   %c %lg %lg %lg", 
		&In.ball_t.type[ns], &In.ball_t.abundance[ns], 
		&In.ball_t.radius[ns], &In.ball_t.mass[ns])==4)
	{ns++; continue;}
      if(ns==In.ball_t.nt) break;
    }

    /* normalize abundances */
    tmp = 0.0;
    for(ns=0; ns<In.ball_t.nt; ns++) {
      tmp += In.ball_t.abundance[ns];
    }
    for(ns=0; ns<In.ball_t.nt; ns++) {
      In.ball_t.abundance[ns] /= tmp;
    }

    printf("Found following %d ball sorts:\n", In.ball_t.nt);
    printf("# Type     Abundance      Radius       Mass\n");
    for(ns=0; ns<In.ball_t.nt; ns++)
      printf("   %c       %8g              %g        %g\n", 
	     In.ball_t.type[ns], In.ball_t.abundance[ns],
	     In.ball_t.radius[ns], In.ball_t.mass[ns] );
    printf("\n");

  fclose(fp);
}

static void create_ball_type_ini(char *file) {  

  FILE *fel;

    fel = fopen( file, "w");
    if( !fel) {
      perr("Cannot create default %s ini-file. Aborting.\n", file);
      exit(1);
    }
    fprintf( fel, 
     "############################################################################\n"
     "### BALL TYPES's default initialization file ###############################\n"
     "############################################################################\n");
    fprintf( fel, 
     "   NUMBER OF SPECIES: 1 \n"
     "# Type     Abundance      Radius     Mass\n"
     "   A          100           0.1       1.0\n"
     "############################################################################\n");

    fclose( fel);
    printf("Default \"%s\" initialization file created. You might edit it.\n", file);
    printf("Exiting...\n");
    exit(1);
}

