#include "main.h"
#include "extern.h"

static Real PBC( Real v, Real L);
static void Bounce( Real *z, Real *vz, Real Lz);
static void TotalForces( System *S, Real t, LIST ***box);
static void ExternalForce( Point3D *P, Vector3D *F);
static void NonConservativeForces(Point3D *P, Vector3D *V, Real t, Vector3D *F);
static void TwoCenterForce( System *S, int c, Vector3D *F, LIST ***box);
static void Fp( Vector3D *D, Vector3D *DV, Real r, Vector3D *F);

void VerletDynamics( System *S, Real t, Real dt, LIST ***box) {
  /* Bilateral Verlet Algorithm (BSA1):
       Lapo Casetti, Physica Scripta 51, 29 (1995)
       Eq. (15)
  */
  register int i;
  int NC = S->n;
  Real L = In.l;   /* X,Y edge */
  Real Lz = In.lz; /*  Z  edge */
  Real dt2 = 2.0*dt;
  Ball *B;

  /* First step: q(t+dt)=q(t)+dt*p(t) */
  Loop( i, NC) {
    B = &(S->ball[i]);
    if( B->pinned) continue;
    B->p.x += (dt * B->v.x);
    B->p.y += (dt * B->v.y);
    B->p.z += (dt * B->v.z);

    B->p.x = PBC( B->p.x, L);
    B->p.y = PBC( B->p.y, L);
    Bounce(&(B->p.z), &(B->v.z), Lz);
  }

  /* Simple Verlet follows */
  /* Second step: p(t+dt)  = p(t)   +dt*F[q(t+dt)] */
  TotalForces(S, t, box);

  Loop( i, NC) {
    B = &(S->ball[i]);
    if( B->pinned) continue;
    B->v.x += (dt * B->F.x);
    B->v.y += (dt * B->F.y);
    B->v.z += (dt * B->F.z);
  }

  return;

  /* following seems not to work with friction */

  /* Second step: p(t+dt)  = p(t)   +dt*F[q(t+dt)] */
  /* Third step:  p(t+2dt) = p(t+dt)+dt*F[q(t+dt)] */
  /* practically: p(t+2dt) = p(t) +2*dt*F[q(t+dt)] */

  Loop( i, NC) {
    B = &(S->ball[i]);
    if( B->pinned) continue;
    B->v.x += (dt2 * B->F.x);
    B->v.y += (dt2 * B->F.y);
    B->v.z += (dt2 * B->F.z);
  }

  /* Fourth step: q(t+2dt)=q(t+dt)+dt*p(t+2dt) */
  Loop( i, NC) {
    B = &(S->ball[i]);
    if( B->pinned) continue;
    B->p.x += (dt * B->v.x);
    B->p.y += (dt * B->v.y);
    B->p.z += (dt * B->v.z);

    B->p.x = PBC( B->p.x, L);
    B->p.y = PBC( B->p.y, L);
    Bounce(&(B->p.z), &(B->v.z), Lz);
  }

  return;
}

static void ExternalForces( Point3D *P, Vector3D *F) {
  F->x = F->y = F->z = 0.0;

  /* external conservative */

  /* effect of gravity */
  F->z += -In.gravacc;

/*   F->x += 0.5-P->x; */
/*   F->y += 0.5-P->y; */
  return;
}

static void NonConservativeForces(Point3D *P, Vector3D *V, Real t, Vector3D *F) {
  F->x = F->y = F->z = 0.0;

  if( t>In.frctime) {
    F->x += -In.friction * V->x;
    F->y += -In.friction * V->y;
    F->z += -In.friction * V->z;
  }
  return;
}

