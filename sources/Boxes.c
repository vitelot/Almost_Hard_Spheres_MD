#include "main.h"
#include "extern.h"

static Int BestBoxNr(void);

LIST *** BoxAlloc(void) { /* alloc and initialize */
  int i,j;
  Int n; /* nr of boxes is n^3 */
  LIST ***box;

  if(In.boxn<0) In.boxn = BestBoxNr();

  if(In.boxn<2) return;

  n=In.boxn;
  printf("Allocating %d bytes for boxes\n", (int)(n*n*n*sizeof(LIST)));

  box = (LIST ***) malloc( (size_t) n * sizeof(LIST **) );
  if (!box) perr("Allocation ERROR 1 in  funct BOX_ALLOC\n\n");

  for(i=0; i<n; i++) {
    box[i] = (LIST **) malloc( (size_t) n * sizeof(LIST *) );
    if (!box[i]) perr("Allocation ERROR 2 in  funct BOX_ALLOC\n\n");

    for(j=0; j<n; j++) {
      box[i][j] = (LIST *) malloc( (size_t) n * sizeof(LIST) );
      if (!box[i][j]) perr("Allocation ERROR 3 in  funct BOX_ALLOC\n\n");
    }
  }

  BoxInit(box);

  return box;
}

void BoxInit(LIST ***box) {
  int i,j,k;
  Int n=In.boxn; /* nr of boxes is n^3 */

  if(In.boxn<2) return;

  for(i=0;i<n;i++) {
    for(j=0;j<n;j++) {
      for(k=0;k<n;k++) {
	ListInit( &box[i][j][k]);
      }}}
}

void BoxFree(LIST ***box) {
  int i,j,k;
  Int n=In.boxn; /* nr of boxes is n^3 */

  if(In.boxn<2) return;

/*   printf("Freeing %d bytes from boxes\n", (int)(n*n*n*sizeof(LIST))); */

  for(i=0;i<n;i++) {
    for(j=0;j<n;j++) {
      for(k=0;k<n;k++) {
	ListFree( &box[i][j][k]);
      }
      free(box[i][j]);
    }
    free(box[i]);
  }
  free(box);
}

void BoxAssign(System *S, LIST ***box) {
if(In.boxn<2) {return;}
else {

  int i;
  int ns = S->n;
  int bn = In.boxn;
  Real L = In.l;   /* X,Y edge */
  Real Lz = In.lz; /*  Z  edge */
  Real stepx, stepy, stepz;
  Real x,y,z;
  int nx,ny,nz;
/*   static int runningcounter=100; */
  
/*   runningcounter++; */

/*   if(runningcounter>100) { */
    stepx = stepy = L/(Real)bn;
    stepz = Lz/(Real)bn;
    Loop(nx, bn) {
      Loop(ny, bn) {
	Loop(nz, bn) {
	  ListEmpty(&box[nx][ny][nz]);
    }}}

    Loop(i,ns) {
      x = S->ball[i].p.x;
      y = S->ball[i].p.y;
      z = S->ball[i].p.z;
      
      nx = (int)(x/stepx);
      ny = (int)(y/stepy);
      nz = (int)(z/stepz);

      ListAppend(&box[nx][ny][nz], i);
    }

/*     runningcounter = 0; */
/*   } */

 } /* else */
}

static Int BestBoxNr(void) {
  int i, boxn;
  Real maxr,minL;

  maxr = 0;
  if(In.radius>0) maxr = In.radius;
  else
    Loop(i,In.ball_t.nt) {
      if(maxr<In.ball_t.radius[i]) maxr=In.ball_t.radius[i];
    }

  minL = (In.l<In.lz)? In.l: In.lz;

  boxn = (int) (minL/(2.1*maxr));

  printf("Optimal BOXN is %d\n", boxn);

  return boxn;
}
