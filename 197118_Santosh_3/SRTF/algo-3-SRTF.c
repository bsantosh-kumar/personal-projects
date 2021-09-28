#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "priority_queue.h"
#define N 3

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
        printf("%*s", maxSizeForFormatting[i], headings[i]);
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

            printf("%*d", maxSizeForFormatting[j], *(processes[i]->allProperties[j]));
        }
        printf("\n");
    }
}
bool compareBasedOnRT(processProperties *a, processProperties *b) {
    return a->rt < b->rt;
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
void SJFAlgo(processProperties **processes, int noOfProcess) {
    processProperties **heap = malloc((2 * noOfProcess + 1) * sizeof(processProperties *));
    int heapSize = 0;
    int currIndex = 0;
    int currTime = 0;
    while (heapSize != 0 || currTime == 0 || currIndex < noOfProcess) {
        if (currIndex < noOfProcess && processes[currIndex]->at > currTime && heapSize == 0) {
            printf("Was Idle from %d to %d\n", currTime, processes[currIndex]->at);
            currTime = processes[currIndex]->at;
            continue;
        }
        int tempIndex = currIndex;
        while (tempIndex < noOfProcess && processes[tempIndex]->at <= currTime) {
            insertIntoPQ(processes[tempIndex], heap, &heapSize, compareBasedOnRT);
            tempIndex++;
        }
        currIndex = tempIndex;
        processProperties *currProcess = extractMinProcess(heap, &heapSize, compareBasedOnRT);
        if (currProcess->rt == currProcess->bt)
            currProcess->frt = currTime;
        int tempTime = currTime;
        currProcess->wt += currTime - currProcess->ct;
        while (tempIndex < noOfProcess) {
            if (currProcess->rt == 0) {
                currProcess->ct = tempTime;
                break;
            }
            if (currProcess->rt <= processes[tempIndex]->at - tempTime) {
                tempTime += currProcess->rt;
                currProcess->ct = tempTime;
                break;
            }
            currProcess->rt -= processes[tempIndex]->at - tempTime;
            tempTime = processes[tempIndex]->at;
            currProcess->ct = tempTime;
            if (compareBasedOnRT(currProcess, processes[tempIndex])) {
                while (tempIndex < noOfProcess && processes[tempIndex]->at <= tempTime) {
                    insertIntoPQ(processes[tempIndex], heap, &heapSize, compareBasedOnRT);
                    tempIndex++;
                }
                continue;
            } else {
                insertIntoPQ(currProcess, heap, &heapSize, compareBasedOnRT);
                break;
            }
        }
        if (tempIndex == noOfProcess) {
            tempTime += currProcess->rt;
            currProcess->rt = 0;
            currProcess->ct = tempTime;
        }
        printf("Executing process P%d from %d to %d\n", currProcess->pid, currTime, currProcess->ct);
        currTime = tempTime;
        currIndex = tempIndex;
    }
    calculateTT(processes, noOfProcess);
    printf("\n");
}
int main() {
    processProperties **processes = NULL;
    int noOfProcess = 0;
    takeInput(&processes, "input-SRTF.txt", &noOfProcess);
    qsort(processes, noOfProcess, sizeof(processProperties *), compareBasedOnAT);
    SJFAlgo(processes, noOfProcess);
    printProcesses(processes, noOfProcess);

    return 0;
}