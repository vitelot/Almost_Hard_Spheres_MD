#include <time.h>

#ifdef GSL

#include <gsl/gsl_rng.h>

static gsl_rng * r;

void InitRandom(void) {
  /*
gsl_rng_mt19937 is used as the default. 
The initial value of gsl_rng_default_seed is zero
  */
  const gsl_rng_type * T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  gsl_rng_set (r, time(NULL)); /* seed */
}


double Rand(void) {
/*   printf("%p\n", (void *) r); */

  return gsl_rng_uniform (r); /* [0,1) */
}

unsigned long int RandN(unsigned long int N) {

  return gsl_rng_uniform_int(r, N); /* [0, N-1] */

}

void FreeRandom(void) {

  gsl_rng_free (r);
}

/*
REFERENCES:
http://www.gnu.org/software/gsl/manual/html_node/Random-number-environment-variables.html
http://www.gnu.org/software/gsl/manual/html_node/Random-number-generator-initialization.html
http://www.gnu.org/software/gsl/manual/html_node/Sampling-from-a-random-number-generator.html
http://www.gnu.org/software/gsl/manual/html_node/Random-number-generator-algorithms.html
*/

#else

#include <stdlib.h>
#include <stdio.h>

void InitRandom(void) {
  srand(time(NULL)); /* seed */
  fprintf(stderr, 
	  "WARNING: you are using rand() as random number generator!\n"
	  "Better if you install the Gnu Scientific Libraries soon.\n\n");
}


double Rand(void) {

  return rand()/(1.0+RAND_MAX); /* [0,1) */
}

unsigned long int RandN(unsigned long int N) {

  return N*Rand(); /* [0, N-1] */

}

void FreeRandom(void) {

  return;
}


#endif
