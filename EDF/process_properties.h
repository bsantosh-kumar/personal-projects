#include <stdio.h>
#include <stdlib.h>
#define M 11
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

struct processProperties_t {
    int pid;               //process id
    int at;                //arrival time
    int bt;                //burst time
    int per;               //period the process
    int pri;               //priority
    int rt;                //remaining time
    int dl;                //dealine
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
    currProcess->per = 0;
    currProcess->pri = 0;
    currProcess->rt = 0;
    currProcess->dl = 0;
    currProcess->frt = 0;
    currProcess->ct = 0;
    currProcess->tt = 0;
    currProcess->wt = 0;
    currProcess->allProperties[0] = &(currProcess->pid);
    currProcess->allProperties[1] = &(currProcess->at);
    currProcess->allProperties[2] = &(currProcess->bt);
    currProcess->allProperties[3] = &(currProcess->per);
    currProcess->allProperties[4] = &(currProcess->pri);
    currProcess->allProperties[5] = &(currProcess->rt);
    currProcess->allProperties[6] = &(currProcess->dl);
    currProcess->allProperties[7] = &(currProcess->frt);
    currProcess->allProperties[8] = &(currProcess->ct);
    currProcess->allProperties[9] = &(currProcess->tt);
    currProcess->allProperties[10] = &(currProcess->wt);
}
void getNext(processProperties **from, processProperties **to) {
    (*to) = malloc(sizeof(processProperties));
    intializeProperties(*to);
    for (int i = 0; i < M; i++) {
        *((*to)->allProperties[i]) = *((*from)->allProperties[i]);
    }
    (*to)->at += (*to)->per;
    (*to)->dl += (*to)->per;
}
void copyProcess(processProperties **from, processProperties **to) {
    (*to) = (processProperties *)malloc(sizeof(processProperties *));
    intializeProperties(*to);
    for (int i = 0; i < M; i++) {
        *((*to)->allProperties[i]) = *((*from)->allProperties[i]);
    }
}