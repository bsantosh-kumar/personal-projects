#define M 7
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define swap(x, y)        \
    ({                    \
        typeof(x) _x = x; \
        typeof(y) _y = y; \
        x = _y;           \
        y = _x;           \
    })

struct processProperties_t {
    int pid;               //process id
    int at;                //arrival time
    int bt;                //burst time
    int frt;               //first response time
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
    currProcess->frt = 0;
    currProcess->ct = 0;
    currProcess->tt = 0;
    currProcess->wt = 0;
    currProcess->allProperties[0] = &(currProcess->pid);
    currProcess->allProperties[1] = &(currProcess->at);
    currProcess->allProperties[2] = &(currProcess->bt);
    currProcess->allProperties[3] = &(currProcess->frt);
    currProcess->allProperties[4] = &(currProcess->ct);
    currProcess->allProperties[5] = &(currProcess->tt);
    currProcess->allProperties[6] = &(currProcess->wt);
}
void copy(processProperties *from, processProperties *to) {
    for (int i = 0; i < M; i++) {
        *(to->allProperties[i]) = *(from->allProperties[i]);
    }
}