#include<stdio.h>
#include<stdlib.h>
#include "map_red_q.h"
#include "reduce.h"
#include "map.h"


//#define DEBUG_M

RdElement* InitRdArr() {
  RdElement* RdArr = (RdElement*) malloc(sizeof(RdElement)*HASH_ARRAY_LEN);
  if( RdArr == NULL)
    {
      //malloc failed
      printf("Reduce Array could not be allocated\n");
    }
  int i;
  for(i=0; i<HASH_ARRAY_LEN; i++)
    {
      (RdArr+i)->key = NULL;
      (RdArr+i)->count = 0; 
    }
  return RdArr;
}

void Reduce(Queue* q, int done, RdElement* RdArr, int q_index) {  
  while( !(done && IsEmptyQ(q)))
    {
      omp_set_lock(&rdq_lck[proc_for_i(q_index)]);
      element* re = RdFromQ(q);
      omp_unset_lock(&rdq_lck[proc_for_i(q_index)]);
      int index = hash_f(re->key);
      #ifdef DEBUG_M
      printf("%d \n", q->len);
      if(re->key != NULL)
	printf("<< %s >>", re->key);
      #endif
      (RdArr+index)->count += re->count;
      if((RdArr+index)->key == NULL)
	{
	  (RdArr+index)->key = re->key;
	}
    }
}

void PrintRdArr(RdElement* RdArr) {
  int i;
  printf("----------------------------------\n");
  for(i=0; i<HASH_ARRAY_LEN; i++)
    {
      if((RdArr+i)->key != NULL)
	printf("(%s, %d)\n", (RdArr+i)->key, (RdArr+i)->count);
    }
}

/* int main() { */
/*   // Reducer */
/*   //Assuming red_Numthreads number of Reducers */
/*   RdElement* RdArrs[red_Numthreads]; */
 
/*   //initializing red_Numthreads Reducer arrays */
/*   int k; */
/*   for(k=0; k<red_Numthreads; k++) */
/*     RdArrs[k] = InitRdArr(); */
  
/*   //priniting after initialization */
/*   for(k=0; k<red_Numthreads; k++) */
/*     PrintRdArr(RdArrs[k]); */

/*   //mapper_done is the single done signal from mapper */
/*   for(k=0; k<red_Numthreads; k++) */
/*     Reduce(Q_red[k], mapper_done, RdArrs[k]); */

/*   /\* //priniting after reduction *\/ */
/*   /\* for(k=0; k<red_Numthreads; k++) *\/ */
/*   /\*   PrintRdArr(RdArrs[k]); *\/ */

 
/* } */