static void TwoCenterForce( System *S, int c, Vector3D *F, LIST ***box) {
  /* 2-body force between cell c and the rest */
  register int i;
  int NC = S->n;
  Real L = In.l; /* X,Y edge */
  Real rc = S->ball[c].r; /* ball radius */
  Real xc = S->ball[c].p.x, 
       yc = S->ball[c].p.y,
       zc = S->ball[c].p.z; /* coord of cell c */
  Real vxc = S->ball[c].v.x, 
       vyc = S->ball[c].v.y,
       vzc = S->ball[c].v.z; /* velocity of cell c */
  Vector3D D; /* diff of coord of cell c and i */
  Vector3D DV; /* diff of velocity of cell c and i */
  Real fy, fx, fz, ri;
  Vector3D fp;

    fx = fy = fz = 0.0;

    if(In.boxn<2) {
      Loop(i, NC) {
	if(i==c) continue; /* no interaction with itself */
	D.x = (xc - S->ball[i].p.x);
	D.y = (yc - S->ball[i].p.y);
	D.z = (zc - S->ball[i].p.z);
	ri = S->ball[i].r;
	DV.x = (vxc - S->ball[i].v.x);
	DV.y = (vyc - S->ball[i].v.y);
	DV.z = (vzc - S->ball[i].v.z);

	/* PBC */
	if(D.x>0) while(fabs(D.x)>L/2.) D.x -= L;
	if(D.x<0) while(fabs(D.x)>L/2.) D.x += L;
	if(D.y>0) while(fabs(D.y)>L/2.) D.y -= L;
	if(D.y<0) while(fabs(D.y)>L/2.) D.y += L;
	/* no PBC on Z */

	Fp( &D, &DV, ri+rc, &fp);
	fx += fp.x;
	fy += fp.y;
	fz += fp.z;
      }
    } else {
      int nx,ny,nz;
      int j,k,l,m;
      int nj,nk,nl;
      int bn = In.boxn;
      Real stepx, stepy, stepz;
      int ls;
      LIST *B;

      stepx = stepy = L/(Real)bn;
      stepz = In.lz/(Real)bn;
      nx = (int)(xc/stepx);
      ny = (int)(yc/stepy);
      nz = (int)(zc/stepz);

      for(j=nx-1; j<=nx+1; j++) {
	nj = j; 
	if(nj <0 ) nj += bn; /* PBC on X */
	if(nj>=bn) nj -= bn;
	for(k=ny-1; k<=ny+1; k++) {
	  nk = k; 
	  if(nk <0 ) nk += bn; /* PBC on Y */
	  if(nk>=bn) nk -= bn;
	  for(l=nz-1; l<=nz+1; l++) {
	    if(l<0 || l>=bn) continue; /* no PBC on Z */
	    nl = l;

	    B = &box[nj][nk][nl];
	    ls = ListSize(B);
	    Loop(m, ls) {
	      i = B->list[m];
	      if(i==c) continue; /* no interaction with itself */

	      D.x = (xc - S->ball[i].p.x);
	      D.y = (yc - S->ball[i].p.y);
	      D.z = (zc - S->ball[i].p.z);
	      ri = S->ball[i].r;
	      DV.x = (vxc - S->ball[i].v.x);
	      DV.y = (vyc - S->ball[i].v.y);
	      DV.z = (vzc - S->ball[i].v.z);

	      /* PBC */
	      if(D.x>0) while(fabs(D.x)>L/2.) D.x -= L;
	      if(D.x<0) while(fabs(D.x)>L/2.) D.x += L;
	      if(D.y>0) while(fabs(D.y)>L/2.) D.y -= L;
	      if(D.y<0) while(fabs(D.y)>L/2.) D.y += L;
	      /* no PBC on Z */

	      Fp( &D, &DV, ri+rc, &fp);
	      fx += fp.x;
	      fy += fp.y;
	      fz += fp.z;
	      
	    }

	  }
	}
      }
    }
    F->x = fx;
    F->y = fy;
    F->z = fz;

  return;
}

static void TotalForces( System *S, Real t, LIST ***box) {
  /* actually delivers accelerations */
  register int i;
  Int NC = S->n;
  Ball *B;
  Point3D P;
  Vector3D V;
  Vector3D F;
  Real m;

  Loop( i, NC) {
    B = &(S->ball[i]);
    P.x = B->p.x;
    P.y = B->p.y;
    P.z = B->p.z;
    V.x = B->v.x;
    V.y = B->v.y;
    V.z = B->v.z;
    m   = B->m;

    B->F.x = 0.0;
    B->F.y = 0.0;
    B->F.z = 0.0;

    NonConservativeForces(&P, &V, t, &F);

    B->F.x += F.x;
    B->F.y += F.y;
    B->F.z += F.z;

    TwoCenterForce(S,i, &F, box);

    B->F.x += F.x;
    B->F.y += F.y;
    B->F.z += F.z;

    /* mass effect */
    B->F.x /= m; 
    B->F.y /= m;
    B->F.z /= m;

    ExternalForces(&P, &F);

    B->F.x += F.x;
    B->F.y += F.y;
    B->F.z += F.z;
    
  }

  return;
}

static Real PBC( Real v, Real L) {
  Real shift;

  if(v >= L) {
    shift = (int) (v/L) * L;
    v -=  shift;
  }
  if(v <  0.0) {
    shift = ((int) (v/L) - 1 )* L;
    v -= shift;
  }

  return v;
}

static void Bounce( Real *z, Real *vz, Real Lz) {

  /* bouncing on upper edge */
  if(*z > Lz) {
    *z = Lz * (1.0 + (int) (*z/Lz)) - *z;
    *vz = -*vz;
  }
  /* bouncing on lower edge */
  if(*z < 0.0) {
    *z = ((int) (*z/Lz))* Lz - *z;
    *vz = 0.0; /* sticky */
  }

  return;
}

static void Fp( Vector3D *D, Vector3D *DV, Real r, Vector3D *F) {
  /* two center force */

  Real d2 = D->x*D->x + D->y*D->y + D->z*D->z;
  Real r2 = r*r;
  Real d, vd, tmp;
  Vector3D Vn;
  const Real K = 1e4;
  const Real gamma = 30.0;

  if(d2 >= r2) { F->x = F->y = F->z = 0.0; return; }


/* ad hoc */
/*   tmp = (1./d2 - 1./r2); */
/*   F->x = tmp * D->x; */
/*   F->y = tmp * D->y; */
/*   F->z = tmp * D->z; */
/*   return; */


  d = sqrt(d2);

/* spring */
  tmp = K*(r-d)/d;


  F->x = tmp * D->x;
  F->y = tmp * D->y;
  F->z = tmp * D->z;

  /* radial velocity difference */
  vd =  DV->x * D->x +  DV->y * D->y + DV->z * D->z;
  Vn.x = vd * D->x /d2;
  Vn.y = vd * D->y /d2;
  Vn.z = vd * D->z /d2;

  F->x -= gamma * Vn.x;
  F->y -= gamma * Vn.y;
  F->z -= gamma * Vn.z;

  
  return;
}
