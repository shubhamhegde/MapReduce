#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Header files
#include "map_red_q.h"
#include "map.h"
#include "reduce.h"

inline int proc_for_i (int i) {
  return floor((red_Numthreads*(i+1)-1)/HASH_ARRAY_LEN);
}

int hash_f(char* key) {
  int val=0;
  int i=0;
  while(*(key+i) != '\0') {
    val = val ^ ((int)(*(key+i)));
    i=i+1;
  }
  return val;
}

int main() {
  int        i, j, k              ;
  int        mapper_done, h       ;
  int*       map_done = (int*) malloc(sizeof(int)*HASH_ARRAY_LEN);
  
  element*   E = (element*) malloc(sizeof(element)*HASH_ARRAY_LEN); // maping based on hashing function
  Queue*     Q_map[map_Numthreads]; // Maping Queue
  Queue*     Q_red[red_Numthreads]; // Reducing Queue
  element*   e = (element *)malloc(sizeof(element));
  element*   rde = (element *)malloc(sizeof(element));
  element*   e_ll = (element *)malloc(sizeof(element));
  RdElement* RdArrs[red_Numthreads];
  
  char       words[][20]   = {"This", "eBook", "is", "for", "the", "use", "si", "anyone", "anywhere", "at"};

  double execTime = -omp_get_wtime();
  //======================================================================================
  // Initialize
  //======================================================================================
  mapper_done = 0;

  for(i=0; i<map_Numthreads; i++)
    map_done[i] = 0;

  for(i=0; i<map_Numthreads; i++)
    Q_map[i] = InitQ();

  for(i=0; i<red_Numthreads; i++)
    Q_red[i] = InitQ();

  for(i=0; i<HASH_ARRAY_LEN; i++) {
    (E+i)->count = 0;
    (E+i)->key   = NULL;
    (E+i)->nxt   = NULL;
  }
  
  for(i=0; i<map_Numthreads; i++) {
    for(j=0; j<10; j++) {
      Add2Q(words[j], 1, Q_map[i]);
    }
  }

  for(i=0; i<HASH_ARRAY_LEN; i++) {
    omp_init_lock(&(lck[i]));
  }
  for(i=0; i<red_Numthreads; i++) {
    omp_init_lock(&(rdq_lck[i]));
  }

  //initializing red_Numthreads Reducer arrays
  for(k=0; k<red_Numthreads; k++) {
    RdArrs[k] = InitRdArr();
  }

  // for(i=0; i<map_Numthreads; i++)
  //   PrintQ(Q_map[i]);
  for(i=0; i<10; i++)
    printf("%s - %d\n", words[i], hash_f(words[i]));
  printf("\n");

  //======================================================================================
  // MAPPER + COMBINER
  //======================================================================================
  #pragma omp parallel sections
  {
    #pragma omp section
    {
      #pragma omp parallel for
      for(i=0; i<map_Numthreads; i=i+1) {
        while(!IsEmptyQ(Q_map[i])) {
          //----------------------------------------------------------
          // Read from (Reader <--> Mapper) Q & put into an array
          //----------------------------------------------------------
          for(j=0; j<red_block_size; j++) {
            e = RdFromQ(Q_map[i]);
            h = hash_f(e->key);
            e_ll = (E+h);
 
            omp_set_lock(&(lck[h])); // lock for each hash element
            while(1) {
              if(e_ll->key == e->key) {
                (e_ll->count) = (e_ll->count) + 1;
                break;
              } else if(e_ll->key == NULL) {
                (e_ll->count) = (e_ll->count) + 1;
                e_ll->key = e->key;
                break;
              } else if(e_ll->nxt == NULL) {
                e_ll->nxt = (element *)malloc(sizeof(element));
                e_ll = e_ll->nxt;
                e_ll->count = 1;
                e_ll->key = e->key;
                e_ll->nxt = NULL;
                break;
              } else {
                e_ll = e_ll->nxt;
              }
            }
            omp_unset_lock(&(lck[h]));

            if(IsEmptyQ(Q_map[i])) break;
          }
        }
        map_done[i] = 1;
      }
          //----------------------------------------------------------
          // Map an array to a Mapper thread TODO should be part of map_Numthreads loop
          //----------------------------------------------------------
          for(j=0; j<HASH_ARRAY_LEN; j=j+1) {
            rde=(E+j);
            if(rde->count != 0) {
              omp_set_lock(&rdq_lck[proc_for_i(j)]);
              Add2Q(rde->key, rde->count, Q_red[proc_for_i(j)]);
              omp_unset_lock(&rdq_lck[proc_for_i(j)]);
              rde->count = 0;
            }
            while(rde->nxt != NULL) {
              rde = rde->nxt;
              if(rde->count != 0) {
                omp_set_lock(&rdq_lck[proc_for_i(j)]);
                Add2Q(rde->key, rde->count, Q_red[proc_for_i(j)]);
                omp_unset_lock(&rdq_lck[proc_for_i(j)]);
                rde->count = 0;
              }
            }
          }

      for(i=0; i<red_Numthreads; i++)
        PrintQ(Q_red[i]);
    
      //TODO add a barrier
      mapper_done = 1;
 
    }

    #pragma omp section
    {
      
      //======================================================================================
      // REDUCER
      //======================================================================================
      #ifdef DEBUG_M
      //priniting after initialization
      for(k=0; k<red_Numthreads; k++)
        PrintRdArr(RdArrs[k]);
      #endif
    
      //mapper_done is the single done signal from mapper
      #pragma omp parallel for
      for(k=0; k<red_Numthreads; k++)
        Reduce(Q_red[k], mapper_done, RdArrs[k], k);
    
    }
  }
  execTime += omp_get_wtime();
  //priniting after reduction
  for(k=0; k<red_Numthreads; k++)
    PrintRdArr(RdArrs[k]);
  printf("\n==========================================\n");
  printf("\tExecution Time = %lf\n", execTime);
  printf("==========================================\n");

  return 0;
}
