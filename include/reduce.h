typedef struct RdElement {
  char* key;
  int count;
} RdElement;

RdElement* InitRdArr();
void Reduce(Queue* q, int done, RdElement* RdArr, int q_index);
void PrintRdArr(RdElement* RdArr);
