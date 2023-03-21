#ifndef __EXTERN_H
#  define __EXTERN_H

#include "main.h"

/************************ Global variables ************************/
extern FILE *fperr; /* file pointer for error messages */
extern struct Input In; /* Input parameters */
/******************************************************************/


/* System.c */
extern void SystemAlloc(System *B, int nc);
extern void SystemRealloc(System *B, int NC);
extern void SystemFree ( System *B);
extern void SystemCopy(System *dest, System *source);
extern void SystemSwap( System *A, System *B);
extern void SystemInit ( System *B);
extern void SystemPrint( System *B, const char *projname);
extern Real SystemPackingRatio(System *S);

/*  IntList.c */
extern void ListInit(       LIST *l);
extern void ListFree(       LIST *l);
extern void ListAppend(     LIST *l, Int value);
extern void ListAppendList( LIST * L1, LIST * L2);
extern Int  ListSize(       LIST *l);
extern void ListEmpty(      LIST *l);

/*  IntVector.c */
extern int *IntVectorAlloc( int dim);
extern int *IntVectorRealloc( int *v, int newdim);
extern void IntVectorFree( int *v);
extern void IntVectorInit( int *v, int dim, int value);
extern int  IntVectorIntersect( int *a, int dima, int *b, int dimb);
extern void IntVectorCopy( int *a, int *b, int dim);

/* Boxes.c */
extern LIST *** BoxAlloc(void);
extern void BoxInit(LIST ***box);
extern void BoxFree(LIST ***box);
extern void BoxAssign(System *S, LIST ***box);

/*  RealVector.c */
/* extern Real *RealVectorAlloc( int dim); */
/* extern Real *RealVectorRealloc( Real *v, int newdim); */
/* extern void  RealVectorFree( Real *v); */
/* extern void  RealVectorInit( Real *v, int dim, Real value); */
/* extern void  RealVectorCopy( Real *a, Real *b, int dim); */

/*  Error.c */
extern void perr(const char *fmt, ...);
extern void OpenError(char *errfilename);
extern void CloseError(void);
extern void AllocError( char *s);
extern void FileOpenError( char *s);

/*  Input.c */
extern void InputParameters( char *ini_file);
extern void SetBallTypes(void);

/*  ShowProgress.c */
extern void ShowProgress(float fraction, float total);

/* File.c */
extern int FileExists(char *);

#endif
