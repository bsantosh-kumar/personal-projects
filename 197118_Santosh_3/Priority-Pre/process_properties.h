
#ifndef __PROCESS__PROPERTIES__SAN__
#define __PROCESS__PROPERTIES__SAN__
#define M 9
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

struct processProperties_t {
    int pid;               //process id
    int at;                //arrival time
    int bt;                //burst time
    int pri;               //Priority
    int rt;                //remaining time
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
    currProcess->pri = 0;
    currProcess->rt = 0;
    currProcess->frt = 0;
    currProcess->ct = 0;
    currProcess->tt = 0;
    currProcess->wt = 0;
    currProcess->allProperties[0] = &(currProcess->pid);
    currProcess->allProperties[1] = &(currProcess->at);
    currProcess->allProperties[2] = &(currProcess->bt);
    currProcess->allProperties[3] = &(currProcess->pri);
    currProcess->allProperties[4] = &(currProcess->rt);
    currProcess->allProperties[5] = &(currProcess->frt);
    currProcess->allProperties[6] = &(currProcess->ct);
    currProcess->allProperties[7] = &(currProcess->tt);
    currProcess->allProperties[8] = &(currProcess->wt);
}
void copy(processProperties *from, processProperties *to) {
    for (int i = 0; i < M; i++) {
        *(to->allProperties[i]) = *(from->allProperties[i]);
    }
}
#endif
