#include "main.h"
#include "extern.h"

extern void VerletDynamics( System *S, Real t, Real dt, LIST ***box);

static void TrackCells( System *S, FILE *fp, double t);
static Real KineticEnergy( System *S, FILE *fp, double t);
static void Cooling( System *S, double t);

void Dynamics(System *body, LIST ***box) {
  register int i=0, j=0, k=0;
  char file[128];
  Real t=0.0;
  Real dt = In.dt;
  Real tmax = In.tmax;
  Real Ek;

  FILE *fpT;
  FILE *fpE;

  if(In.track) {
    char filename[128];
    sprintf(filename, "=%s.%s.dat", In.projname, "TrackBalls");
    fpT = fopen(filename, "w");
  }
  if(In.kinE) {
    char filename[128];
    sprintf(filename, "=%s.%s.dat", In.projname, "KineticEnergy");
    fpE = fopen(filename, "w");
  }
  
  /* print initial configuration */
  sprintf(file, "%s_%06d", In.projname, i);
  SystemPrint( body, file);

  printf("Doing calculations (saving data each %.2f%%)\n", 
	 In.savestep/In.tmax*In.dt*100);

  for(; t<tmax; t+=dt) { /* bilateral Verlet: 2*dt */
    BoxAssign( body, box);
    VerletDynamics(body, t, dt, box);
    
    if(In.track>0 && (j++ % In.track)==0)
      TrackCells( body, fpT, t);
    if(In.kinE>0 && (k++ % In.kinE)==0)
      Ek = KineticEnergy( body, fpE, t);
/*       if(KineticEnergy( body, fpE, t)<1e-6 && k>In.savestep) { */
/* 	printf("Average kinetic energy reached 0. Simulation ends here\n"); */
/* 	return; */
/*       } */

    if(In.savestep>0 && (++i % In.savestep)==0) {
      printf("\nCurrent packing ratio: %.1f%%\n", 100.*SystemPackingRatio(body));
      printf("Average kinetic energy per particle: %g\n\n", Ek);

      sprintf(file, "%s_%06d", In.projname, i);
      SystemPrint( body, file);

      if(In.track) fflush(fpT);
      if(In.kinE)  fflush(fpE);
    }

    Cooling(body, t);

    ShowProgress( t, tmax);

  }
  printf("Done.                 \n\n");

  if(In.track) fclose(fpT);
  if(In.kinE)  fclose(fpE);
}

static void TrackCells( System *B, FILE *fp, double t) {
  register int i;

  Loop(i,In.nc) {
    fprintf(fp, "%d %f %f %f %f %f %f %f %f %f %f %f\n", i, t,
	    B->ball[i].p.x, B->ball[i].p.y, B->ball[i].p.z,
	    B->ball[i].v.x, B->ball[i].v.y, B->ball[i].v.z,
	    B->ball[i].F.x, B->ball[i].F.y, B->ball[i].F.z,
	    B->ball[i].r);
  }
}

static Real KineticEnergy( System *S, FILE *fp, double t) {
  register int i;
  Real Ek = 0.0;
  Ball *B;
  Loop(i,In.nc) {
    B = &(S->ball[i]);
    Ek += ( B->v.x * B->v.x + 
            B->v.y * B->v.y +
            B->v.z * B->v.z )
      * B->m;
  }

  Ek *= 0.5/In.nc;

  fprintf( fp, "%f %g\n", t, Ek);
  return Ek;
}

static void Cooling( System *S, double t) {

  if(t>=In.cooltime){

    register int i;
    Int nc=S->n;
    Ball *B;

    Loop(i,In.nc) {
      B = &(S->ball[i]);
      B->v.x *= In.cooling;
      B->v.y *= In.cooling;
      B->v.z *= In.cooling;
    }
  }
  return;
}
