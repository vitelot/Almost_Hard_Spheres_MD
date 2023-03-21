#include "main.h"
#include "extern.h"

Int *IntVectorAlloc( int dim)
{
  Int *out;

  out = (Int *) calloc( (size_t) dim, sizeof(Int));

  if(!out) {
    perr("Allocation error in IntVectorAlloc. Exit.\n");
    exit(1);
  }
  return out;
}

Int *IntVectorRealloc( Int *v, int newdim)
{
/*      newlist = IntVectorAlloc( d+ALLOCSTEP); */
/*      IntVectorCopy( newlist, queue->list, d); */
/*      IntVectorFree( queue->list); */
/*      queue->list = newlist; */

  Int *out;

  /* newdim++;  better one location more */
  out = (Int *) realloc( (void *) v, (size_t) newdim*sizeof(Int));

  if(!out) {
    perr("Reallocation error in IntVectorRealloc. Exit.\n");
    exit(1);
  }
  return out;
}

void IntVectorFree( Int *v)
{ 
  free( (Int *) v);
}

void IntVectorInit( Int *v, Int dim, Int value)
{
  register Int i;
  for( i=0; i<dim; i++)
    v[i] = value;
}

Int IntVectorIntersect( Int *a, int dima, Int *b, int dimb)
{ /* Returns how many elements of a are contained in b */
  register int i,j;
  Int count=0;

  Loop(i,dima) {
    Loop(j,dimb) {
      if( a[i] == b[j]) { ++count; break; }
    }
  }
  return count;
}

void IntVectorCopy( Int *a, Int *b, int dim)
{
  /* copy b onto a */
  register Int i;

  for( i=0; i<dim; i++)
    a[i] = b[i];
}
