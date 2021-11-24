#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "priority_queue.h"
#include "process_properties.h"
#include "queue.h"
#define N 5
#define MNP 4  //maximum number of process
#define PID 1
bool compareBasedOnAT(void *a, void *b) {
    processProperties *_a = (processProperties *)(a);
    processProperties *_b = (processProperties *)(b);
    if (_a->at != _b->at)
        return _a->at < _b->at;
    return _a->pri < _b->pri;
}
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
    (*processes) = (processProperties **)malloc((MNP * (count / N) + 1) * sizeof(processProperties *));
    (*noOfProcess) = 0;
    for (int i = 0; i < count / N; i++) {
        (*processes)[*noOfProcess] = malloc(sizeof(processProperties));
        processProperties *currProcess = (*processes)[*noOfProcess];
        intializeProperties(currProcess);
        for (int j = 0; j < N; j++) {
            fscanf(inputFile, "%d", (currProcess->allProperties[j]));
        }
        // int temp;
        // scanf("%d", &temp);
        currProcess->rt = currProcess->bt;
        currProcess->dl = currProcess->at;
        insertIntoPQ(currProcess, (*processes), sizeof(processProperties *), noOfProcess, compareBasedOnAT);
    }
}
int noOfDigits(int n) {
    int count = 0;
    while (n > 0) {
        count++;
        n /= 10;
    }
    return count;
}
void printProcesses(queue *q) {
    char *headings[] = {"Process ID", "Arrival Time", "Burst Time", "Period", "Priority", "Reamining time", "dead line", "First Response Time", "Completion Time", "Turn-around Time", "Waiting Time"};
    int maxSizeForFormatting[M] = {0};
    for (int i = 0; i < M; i++) {
        if (i == 5 || i == 6) continue;
        maxSizeForFormatting[i] = max(maxSizeForFormatting[i], strlen(headings[i]));
    }
    lptr T = q->f;
    while (T != NULL) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                if (j == 5 || j == 6) continue;
                maxSizeForFormatting[i] = max(maxSizeForFormatting[i], noOfDigits(*(T->data->allProperties[j])));
            }
        }
        T = T->next;
    }
    printf("*");
    for (int i = 0; i < M; i++) {
        maxSizeForFormatting[i] += 2;
        if (i == 5 || i == 6) continue;
        for (int j = 0; j < maxSizeForFormatting[i]-1; j++) {
            printf("-");
        }
        printf("*");
    }
    printf("\n");
    for (int i = 0; i < M; i++) {
        if (i == 5 || i == 6) continue;
        printf("|%*s", maxSizeForFormatting[i]-1, headings[i]);
    }
    printf("|\n");
 printf("*");
    for (int i = 0; i < M; i++) {
        if (i == 5 || i == 6) continue;
        for (int j = 0; j < maxSizeForFormatting[i]-1; j++) {
            printf("-");
        }
        printf("*");
    }
    printf("\n");
    T = q->f;
    while (T != NULL) {
        for (int j = 0; j < M; j++) {
            if (j == 5 || j == 6) continue;
            printf("|%*d", maxSizeForFormatting[j]-1, *(T->data->allProperties[j]));
        }
        T = T->next;
        printf("|\n");
         printf("*");
        for (int i = 0; i < M; i++) {
            if (i == 5 || i == 6) continue;
            for (int j = 0; j < maxSizeForFormatting[i]-1; j++) {
                printf("-");
            }
            printf("*");
        }
        printf("\n"); 
    }
}
bool compareBasedOnDL(void *a, void *b) {
    processProperties *_a = (processProperties *)a;
    processProperties *_b = (processProperties *)b;
    if (_a->pri != _b->pri)
        return _a->pri < _b->pri;
    return _a->at < _b->at;
}
void calculateTT(queue *q) {
    lptr T = q->f;
    while (T != NULL) {
        T->data->tt = T->data->ct - T->data->at;
        T = T->next;
    }
}
void EDFAlgo(processProperties **processes, int noOfProcess, queue *q) {
    processProperties **heap = malloc((2 * MNP * noOfProcess + 1) * sizeof(processProperties *));
    int heapSize = 0;
    int currIndex = 0;
    int currTime = 0;
    int noLeft = MNP;
    while (noLeft) {
        processProperties *peekProcess = peekPQ(processes);

        if ((peekProcess != NULL && peekProcess->at > currTime) && heapSize == 0) {
            printf("Was Idle from %d to %d\n", currTime, peekProcess->at);
            currTime = peekProcess->at;
            continue;
        }
        int temp = currIndex;
        while (noOfProcess != 0 && peekProcess != NULL && peekProcess->at <= currTime) {
            peekProcess = extractMinProcess(processes, sizeof(processes[0]), &noOfProcess, compareBasedOnAT);
            insertIntoPQ(peekProcess, heap, sizeof(peekProcess), &heapSize, compareBasedOnDL);
            processProperties *nextProcess = NULL;
            nextProcess = NULL;
            getNext(&peekProcess, &nextProcess);
            insertIntoPQ(nextProcess, processes, sizeof(nextProcess), &noOfProcess, compareBasedOnAT);
            peekProcess = peekPQ(processes);
        }
        processProperties *currProcess = extractMinProcess(heap, sizeof(heap[0]), &heapSize, compareBasedOnDL);
        if (currProcess->rt == currProcess->bt) {
            currProcess->frt = currTime;
            currProcess->ct = currProcess->at;
        }
        int tempTime = currTime;
        currProcess->wt += tempTime - currProcess->ct;
        int countIterations = 0;
        while (noOfProcess) {
            peekProcess = peekPQ(processes);
            if (currProcess->rt == 0) {
                if (currProcess->pid == PID) {
                    noLeft--;
                }
                currProcess->ct = tempTime;
                enqueue(q, currProcess);
                break;
            }
            if (currProcess->rt <= peekProcess->at - tempTime) {
                if (currProcess->pid == PID) {
                    noLeft--;
                }
                tempTime += currProcess->rt;
                currProcess->ct = tempTime;
                enqueue(q, currProcess);
                break;
            }

            currProcess->rt -= peekProcess->at - tempTime;
            tempTime = peekProcess->at;
            currProcess->ct = tempTime;
            if (compareBasedOnDL(currProcess, peekProcess)) {
                while (noOfProcess != 0 && peekProcess != NULL && peekProcess->at <= tempTime) {
                    peekProcess = extractMinProcess(processes, sizeof(processes[0]), &noOfProcess, compareBasedOnAT);
                    insertIntoPQ(peekProcess, heap, sizeof(heap[0]), &heapSize, compareBasedOnDL);
                    processProperties *nextProcess = NULL;
                    getNext(&peekProcess, &nextProcess);
                    insertIntoPQ(nextProcess, processes, sizeof(processes[0]), &noOfProcess, compareBasedOnAT);
                    peekProcess = peekPQ(processes);
                }
                continue;
            } else {
                insertIntoPQ(currProcess, heap, sizeof(heap[0]), &heapSize, compareBasedOnDL);
                break;
            }
        }
        if (noOfProcess == 0) {
            tempTime += currProcess->rt;
            currProcess->rt = 0;
            currProcess->ct = tempTime;
        }
        printf("Process P%d from %d to %d\n", currProcess->pid, currTime, tempTime);
        currTime = tempTime;
        currProcess->ct = currTime;
    }
    calculateTT(q);
}
int main() {
    processProperties **processes = NULL;
    int noOfProcess = 0;
    takeInput(&processes, "input-RM.txt", &noOfProcess);
    int noOf = 0;
    processProperties **finalAns = malloc(sizeof(processProperties *) * (2 * MNP * noOfProcess));
    for (int i = 0; i < (2 * MNP * noOfProcess); i++) {
        finalAns[i] = NULL;
    }
    queue *q = malloc(sizeof(queue));
    intializeQueue(q);
    EDFAlgo(processes, noOfProcess, q);
    printProcesses(q);

    return 0;
}