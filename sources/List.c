#include "main.h"
#include "extern.h"

void ListInit(  LIST *l)
{
     l->n = 0; /* initialize n */
     l->Dim = 0;
     l->list = NULL;
}

void ListFree(  LIST *list)
{
  list->n=0;
  list->Dim=0;
  IntVectorFree( list->list);
  list->list = NULL;
}

void ListEmpty ( LIST *l) {
    l->n=0;
}

Int ListSize( LIST * list)
{ return list->n; }


#define ALLOCSTEP 100

void ListAppend( LIST * list, Int value)
{
  Int n, dim;

  n = list->n;
  dim = list->Dim;
  if( n == 0 && dim==0) {
    list->list = IntVectorAlloc( ALLOCSTEP);
    list->Dim = ALLOCSTEP;
  } else if( n >= dim-1 ) {
    list->list = IntVectorRealloc( list->list, dim+ALLOCSTEP);
    list->Dim += ALLOCSTEP;
  }

  list->list[n] = value;
  ++(list->n);
}

void ListAppendList( LIST * L1, LIST * L2)
     /* append List L2 into List L1 */
{
  register int i;
  Int n1,n2;
  Int d1, newd1;

  n2 = L2->n;
  if(n2==0) return;

  n1 = L1->n;
  d1 = L1->Dim;
  
  if( n1+n2 > d1) {
    newd1 = ALLOCSTEP * ((n1+n2)/ALLOCSTEP) + ALLOCSTEP;
    L1->list = IntVectorRealloc( L1->list, newd1);
    L1->Dim = newd1;
  }

  for( i=0; i<n2; i++) 
    L1->list[n1+i] = L2->list[i];
  L1->n += n2;
}

#undef ALLOCSTEP

