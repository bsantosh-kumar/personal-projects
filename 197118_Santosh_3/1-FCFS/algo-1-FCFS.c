#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define N 3 /*This represents no of columns in input file                               \
              And make sure each line in input file contains N space separated integers \
             */
#define M 7 /* This is the total number of properties for a process */
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
struct processProperties_t {
    int pid;               //process id
    int at;                //arrival time
    int bt;                //burst time
    int frt;               //response time
    int ct;                //completion time
    int tt;                //turn around time
    int wt;                //waiting time
    int *allProperties[M]; /* The purpose for this array is just for index-wise accessing of the properties */
};
typedef struct processProperties_t processProperties;
/*
    This function is used to intialize all the values including the allProperties array
*/
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
/*
    This function is used to take Input from a given fileName
    It first assertains that the number of elements in a multiple of N
    (Where each multiple can be considered as first N properties of the process)
    It fills in the proccess array ( and also noOfProcess)
*/
void takeInput(processProperties ***processes, char fileName[], int *noOfProcess) {
    FILE *inputFile = fopen(fileName, "r");
    int count = 0;
    int temp = 0;
    while ((fscanf(inputFile, "%d", &temp)) != EOF) {
        count++;
    }
    if (count % N != 0) {
        fprintf(stderr, "Input not present in given format\n");  //If the input is not proper we are returning
        assert(count % N == 0);
        exit(EXIT_FAILURE);
    }
    fseek(inputFile, 0, SEEK_SET);
    (*processes) = (processProperties **)malloc((count / N) * sizeof(processProperties *));
    for (int i = 0; i < count / N; i++) {
        (*processes)[i] = malloc(sizeof(processProperties));
        intializeProperties((*processes)[i]);
        for (int j = 0; j < N; j++) {
            fscanf(inputFile, "%d", (*processes)[i]->allProperties[j]);
        }
    }
    (*noOfProcess) = count / N;
}
int cmpFunc(const void *a, const void *b) {
    processProperties *_a = *(processProperties **)(a);
    processProperties *_b = *(processProperties **)(b);
    return _a->at - _b->at;
}
int noOfDigits(int n) {
    int count = 0;
    while (n > 0) {
        count++;
        n /= 10;
    }
    return count;
}
void printProcesses(processProperties **processes, int noOfProcess) {
    char *headings[] = {"Process ID", "Arrival Time", "Burst Time", "First Response Time", "Completion Time", "Turn-around Time", "Waiting Time"};
    int maxSizeForFormatting[M] = {0};
    for (int i = 0; i < M; i++) {
        maxSizeForFormatting[i] = max(maxSizeForFormatting[i], strlen(headings[i]));
    }
    for (int i = 0; i < noOfProcess; i++) {
        for (int j = 0; j < M; j++) {
            maxSizeForFormatting[i] = max(maxSizeForFormatting[i], noOfDigits(*(processes[i]->allProperties[j])));
        }
    }
        printf("*");
    for (int i = 0; i < M; i++) {
        maxSizeForFormatting[i] += 2;
        for (int j = 0; j < maxSizeForFormatting[i]-1; j++) {
            printf("-");
        }
        printf("*");
    }
    printf("\n");
    for (int i = 0; i < M; i++) {
        printf("|%*s", maxSizeForFormatting[i]-1, headings[i]);
    }
    printf("|\n");
        printf("*");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < maxSizeForFormatting[i]-1; j++) {
            printf("-");
        }
        printf("*");
    }
    printf("\n");
    for (int i = 0; i < noOfProcess; i++) {
        for (int j = 0; j < M; j++) {
            printf("|%*d", maxSizeForFormatting[j]-1, *(processes[i]->allProperties[j]));
        }
        printf("|\n");
        printf("*");

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < maxSizeForFormatting[i]-1; j++) {
                printf("-");
            }
            printf("*");
        }
        printf("\n");    
    }
}
bool checkForIdle(int arrivalTime, int currTime) {
    return arrivalTime > currTime;
}
void calculateTT(processProperties **processes, int noOfProcess) {
    for (int i = 0; i < noOfProcess; i++) {
        processes[i]->tt = processes[i]->ct - processes[i]->at;
    }
}
void FCFSAlgo(processProperties **processes, int noOfProcess) {
    int currProcessIndex = 0;
    int countNoOfSchedules = 0;
    int currTime = 0;
    bool isVisited[noOfProcess]; /*This array tells whether a process has been visited or not
                                      If it is not visited earlier and this is the first encounter
                                      then it stores the currTime inside it
                                      Basically a place holder for first response time
                                      */
    for (int i = 0; i < noOfProcess; i++) {
        isVisited[i] = false;
    }
    while (currProcessIndex < noOfProcess) {
        bool isIdle = checkForIdle(processes[currProcessIndex]->at, currTime);
        if (isIdle) {
            printf("Was Idle from %d to %d\n", currTime, processes[currProcessIndex]->at);
            currTime = processes[currProcessIndex]->at;
            countNoOfSchedules++;
        }
        if (!isVisited[currProcessIndex]) {
            isVisited[currProcessIndex] = true;
            processes[currProcessIndex]->frt = currTime;
        }
        printf("Executing process P%d from %d to %d\n", processes[currProcessIndex]->pid, currTime, processes[currProcessIndex]->bt + currTime);
        processes[currProcessIndex]->wt = currTime - processes[currProcessIndex]->at;
        currTime += processes[currProcessIndex]->bt;
        processes[currProcessIndex]->ct = currTime;
        currProcessIndex++;
    }
    calculateTT(processes, noOfProcess);
}
int main() {
    processProperties **processes = NULL;
    int noOfProcess = 0;
    takeInput(&processes, "input-FCFS.txt", &noOfProcess);

    qsort(processes, noOfProcess, sizeof(processProperties *), cmpFunc);
    //We are sorting by arrival times
    FCFSAlgo(processes, noOfProcess);
    printProcesses(processes, noOfProcess);
    return 0;
}