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

bool compareBasedOnBT(processProperties *a, processProperties *b) {
    return a->bt < b->bt;
}
int main() {
    processProperties **processes = NULL;
    int noOfProcess = 0;
    takeInput(&processes, "input-SJF.txt", &noOfProcess);
    int n;
    processProperties **heap = malloc((2 * noOfProcess + 1) * sizeof(processProperties *));
    int heapSize = 0;
    for (int i = 0; i < noOfProcess; i++) {
        insertIntoPQ(processes[i], heap, &heapSize, compareBasedOnBT);
    }
    while (heapSize) {
        processProperties *temp = extractMinProcess(heap, &heapSize, compareBasedOnBT);
    }

    return 0;
}