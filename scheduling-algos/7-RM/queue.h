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
    T->next = NULL;
    if ((*P) == NULL) {
        (*P) = T;
        return;
    }
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
    lptr f;   //front
    lptr r;   //rear
    int nfe;  //no of elements
};
typedef struct queue_t queue;
void intializeQueue(queue *q) {
    q->f = NULL;
    q->r = NULL;
    q->nfe = 0;
}
void enqueue(queue *q, processProperties *ele) {
    insert_back(&(q->r), ele);
    if (q->f == NULL) {
        q->f = q->r;
    }
    q->nfe++;
}
processProperties *dequeue(queue *q) {
    processProperties *ans = del_front(&(q->f));
    if (q->f == NULL) {
        q->r = NULL;
    }
    if (ans != NULL) q->nfe--;
    return ans;
}
processProperties *peekQueue(queue *q) {
    if (q->f)
        return q->f->data;
    return 0;
}
int getLength(queue *q) {
    return q->nfe;
}
void copyQueue(processProperties ***p, queue *q) {
    int len = getLength(q);
    (*p) = (processProperties **)malloc(len * sizeof(processProperties *));
    lptr T = q->f;
    for (int i = 0; i < len; i++) {
        copyProcess(&(*p)[i], &(T->data));
        T = T->next;
    }
}
#endif