#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Random.h"

#define Loop(i,N) for( (i)=0; (i)<(N); ++(i))
#define Vector3D Point3D

typedef double Real;
typedef int Int;
typedef enum {FALSE, TRUE} Bool;

typedef struct {
  Real x;
  Real y;
  Real z;
} Point3D;

typedef struct {
  Point3D p; /* position */
  Vector3D v; /* velocity */
  Real r; /* radius */
} Ball;

typedef struct {
  int n;
  Ball *ball;
} System; /* set of n balls */

void ScanSystem(System *S);

void SystemAlloc(System *B, int NC) {

  B->n = NC;
  
  B->ball = (Ball *) malloc( (size_t) NC * sizeof(Ball));
}

int main(int argc, char *argv[]) {
  Int i;
  char *infile = argv[1];
  System B;
  FILE *IN;
  char line[128], *fake;

  if(argc<2){
    fprintf(stderr,"ERROR: input file name missing in command\n");
    exit(1);
  }

  if(!(IN=fopen(infile,"r"))) {
    fprintf(stderr,"ERROR: input file \"%s\" does not exist\n",infile);
    exit(1);
  } else {
    
    /* count how many balls there are */
    i=0;
    while(1) {
      fake=fgets( line, 126, IN);
      if(feof(IN)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      ++i;
      /*       printf("### %d %s", i, line); */
    }
    printf("Found %d particles.\n", i);

    SystemAlloc(&B, i);

    rewind(IN);
    /* finally load ball positions */
    i=0;
    while(1) {
      fake=fgets( line, 126, IN);
      if(feof(IN)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      sscanf(line, "%*d %lg %lg %lg %lg %lg %lg %lg %*c", 
	     &B.ball[i].p.x, &B.ball[i].p.y, &B.ball[i].p.z,
	     &B.ball[i].v.x, &B.ball[i].v.y, &B.ball[i].v.z,
	     &B.ball[i].r);
      ++i;
    }

  }
  fclose(IN);

  InitRandom();

  ScanSystem(&B);
/*   printf("%d %s %g\n",argc, infile, Rand()); */

  return 0;
}

void ScanSystem(System *S) {
  int NB = S->n;
  int i, n=100000, np, p;
  Real xp,yp,zp,rp; /* probe position and radius */
  const Real rpi=0.0001, rpf=.00001, rpstep=.00001;
  const Real xmin=0.02, xmax=0.98, ymin=0.02, ymax=0.98, zmin=0.06, zmax=0.4;
  Real hx=xmax-xmin, hy=ymax-ymin, hz=zmax-zmin;
  Real dx,dy,dz,rr;

  for(rp=rpi; rp>rpf; rp-=rpstep){ 
    np = 0;
    for(p=0; p<n; p++){
      xp = xmin+hx*Rand();
      yp = ymin+hy*Rand();
      zp = zmin+hz*Rand();
      np++;
      for(i=0; i<NB; i++) {
	dx = xp - S->ball[i].p.x;
	dy = yp - S->ball[i].p.y;
	dz = zp - S->ball[i].p.z;
	rr = S->ball[i].r + rp;
	if( dx*dx + dy*dy + dz*dz < rr*rr) {np--; break;}
      }
    }
    printf("%g %g\n", rp, (Real)np/n);
  }
}
