typedef struct element {
  char* key  ;
  int   count;
  struct element* nxt;
} element;
  
typedef struct Queue {
  element* head;
  element* tail;
  int len;
} Queue;

int      Add2Q  (char* key, int count, Queue* q);
element* RdFromQ(Queue* q);  
Queue*   InitQ ();
int      IsEmptyQ(Queue* q);
void     PrintQ(Queue* q);
