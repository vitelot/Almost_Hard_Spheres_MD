#ifndef __MAIN_H
# define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Version.h"

#define Loop(i,N) for( (i)=0; (i)<(N); ++(i))
#define Vector3D Point3D

#define P(x) printf("### check point %d ###\n", (x));  /* for debugging */

typedef double Real;
typedef int Int;
typedef enum {FALSE, TRUE} Bool;

typedef struct {
  Real x;
  Real y;
  Real z;
} Point3D;

typedef struct {
  char type;
  Point3D p; /* position */
  Vector3D v; /* velocity */
  Vector3D F; /* Force */
  Real r; /* radius */
  Real m; /* mass */
  Bool pinned;
} Ball;

typedef struct {
  int n;
  Ball *ball;
} System; /* set of n balls */

typedef struct {
  int nt; /* nr of sorts */
  char *type; /* list of nt types */
  Real *abundance; /* relative abundance */
  Real *radius; /*  */
  Real *mass; /* */
} Ball_types; /* list of ball sorts */

struct IntList {
  Int Dim; /* Allocated dimension of the list */
  Int n; /* total nr of elements in the list */
  Int *list; /* element list */
};

typedef struct IntList LIST;

struct Input {
  int nc;
  double l;
  double lz;
  double dt;
  double tmax;
  double radius;
  double v0;
  Ball_types ball_t;
  double gravacc;
  double pinlvl;
  double friction;
  double frctime;
  double cooling;
  double cooltime;
  int boxn;
  char projname[128];
  int savestep;
  int track;
  int kinE;
  int printdat;
  int rasmol;
  char pngshow[128];
  int rungnuplot;
};

#endif
