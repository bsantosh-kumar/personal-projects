#include <stdio.h>

#include "queue.h"
int main() {
    queue *q = malloc(sizeof(queue));
    intializeQueue(q);
    processProperties *p1 = malloc(sizeof(processProperties)), *p2 = malloc(sizeof(processProperties));
    intializeProperties(p1);
    intializeProperties(p2);
    p1->pid = 1;
    p2->pid = 2;
    enqueue(q, p1);
    printf("q->nfe=%d\n", q->nfe);
    processProperties *p3 = dequeue(q);
    if (p3 == p1) {
        printf("Same\n");
    }
    p3 = dequeue(q);
}