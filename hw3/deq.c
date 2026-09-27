#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deq.h"
#include "error.h"

// indices and size of array of node pointers
typedef enum {Head,Tail,Ends} End;

typedef struct Node {
  struct Node *np[Ends];        // next/prev neighbors
  Data data;
} *Node;

typedef struct {
  Node ht[Ends];                // head/tail nodes
  int len;
} *Rep;

static Rep rep(Deq q) {
  if (!q) ERROR("zero pointer");
  return (Rep)q;
}

static void put(Rep r, End e, Data d) {
  Node n = malloc(sizeof(*n));
  if (!n) ERROR("malloc() failed");
  n->data = d;
  if (r->len == 0){             // account for empty list
    n->np[Head] = NULL;
    n->np[Tail] = NULL;
    r->ht[Head] = n;
    r->ht[Tail] = n;
  } else if (e == Head) {       // putting at head
    n->np[Head] = NULL;
    n->np[Tail] = r->ht[Head];
    r->ht[Head]->np[Head] = n;
    r->ht[Head] = n;
  } else {                      // putting at tail
    n->np[Tail] = NULL;
    n->np[Head] = r->ht[Tail];
    r->ht[Tail]->np[Tail] = n;
    r->ht[Tail] = n;
  }
  r->len++;
}
static Data ith(Rep r, End e, int i)  {
  if (r->len == 0 || i >= r->len || i < 0) { // check some base cases (0 length, i out of bounds)
    return 0;
  }
  int counter = 0;
  if (e == Head) {              // handle starting from head for ith element
    for (Node n=r->ht[Head]; n; n=n->np[Tail]) {
      if (counter == i) {
        return n->data;
      }
      counter++;
    }
  } else {                      // handle starting from tail for ith elemetn
    for (Node n=r->ht[Tail]; n; n=n->np[Head]) {
      if (counter == i) {
        return n->data;
      }
      counter++;
    }
  }
  return 0;
}
static Data get(Rep r, End e)         {
  if (r->len == 0){             // handling empty deq, return 0
    return 0;
  }
  Data d;
  if (r->len == 1) {            // length 1 deq, handle making the deq empty
    Node n = r->ht[Head];
    d = n->data;
    r->ht[Head] = NULL;
    r->ht[Tail] = NULL;
    free(n);
  } else if (e == Head) {       // handling getting at head
    Node n = r->ht[Head];
    d = n->data;
    r->ht[Head] = n->np[Tail];
    r->ht[Head]->np[Head] = NULL;
    free(n);
  } else {                      // handling getting at tail
    Node n = r->ht[Tail];
    d = n->data;
    r->ht[Tail] = n->np[Head];
    r->ht[Tail]->np[Tail] = NULL;
    free(n);
  }
  r->len--;
  return d; 
}
static Data rem(Rep r, End e, Data d) {
  if (r->len == 0){ // make sure length is greater than 0
    return 0;
  }
  
  // These are used to set up the for loop because the actions whether starting at left or right are symmetric
  // and we don't have to repeat them, we just have to do a little set up work for the loop.
  Node nStart; 
  int to;
  if (e == Head) {
    nStart = r->ht[Head];
    to = Tail;
  } else {
    nStart = r->ht[Tail];
    to = Head;
  }

  // Looping through the linked list
  for (Node n=nStart; n; n=n->np[to]){
    if (n->data == d){
      if (n->np[Head] && n->np[Tail]) { // middle
        n->np[Head]->np[Tail] = n->np[Tail];
        n->np[Tail]->np[Head] = n->np[Head];
      } else if (n->np[Head]) { // at tail
        r->ht[Tail] = n->np[Head];
        n->np[Head]->np[Tail] = NULL;
      } else if (n->np[Tail]) { // at head
        r->ht[Head] = n->np[Tail];
        n->np[Tail]->np[Head] = NULL;
      } else {                // single element
        r->ht[Head] = NULL;
        r->ht[Tail] = NULL;
      }
      n->np[Head] = NULL;     // finally free n
      n->np[Tail] = NULL;
      free(n);
      r->len--;
      return d;
    }
  }
  return 0;
}

extern Deq deq_new() {
  Rep r=(Rep)malloc(sizeof(*r));
  if (!r) ERROR("malloc() failed");
  r->ht[Head]=0;
  r->ht[Tail]=0;
  r->len=0;
  return r;
}

extern int deq_len(Deq q) { return rep(q)->len; }

extern void deq_head_put(Deq q, Data d) {        put(rep(q),Head,d); }
extern Data deq_head_get(Deq q)         { return get(rep(q),Head);   }
extern Data deq_head_ith(Deq q, int i)  { return ith(rep(q),Head,i); }
extern Data deq_head_rem(Deq q, Data d) { return rem(rep(q),Head,d); }

extern void deq_tail_put(Deq q, Data d) {        put(rep(q),Tail,d); }
extern Data deq_tail_get(Deq q)         { return get(rep(q),Tail);   }
extern Data deq_tail_ith(Deq q, int i)  { return ith(rep(q),Tail,i); }
extern Data deq_tail_rem(Deq q, Data d) { return rem(rep(q),Tail,d); }

extern void deq_map(Deq q, DeqMapF f) {
  for (Node n=rep(q)->ht[Head]; n; n=n->np[Tail])
    f(n->data);
}

extern void deq_del(Deq q, DeqMapF f) {
  if (f) deq_map(q,f);
  Node curr=rep(q)->ht[Head];
  while (curr) {
    Node next=curr->np[Tail];
    free(curr);
    curr=next;
  }
  free(q);
}

extern Str deq_str(Deq q, DeqStrF f) {
  char *s=strdup("");
  for (Node n=rep(q)->ht[Head]; n; n=n->np[Tail]) {
    char *d=f ? f(n->data) : n->data;
    char *t; asprintf(&t,"%s%s%s",s,(*s ? " " : ""),d);
    free(s); s=t;
    if (f) free(d);
  }
  return s;
}
