#include <stdio.h>
#include <stdlib.h>
#include "map_red_q.h"

int Add2Q(char* key, int count, Queue* q) {
  element* e = (element *)malloc(sizeof(element));
  if(e == NULL) {
    return 0;
  }
  e->key = key;
  e->count = count;
  e->nxt = NULL;
  if( q->len == 0 )
    {
      q->tail = e;
      q->head = e;
    }
  else
    {
      q->tail->nxt = e;
      q->tail = e;
    }
  q->len +=1;
  return 1;
}

element* RdFromQ(Queue* q) {
  element* e = q->head;
  if( q->len == 1)
    {
      //only one element in the queue & that is being read
      q->head = NULL;
      q->tail = NULL;
    }
  else
    {
      q->head = q->head->nxt;
    }  
  q->len -=1;
return e;
} 

int IsEmptyQ(Queue* q) {
  if(q->len == 0)
    return 1;
  return 0;

}

Queue* InitQ() {
  Queue* q = (Queue*) malloc(1*sizeof(Queue));
  q->head = NULL;
  q->tail = NULL;
  q->len = 0;
  return q;
}

void PrintQ(Queue* q) {
  //printf("%d", q->len);
  if(q->len == 0) 
    {
      printf("Queue Empty! \n");
    }
  else 
    {
      element* p = q->head;
      while(p != NULL)
	{
	  printf("(%s, %d)->", p->key, p->count);
	  p = p->nxt;
	}
      printf("\n");
    }
} 

/*
//Just for checking
int main() {
  Queue* q = InitQ();
  PrintQ(q);
  char a[] = "abc";
 
  Add2Q( a, 2, q);
  PrintQ(q);
  Add2Q( a, 1, q);
  PrintQ(q);
  RdFromQ(q);
  PrintQ(q);
  printf("Is Empty: %d \n", IsEmptyQ(q));
  RdFromQ(q);
  PrintQ(q);
  printf("Is Empty: %d \n", IsEmptyQ(q));
}
*/
