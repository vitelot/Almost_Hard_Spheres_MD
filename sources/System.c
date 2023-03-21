#include "main.h"
#include "extern.h"
#include <time.h>

#define MAXFAIL 1000000 /* max nr of failed ball placement, in case of core */

#define Min(x,y) (((x)<(y))?(x):(y))


static Int SelectBallType(void);

void SystemAlloc(System *B, int NC) {

  B->n = NC;
  
  B->ball = (Ball *) malloc( (size_t) NC * sizeof(Ball));
}

void SystemRealloc(System *B, int NC) {

  B->n = NC;
  
  B->ball = (Ball *) realloc( B->ball, NC * sizeof(Ball));
}

void SystemFree( System *B) {
  B->n = 0;
  free(B->ball);
  B->ball = NULL;
}

void SystemCopy( System *dest, System *source) {
  register int i;
  int NC = dest->n;
  Ball *D, *S;

  if(dest->n != source->n) {
    perr("Cannot copy two systems of different size (%d != %d).\n",
	 dest->n, source->n);
    exit(1);
  }

  Loop( i, NC) {
    D = &(dest->ball[i]);
    S = &(source->ball[i]);
    D->p.x = S->p.x;
    D->v.x = S->v.x;
    D->F.x = S->F.x;
    D->p.y = S->p.y;
    D->v.y = S->v.y;
    D->F.y = S->F.y;
    D->p.z = S->p.z;
    D->v.z = S->v.z;
    D->F.z = S->F.z;
    D->r   = S->r;
    D->m   = S->m;
    D->type= S->type;
    D->pinned= S->pinned;
  }
}

void SystemSwap( System *A, System *B) {
  register int i;
  int NC = A->n;
  Real tmp;
  char type;
  Bool pinned;

  if(A->n != B->n) {
    perr("Cannot swap two systems of different size (%d != %d).\n",
	 A->n, B->n);
    exit(1);
  }

  Loop( i, NC) {
    tmp = A->ball[i].p.x;
    A->ball[i].p.x = B->ball[i].p.x;
    B->ball[i].p.x = tmp;
    tmp = A->ball[i].v.x;
    A->ball[i].v.x = B->ball[i].v.x;
    B->ball[i].v.x = tmp;
    
    tmp = A->ball[i].p.y;
    A->ball[i].p.y = B->ball[i].p.y;
    B->ball[i].p.y = tmp;
    tmp = A->ball[i].v.y;
    A->ball[i].v.y = B->ball[i].v.y;
    B->ball[i].v.y = tmp;

    tmp = A->ball[i].p.z;
    A->ball[i].p.z = B->ball[i].p.z;
    B->ball[i].p.z = tmp;
    tmp = A->ball[i].v.z;
    A->ball[i].v.z = B->ball[i].v.z;
    B->ball[i].v.z = tmp;

    tmp = A->ball[i].r;
    A->ball[i].r = B->ball[i].r;
    B->ball[i].r = tmp;

    tmp = A->ball[i].m;
    A->ball[i].m = B->ball[i].m;
    B->ball[i].m = tmp;

    type = A->ball[i].type;
    A->ball[i].type = B->ball[i].type;
    B->ball[i].type = type;

    pinned = A->ball[i].pinned;
    A->ball[i].pinned = B->ball[i].pinned;
    B->ball[i].pinned = pinned;

  }
}

