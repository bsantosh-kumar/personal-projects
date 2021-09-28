#include "process_properties.h"

void insertIntoPQ(processProperties *n, processProperties **heap, int *s, bool (*cmp)(processProperties *, processProperties *)) {
    int i = (*s), j;
    // heap[i] = malloc(sizeof(processProperties));
    // intializeProperties(heap[i]);
    // copy(n, heap[i]);
    heap[i] = n;
    while (i > 0) {
        j = i;
        i = (i - 1) / 2;
        if (cmp(heap[j], heap[i]))
            swap(heap[j], heap[i]);
        else
            break;
    }
    (*s)++;
}
processProperties *extractMinProcess(processProperties **heap, int *s, bool (*cmp)(processProperties *, processProperties *)) {
    if ((*s) == 0)
        return NULL;
    int i = 0, j = 1, k = 2;
    processProperties *n = heap[0];
    heap[0] = heap[(*s) - 1];
    (*s)--;
    while (j < (*s)) {
        if ((!cmp(heap[i], heap[j])) && k < (*s) && (!cmp(heap[i], heap[k]))) {
            if (cmp(heap[j], heap[k])) {
                swap(heap[j], heap[i]);
                i = j;
            } else {
                swap(heap[k], heap[i]);
                i = k;
            }
        } else if (!cmp(heap[i], heap[j])) {
            swap(heap[j], heap[i]);
            i = j;
        } else if (k < (*s) && (!cmp(heap[i], heap[k]))) {
            swap(heap[k], heap[i]);
            i = k;
        } else
            break;
        j = 2 * i + 1;
        k = 2 * i + 2;
    }
    return n;
}
