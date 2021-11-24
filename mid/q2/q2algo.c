#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "process_properties.h"
#include "queue.h"
#define N 3
#define TQ 2  // Time Quantum
void takeInput(processProperties ***processes, char fileName[], int *noOfProcess) {
    FILE *inputFile = fopen(fileName, "r");
    int count = 0;
    int temp = 0;
    while ((fscanf(inputFile, "%d", &temp)) != EOF) {
        count++;
    }
    if (count % N != 0) {
        fprintf(stderr, "Input not present in given format\n");  // If the input is not proper we are returning
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
        (*processes)[i]->rt = (*processes)[i]->bt;
    }
    (*noOfProcess) = count / N;
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
    char *headings[] = {"Process ID", "Arrival Time", "Burst Time", "Remaining Time", "First Response Time", "Completion Time", "Turn-around Time", "Waiting Time"};
    int maxSizeForFormatting[M] = {0};
    for (int i = 0; i < M; i++) {
        if (i == 3) continue;
        maxSizeForFormatting[i] = max(maxSizeForFormatting[i], strlen(headings[i]));
    }
    for (int i = 0; i < noOfProcess; i++) {
        for (int j = 0; j < M; j++) {
            if (j == 3) continue;

            maxSizeForFormatting[i] = max(maxSizeForFormatting[i], noOfDigits(*(processes[i]->allProperties[j])));
        }
    }
    for (int i = 0; i < M; i++) {
        maxSizeForFormatting[i] += 2;
        if (i == 3) continue;
        printf("|%*s", maxSizeForFormatting[i]-1, headings[i]);
    }
    printf("\n");
    for (int i = 0; i < M; i++) {
        if (i == 3) continue;
        for (int j = 0; j < maxSizeForFormatting[i]; j++) {
            printf("-");
        }
    }
    printf("\n");
    for (int i = 0; i < noOfProcess; i++) {
        for (int j = 0; j < M; j++) {
            if (j == 3) continue;

            printf("|%*d", maxSizeForFormatting[j]-1, *(processes[i]->allProperties[j]));
        }
        printf("\n");
    }
}

bool compareBasedOnBT(processProperties *a, processProperties *b) {
    return a->bt < b->bt;
}
int compareBasedOnAT(const void *a, const void *b) {
    processProperties *_a = *(processProperties **)(a);
    processProperties *_b = *(processProperties **)(b);
    if (_a->at != _b->at)
        return _a->at - _b->at;
    return _a->bt - _b->bt;
}
void calculateTT(processProperties **processes, int noOfProcess) {
    for (int i = 0; i < noOfProcess; i++) {
        processes[i]->tt = processes[i]->ct - processes[i]->at;
    }
}
void RRAlgo(processProperties **processes, int noOfProcess) {
    queue *q = malloc(sizeof(queue));
    intializeQueue(q);
    int currIndex = 0, currTime = 0;
    while (q->nfe != 0 || currIndex < noOfProcess) {
        if (currIndex < noOfProcess && q->nfe == 0 && processes[currIndex]->at > currTime) {
            printf("Was Idle from %d to %d\n", currTime, processes[currIndex]->at);
            currTime = processes[currIndex]->at;
            continue;
        }
        int tempIndex = currIndex;
        while (tempIndex < noOfProcess && processes[tempIndex]->at <= currTime) {
            enqueue(q, processes[tempIndex]);
            tempIndex++;
        }
        currIndex = tempIndex;
        processProperties *currProcess = getFront(q);
        if (currProcess->ct == 0) {
            currProcess->frt = currTime;
            currProcess->ct = currProcess->at;
        }
        if (currProcess->rt == 0) {
            dequeue(q);
            continue;
        }
        currProcess->wt += currTime - currProcess->ct;
        int tempTime = currTime;
        if (currProcess->rt <= TQ) {
            tempTime += currProcess->rt;
            currProcess->rt = 0;
            currProcess->ct = tempTime;
            printf("Executing process P%d from %d to %d\n", currProcess->pid, currTime, currProcess->ct);
            currTime = tempTime;
            dequeue(q);
            continue;
        }
        tempTime += TQ;
        currProcess->rt -= TQ;
        currProcess->ct = tempTime;
        while (tempIndex < noOfProcess && processes[tempIndex]->at < tempTime) {
            enqueue(q, processes[tempIndex]);
            tempIndex++;
        }
        printf("Executing process P%d from %d to %d\n", currProcess->pid, currTime, currProcess->ct);
        if (currProcess->rt <= 0)
            dequeue(q);
        else {
            translate(q);
        }
        currTime = tempTime;
    }

    calculateTT(processes, noOfProcess);
}
int main() {
    processProperties **processes = NULL;
    int noOfProcess = 0;
    takeInput(&processes, "input-q2.txt", &noOfProcess);
    qsort(processes, noOfProcess, sizeof(processProperties *), compareBasedOnAT);
    RRAlgo(processes, noOfProcess);
    printProcesses(processes, noOfProcess);

    return 0;
}