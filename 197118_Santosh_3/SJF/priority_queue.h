#define M 7
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define swap(x, y)        \
    do {                  \
        \ 
   typeof(x) _x = x;      \
        typeof(y) _y = y; \
        x = _y;           \
        y = _x;           \
    } while (0)

struct processProperties_t {
    int pid;               //process id
    int at;                //arrival time
    int bt;                //burst time
    int rt;                //response time
    int ct;                //completion time
    int tt;                //turn around time
    int wt;                //waiting time
    int *allProperties[M]; /* The purpose for this array is just for index-wise accessing of the properties */
};
typedef struct processProperties_t processProperties;
void intializeProperties(processProperties *currProcess) {
    currProcess->pid = 0;
    currProcess->at = 0;
    currProcess->bt = 0;
    currProcess->rt = 0;
    currProcess->ct = 0;
    currProcess->tt = 0;
    currProcess->wt = 0;
    currProcess->allProperties[0] = &(currProcess->pid);
    currProcess->allProperties[1] = &(currProcess->at);
    currProcess->allProperties[2] = &(currProcess->bt);
    currProcess->allProperties[3] = &(currProcess->rt);
    currProcess->allProperties[4] = &(currProcess->ct);
    currProcess->allProperties[5] = &(currProcess->tt);
    currProcess->allProperties[6] = &(currProcess->wt);
}
void copy(processProperties *from, processProperties *to) {
    for (int i = 0; i < M; i++) {
        *(to->allProperties[i]) = *(from->allProperties[i]);
    }
}

void insertIntoPQ(processProperties *n, processProperties **heap, int *s, bool *cmp(processProperties *, processProperties *)) {
    int i = (*s), j;
    heap[i] = malloc(sizeof(processProperties));
    intializeProperties(heap[i]);
    copy(n, heap[i]);
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
processProperties *extractMinProcess(processProperties **heap, int *s, bool *cmp(processProperties *, processProperties *)) {
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
