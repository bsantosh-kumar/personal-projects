#include <stdio.h>

void swap(void **x, void **y) {
    void *temp = (*x);
    (*x) = (*y);
    (*y) = temp;
}
void insertIntoPQ(void *n, void **heap, int eleSize, int *s, bool *cmp(void *, void *)) {
    int i = (*s), j;
    *(heap + i * eleSize) = n;
    while (i > 0) {
        j = i;
        i = (i - 1) / 2;
        if (cmp(*(heap + j * eleSize), *(heap + i * eleSize)))
            swap((heap + j * eleSize), (heap + i * eleSize));
        else
            break;
    }
    (*s)++;
}
void *extractMinProcess(void **heap, int eleSize, int *s, bool *cmp(void *, void *)) {
    if ((*s) == 0)
        return NULL;
    int i = 0, j = 1, k = 2;
    void *n = *(heap + 0 * eleSize);
    *(heap + 0 * eleSize) = *(heap + ((*s) - 1) * eleSize);
    (*s)--;
    while (j < (*s)) {
        if ((!cmp(*(heap + i * eleSize), *(heap + j * eleSize))) && k < (*s) && (!cmp(*(heap + i * eleSize), *(heap + k * eleSize)))) {
            if (cmp(*(heap + j * eleSize), *(heap + k * eleSize))) {
                swap((heap + j * eleSize), (heap + i * eleSize));
                i = j;
            } else {
                swap((heap + k * eleSize), (heap + i * eleSize));
                i = k;
            }
        } else if (!cmp(*(heap + i * eleSize), *(heap + j * eleSize))) {
            swap((heap + j * eleSize), (heap + i * eleSize));
            i = j;
        } else if (k < (*s) && (!cmp(*(heap + i * eleSize), *(heap + k * eleSize)))) {
            swap((heap + k * eleSize), (heap + i * eleSize));
            i = k;
        } else
            break;
        j = 2 * i + 1;
        k = 2 * i + 2;
    }
    return n;
}
