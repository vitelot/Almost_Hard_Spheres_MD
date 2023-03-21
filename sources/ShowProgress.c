#include <stdio.h>
void ShowProgress(float fraction, float total) {
  float percent =  (fraction/total*100);
  /*  if(percent%5==0) {*/
    printf("   %.2f%%\r", percent);
    fflush(stdout);
    /* } */
}