void SystemInit( System *B) {
  /* load starting positions and velocities if file "+positions" exists */
  register int i;
  Int nrpinned=0;
  FILE *fp;

  if( (fp=fopen("+positions", "r")) ) {
    char line[128];
 
    printf("File \"+positions\" with starting ball positions found. Loading it.\n\n");

    /* count how many balls there are */
    i=0;
    while(1) {
      fgets( line, 126, fp);
      if(feof(fp)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      ++i;
/*       printf("### %d %s", i, line); */
    }
    if( i != B->n ) {
      printf("Found %d balls instead of %d. Using %d from now on.\n\n",
	     i, B->n, i);
      SystemRealloc( B, i);
      In.nc = i;
    }
    rewind(fp);
    /* finally load ball positions */
    i=0;
    while(1) {
      fgets( line, 126, fp);
      if(feof(fp)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      sscanf(line, "%*d %lg %lg %lg %lg %lg %lg %lg %lg %c %d", 
	     &B->ball[i].p.x, &B->ball[i].p.y, &B->ball[i].p.z,
	     &B->ball[i].v.x, &B->ball[i].v.y, &B->ball[i].v.z,
	     &B->ball[i].r, &B->ball[i].m, &B->ball[i].type, &B->ball[i].pinned);
      if(B->ball[i].pinned) { B->ball[i].v.x = B->ball[i].v.y = B->ball[i].v.z = 0.0; }
      ++i;
    }
    fclose(fp);

  } else {
  /* assign balls random positions inside the cube with edge In.l
     and random velocities */

    srand(time(NULL));
    {
      Bool failed;
      register int j;
      Int t;
      Int countfail;
      Real L = In.l;
      Real Lz = In.lz;
      Real xtmp, ytmp, ztmp, dx, dy, dz, ri, rj;
/*       if( In.l/In.radius * In.l/In.radius * In.lz/In.radius *0.75/3.14 <= In.nc) { */
/* 	printf("Not enough space for balls without overlap. reduce number of balls (<<%d).\n", */
/* 	       (int)(In.l/In.radius * In.l/In.radius * In.l/In.radius *0.75/3.14)); */
/* 	exit(1); */
/*       } */


      Loop(i,B->n) {
	B->ball[i].pinned = FALSE;
	/* set initial speed */
	B->ball[i].v.x = In.v0*2.0*(0.5-rand()/(RAND_MAX+1.0))/1.732; /* -v0 < |v| < v0 */
	B->ball[i].v.y = In.v0*2.0*(0.5-rand()/(RAND_MAX+1.0))/1.732;
	B->ball[i].v.z = In.v0*2.0*(0.5-rand()/(RAND_MAX+1.0))/1.732;

	if(In.radius<=0) {
	  t = SelectBallType();
	  B->ball[i].type = In.ball_t.type[t];
	  B->ball[i].r = In.ball_t.radius[t];
	  B->ball[i].m = In.ball_t.mass[t];
	} else {
	  B->ball[i].type = 'A';
	  B->ball[i].r = In.radius;
	  B->ball[i].m = 1.0;
	}
      }
      B->ball[0].p.x = rand()/(RAND_MAX+1.0)*L;
      B->ball[0].p.y = rand()/(RAND_MAX+1.0)*L;
      B->ball[0].p.z = rand()/(RAND_MAX+1.0)*Lz;
      for(i=1; i<(B->n); i++) {
	countfail = 0;
	ri = B->ball[i].r;
	do {
	  failed = FALSE;
	  xtmp = rand()/(RAND_MAX+1.0)*L;
	  ytmp = rand()/(RAND_MAX+1.0)*L;
	  ztmp = rand()/(RAND_MAX+1.0)*Lz;
	  Loop(j,i) {
	    dx = xtmp - B->ball[j].p.x;
	    dy = ytmp - B->ball[j].p.y;
	    dz = ztmp - B->ball[j].p.z;
	    rj = B->ball[j].r;
/* 	    printf("### i:%d xi:%g yi:%g j:%d xj:%g yj:%g dx:%g dy:%g\n", */
/* 		   i,B->ball[j].p.x,B->ball[j].p.y, */
/* 		   j,xtmp,ytmp, */
/* 		   dx,dy); */
	    /* PBC along x and y only*/
	    if(dx>0) while(fabs(dx)>L/2.) dx -= L;
	    if(dx<0) while(fabs(dx)>L/2.) dx += L;
	    if(dy>0) while(fabs(dy)>L/2.) dy -= L;
	    if(dy<0) while(fabs(dy)>L/2.) dy += L;
/* 	    printf("*** i:%d j:%d dx:%g dy:%g\n",i,j,dx,dy); */

	    if( dx*dx + dy*dy + dz*dz <= (ri+rj)*(ri+rj)) {
	      ++countfail;
	      failed = TRUE;
/* 	      printf("??? d/2r:%g\n", sqrt(dx*dx + dy*dy)/2./In.radius); */
	      break;
	    }
	  } 
	  if(countfail > MAXFAIL) {
	    printf("Cannot find an empty space for ball #%d of type %c after %d trials. Set a lower density.\n",
		   i, B->ball[i].type, MAXFAIL);
	    exit(1);
	  }
	} while(failed);
	B->ball[i].p.x = xtmp;
	B->ball[i].p.y = ytmp;
	B->ball[i].p.z = ztmp;
	if(ztmp < In.pinlvl) {
	  B->ball[i].pinned=TRUE;
	  ++nrpinned;
	}
	printf("Placed ball of type %c. Placing still %d initial balls. Rejected: %d         \r", 
	       B->ball[i].type, B->n-i, countfail);
	fflush(stdout);
      }
      printf("                                                                       \n");
    }
  }
  if(nrpinned>0)
    printf("Pinned %d balls at z-level less than %f\n", nrpinned, In.pinlvl);
  printf("Initial packing ratio: %.1f%%\n\n", 100.*SystemPackingRatio(B));
}

void SystemPrint( System *B, const char *projname) {
  register int i;
  char filename[128];
  FILE *fp;

  if(! In.printdat) return;

  sprintf(filename, "=%s.%s.dat", projname, "BallPositions");
  printf("Saving balls positions and velocities onto file \"%s\".\n",
         filename);

  fp = fopen( filename, "w");
  if( !fp)  FileOpenError(filename);

  fprintf(fp, "#%5s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%4s\t%4s\n",
	  "i", "x", "y", "z", "vx", "vy", "vz", "r", "m", "type", "pin");

  Loop( i, B->n) {
    fprintf(fp, "%5d\t%10g\t%10g\t%10g\t%10g\t%10g\t%10g\t%10g\t%10g\t%4c\t%4d\n", i,
	    B->ball[i].p.x,
	    B->ball[i].p.y,
	    B->ball[i].p.z,
	    B->ball[i].v.x,
	    B->ball[i].v.y,
	    B->ball[i].v.z,
	    B->ball[i].r,
	    B->ball[i].m,
	    B->ball[i].type,
	    B->ball[i].pinned);
  }

  fclose(fp);

  if(In.rasmol){
    char command[128];
    const Real rasmol_factor=10.0;

    sprintf(filename, "=%s.%s.xyz", projname, "BallPositions");
    printf("Saving balls positions in XYZ format onto file \"%s\".\n",
	   filename);

    fp = fopen( filename, "w");
    if( !fp)  FileOpenError(filename);

    fprintf(fp, "%d\n", B->n);
    fprintf(fp, "Sferette - XYZ format\n");
    
    Loop( i, B->n) {
      fprintf(fp, "%c %10g\t%10g\t%10g\n",
	      B->ball[i].type,
	      rasmol_factor*B->ball[i].p.x,
	      rasmol_factor*B->ball[i].p.y,
	      rasmol_factor*B->ball[i].p.z);
    }
    fclose(fp);

      printf("Creating RASMOL.script\n");
      fp=fopen("RASMOL.script","w");
      fprintf(fp, "zap\nload xyz %s\n",filename);
      if(In.radius>0) {
	fprintf(fp, "spacefill %f\n", rasmol_factor*In.radius);
      } else {
	Loop(i,In.ball_t.nt) {
	  fprintf(fp, "select *.%c\n",In.ball_t.type[i]);
	  fprintf(fp, "spacefill %f\n", rasmol_factor*In.ball_t.radius[i]);
	}
      }
      fprintf(fp,
	      "select all\n"
	      "wireframe off\n"
	      "set boundingbox on\n"
	      "set axes on\n");
/*       fprintf(fp, "write ppm %s.ppm\n", filename); */
      fclose(fp);

    printf("Invoking RASMOL.\n");
    sprintf(command, "rasmol -script RASMOL.script");
    system(command);
  }

}

static Int SelectBallType(void) {
  Int i;
  Real cumul=0.0;
  static Int acc=0;
  static Int *Counter;

  if(acc==0) {
    Counter = (Int*)malloc(sizeof(Int)*In.ball_t.nt);
    Loop(i,In.ball_t.nt) {
      Counter[i] = (Real)In.nc * In.ball_t.abundance[i];
    }
  }

  Loop(i,In.ball_t.nt) 
    if(Counter[i]) {
      acc++;
      --Counter[i];
      return i;
    }

  return (i-1);

}

Real SystemPackingRatio(System *S) {
  register int i;
  Ball *B;
  Int n = S->n;
  Real xm, ym ,zm; /* geometrical center */
  Real R;
  Real x ,y, z, r, d, Vb;

  /* find the geometrical center */
  xm = ym = zm = 0.0;
  Loop( i, n) {
    B = &(S->ball[i]);
    xm += B->p.x;
    ym += B->p.y;
    zm += B->p.z;
  }
  xm /= n;
  ym /= n;
  zm /= n;

  /* set the radius of big sphere */
  R = Min(zm, Min(ym,xm));

/*   printf("xm:%f ym:%f zm:%f R:%f\n",xm,ym,zm,R); */

  Vb = 0.0; /* volume, divided 4/3 Pi, of the sum of small spheres */
  Loop( i, n) {
    B = &(S->ball[i]);
    x = B->p.x;
    y = B->p.y;
    z = B->p.z;
    r = B->r;

    /* distance between centers */
    d = sqrt( (xm-x)*(xm-x)+(ym-y)*(ym-y)+(zm-z)*(zm-z) );
    if(d >= R+r) continue; /* spheres are extern each other */
    if(d <= R-r) { Vb += r*r*r; continue; } /* spheres are internal */

    /* spheres are intersecting along a lens */
    Vb += (R+r-d)*(R+r-d)*(d*d + 2.*d*r - 3.*r*r + 2.*d*R + 6.*r*R - 3.*R*R)/(16.*d);
    /* http://mathworld.wolfram.com/Sphere-SphereIntersection.html */
  }

  return Vb/(R*R*R);
}
