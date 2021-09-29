#ifndef __PRIORITY__QUEUE__SAN__
#define __PRIORITY__QUEUE__SAN__
#include <stdio.h>
#include <stdlib.h>

#include "process_properties.h"
void swap(processProperties **x, processProperties **y) {
    processProperties *temp = (*x);
    (*x) = (*y);
    (*y) = temp;
}
void insertIntoPQ(processProperties *n, processProperties **heap, int eleSize, int *s, bool (*cmp)(processProperties *, processProperties *)) {
    int i = (*s), j;
    // heap[i] = malloc(sizeof(processProperties));
    // intializeProperties(heap[i]);
    // copy(n, heap[i]);
    heap[i] = n;
    while (i > 0) {
        j = i;
        i = (i - 1) / 2;
        if (cmp(heap[j], heap[i]))
            swap(&heap[j], &heap[i]);
        else
            break;
    }
    (*s)++;
}
processProperties *extractMinProcess(processProperties **heap, int eleSize, int *s, bool (*cmp)(processProperties *, processProperties *)) {
    if ((*s) == 0)
        return NULL;
    int i = 0, j = 1, k = 2;
    processProperties *n = heap[0];
    heap[0] = heap[(*s) - 1];
    (*s)--;
    while (j < (*s)) {
        if ((!cmp(heap[i], heap[j])) && k < (*s) && (!cmp(heap[i], heap[k]))) {
            if (cmp(heap[j], heap[k])) {
                swap(&heap[j], &heap[i]);
                i = j;
            } else {
                swap(&heap[k], &heap[i]);
                i = k;
            }
        } else if (!cmp(heap[i], heap[j])) {
            swap(&heap[j], &heap[i]);
            i = j;
        } else if (k < (*s) && (!cmp(heap[i], heap[k]))) {
            swap(&heap[k], &heap[i]);
            i = k;
        } else
            break;
        j = 2 * i + 1;
        k = 2 * i + 2;
    }
    return n;
}

void *peekPQ(void **heap) {
    return *(heap);
}
void copyPQ(void **from, void ***to, int s, int eleSize) {
    (*to) = (void **)malloc(s * eleSize);
    for (int i = 0; i < s; i++) {
        *((*to) + i * eleSize) = *(from + i * eleSize);
    }
}
#endif