#include <omp.h>

#define map_Numthreads 10
#define red_Numthreads 10
#define red_block_size 1000

#define HASH_ARRAY_LEN 256

omp_lock_t lck[HASH_ARRAY_LEN];
omp_lock_t rdq_lck[red_Numthreads];

inline int proc_for_i (int i) __attribute__((always_inline));

int hash_f(char* key);


