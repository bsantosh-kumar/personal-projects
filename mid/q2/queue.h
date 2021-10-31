#ifndef __QUEUE__SAN__
#define __QUEUE__SAN__
#include <stdio.h>
#include <stdlib.h>

#include "process_properties.h"
struct llNode_t {
    processProperties *data;
    struct llNode_t *next;
};
typedef struct llNode_t llNode;
typedef llNode *lptr;
void insert_back(lptr *P, processProperties *n) {
    lptr T = (lptr)malloc(sizeof(llNode));
    T->data = n;
    T->next = (*P)->next;
    (*P)->next = T;
    (*P) = T;
}
processProperties *del_front(lptr *P) {
    processProperties *ans = NULL;
    if ((*P) != NULL) {
        ans = (*P)->data;
        lptr temp = (*P);
        (*P) = (*P)->next;
        free(temp);
    }
    return ans;
}
struct queue_t {
    lptr f;   // front
    lptr r;   // rear
    int nfe;  // no of elements
};
typedef struct queue_t queue;
void intializeQueue(queue *q) {
    q->f = NULL;
    q->r = NULL;
    q->nfe = 0;
}
void enqueue(queue *q, processProperties *ele) {
    if (q->f == NULL) {
        lptr T = (lptr)malloc(sizeof(llNode));
        T->data = ele;
        T->next = T;
        q->f = q->r = T;
        q->nfe++;
        return;
    }
    insert_back(&(q->r), ele);
    q->nfe++;
}
processProperties *dequeue(queue *q) {
    if (q->f == NULL) return NULL;
    q->r->next = q->f->next;
    processProperties *ans = del_front(&(q->f));
    if (q->f == NULL) {
        lptr temp = q->r;
        q->r = NULL;
        free(temp);
    }
    if (ans != NULL) q->nfe--;
    return ans;
}
void translate(queue *q) {
    if (q->f == NULL) return;
    q->f = q->f->next;
    q->r = q->r->next;
}
processProperties *getFront(queue *q) {
    if (q->f == NULL) return NULL;
    return q->f->data;
}
#endif
