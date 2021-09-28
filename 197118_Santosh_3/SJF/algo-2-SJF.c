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
    for (int i = 0; i < M; i++) {
        maxSizeForFormatting[i] += 2;
        printf("%*s", maxSizeForFormatting[i], headings[i]);
    }
    printf("\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < maxSizeForFormatting[i]; j++) {
            printf("-");
        }
    }
    printf("\n");
    for (int i = 0; i < noOfProcess; i++) {
        for (int j = 0; j < M; j++) {
            printf("%*d", maxSizeForFormatting[j], *(processes[i]->allProperties[j]));
        }
        printf("\n");
    }
}
bool compareBasedOnBT(void *a, void *b) {
    processProperties *_a = (processProperties *)a;
    processProperties *_b = (processProperties *)b;
    return _a->bt < _b->bt;
}
int compareBasedOnAT(const void *a, const void *b) {
    processProperties *_a = *(processProperties **)(a);
    processProperties *_b = *(processProperties **)(b);
    return _a->at - _b->at;
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
        if (processes[currIndex]->at > currTime) {
            printf("Was Idle from %d to %d\n", currTime, processes[currIndex]->at);
            currTime = processes[currIndex]->at;
            continue;
        }
        int temp = currIndex;
        while (temp < noOfProcess && processes[temp]->at <= currTime) {
            insertIntoPQ(processes[temp], heap, sizeof(heap[0]), &heapSize, compareBasedOnBT);
            temp++;
        }
        currIndex = temp;
        processProperties *currProcess = extractMinProcess(heap, sizeof(heap[0]), &heapSize, compareBasedOnBT);
        currProcess->frt = currTime;
        printf("Executing process P%d from %d to %d\n", currProcess->pid, currTime, currProcess->bt + currTime);
        currProcess->wt = currTime - currProcess->at;
        currTime += currProcess->bt;
        currProcess->ct = currTime;
    }
    calculateTT(processes, noOfProcess);
}
int main() {
    processProperties **processes = NULL;
    int noOfProcess = 0;
    takeInput(&processes, "input-SJF.txt", &noOfProcess);
    qsort(processes, noOfProcess, sizeof(processProperties *), compareBasedOnAT);
    SJFAlgo(processes, noOfProcess);
    printProcesses(processes, noOfProcess);

    return 0;
}