#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 3     // no of resources
#define M 3     // no of processes
#define MIN 5   // minimum number of instances of resources
#define MAX 10  // maximum number of instances of resources
struct process_resources_t {
    int pid;              // process id for which the resources are allocated
    int maxRes[N];        // maximum Resources
    int allocatedRes[N];  // allocated resources
    int needRes[N];       // Need resources, that a process needs
    sem_t resourceSem;
};
typedef struct process_resources_t process_resources;
typedef process_resources *prPtr;  // shared resources pointer

struct request_resources_t {
    int pid;
    int requestToken[N];
    int isRequesting;  // -1 thread has teriminated
                       //  0 thread is present, and not requesting now
                       //  1 thread is present, and requesting resources
    int canAllocate;
};
typedef struct request_resources_t request_resources;
typedef request_resources *rrPtr;  // pointer for request resources

struct process_utility_t {
    prPtr process_resources;
    rrPtr request;
    sem_t processSem;
};
typedef struct process_utility_t process_utility;
typedef process_utility *puPtr;

struct allResources_t {
    prPtr kernelRes;
    puPtr *allProcesses;
};
typedef struct allResources_t allResources;
typedef allResources *arPtr;

/*
    Intialize request_resources structure
*/
void request_resources_init(rrPtr *currRequest, prPtr process) {
    (*currRequest) = (rrPtr)malloc(sizeof(request_resources));
    rrPtr ans = (*currRequest);
    ans->pid = process->pid;
    ans->isRequesting = 0;
    ans->canAllocate = 0;
    for (int i = 0; i < N; i++) {
        ans->requestToken[i] = 0;
    }
    return;
}

void process_resources_init(prPtr *currProcess, int maxResources[], int pid) {
    (*currProcess) = (process_resources *)malloc(sizeof(process_resources));
    prPtr ans = (*currProcess);
    for (int i = 0; i < N; i++) {
        ans->pid = pid;
        ans->maxRes[i] = maxResources[i];
        ans->needRes[i] = maxResources[i];
        ans->allocatedRes[i] = 0;
    }
    sem_init(&(ans->resourceSem), 0, 0);
    return;
}
struct kernelArg_t {
    puPtr *allP;
    prPtr kernelRes;
    pthread_t *procThreads;
};
typedef struct kernelArg_t kernelArg;
sem_t resourcesLock;
sem_t printLock;
void printResources(prPtr resources) {
    sem_wait(&printLock);
    printf("\n\n");
    printf("index:%d\n", resources->pid);
    printf("Maximum resources\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", resources->maxRes[i]);
    }
    printf("\n");
    printf("Allocated resources\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", resources->allocatedRes[i]);
    }
    printf("\n");
    printf("Need resources\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", resources->needRes[i]);
    }
    printf("\n\n");
    sem_post(&printLock);
}
void printRequest(rrPtr currRequest) {
    sem_wait(&printLock);
    printf("Process:%d is requesting resources\nThe request being:\n", currRequest->pid);
    for (int i = 0; i < N; i++) {
        printf("%d ", currRequest->requestToken[i]);
    }
    printf("\n");
    sem_post(&printLock);
}
/*
This function is used to copy all the resources,
given a pointer to copy 'from', and a pointer to copy 'to'
*/
void copyResources(prPtr *from, prPtr *to) {
    if ((*from) == NULL) {
        (*to) = NULL;
        return;
    }
    (*to) = malloc(sizeof(process_resources));
    for (int i = 0; i < N; i++) {
        (*to)->allocatedRes[i] = (*from)->allocatedRes[i];
        (*to)->maxRes[i] = (*from)->maxRes[i];
        (*to)->needRes[i] = (*from)->needRes[i];
    }
    (*to)->pid = (*from)->pid;
}

/*
    This function checks whether it is possible to allocate resources or not
    Given avialable resources, and the process-need

*/
bool isLessThan(int process[], int avialable[]) {
    for (int i = 0; i < N; i++) {
        // if the need is greater than currently avialable then we cannot allocate
        if (avialable[i] < process[i])
            return false;
    }
    return true;
}

/*
    This function implements safety algorithm,
    determines whether a the system is in safe state, or unsafe state
*/
bool safetyAlgorithm(puPtr allProcesses[], prPtr avialable) {
    prPtr copyAvialable;
    copyResources(&avialable, &copyAvialable);
    // Copy all the resources and then check
    // for (int i = 0; i < M; i++) {
    //     copyResources(&allProcesses[i], &allProcesses[i]);
    // }
    bool canAllocate = false;
    bool isZero[M];
    for (int i = 0; i < M; i++) isZero[i] = false;
    // sem_wait(&printLock);
    // printf("Before safety algo:\n");
    // for (int i = 0; i < N; i++) {
    //     printf("%d ", copyAvialable->allocatedRes[i]);
    // }
    // printf("\n");
    // sem_post(&printLock);
    do {
        canAllocate = false;
        for (int i = 0; i < M; i++) {
            sem_wait(&allProcesses[i]->processSem);
            if (allProcesses[i]->request->isRequesting < 0) {
                isZero[i] = true;
                sem_post(&allProcesses[i]->processSem);
                continue;
            }
            if (!isZero[i] && isLessThan(allProcesses[i]->process_resources->needRes, copyAvialable->allocatedRes)) {
                canAllocate = true;
                for (int j = 0; j < N; j++) {
                    copyAvialable->allocatedRes[j] += allProcesses[i]->process_resources->allocatedRes[j];
                }
                isZero[i] = true;
            }
            sem_post(&allProcesses[i]->processSem);
        }
    } while (canAllocate);
    bool isSafe = true;
    for (int i = 0; i < M; i++) {
        if (!isZero[i]) {
            isSafe = false;
            break;
        }
    }
    return isSafe;
}

void init_code() {
    srand(time(NULL));
    sem_init(&resourcesLock, 0, 1);
    int outputFile = open("output.txt", O_RDWR);
    if (outputFile == -1) {
        printf("Cannot open output file\n");
        exit(0);
    }
    if (-1 == dup2(outputFile, STDOUT_FILENO)) {
        printf("cannot redirect output\n");
        exit(0);
    }
    sem_init(&printLock, 0, 1);
}
void generateRandomMaxResources(int maxResources[], prPtr kernelResources) {
    for (int i = 0; i < N; i++) {
        maxResources[i] = rand() % (kernelResources->maxRes[i]) + 1;
    }
}
void all_processes_init(puPtr allProcesses[], prPtr kernelResources) {
    for (int i = 0; i < M; i++) {
        int maxResources[N];
        int pid = i;
        generateRandomMaxResources(maxResources, kernelResources);
        printf("Came here i:%d\n", i);
        allProcesses[i] = (puPtr)malloc(sizeof(process_utility));
        process_resources_init(&(allProcesses[i]->process_resources), maxResources, pid);
        printResources(allProcesses[i]->process_resources);
        sem_init(&allProcesses[i]->processSem, 0, 1);
    }
    for (int i = 0; i < M; i++) {
        request_resources_init(&(allProcesses[i]->request), allProcesses[i]->process_resources);
    }
}

void generateRandomKernelResources(int maxKernelResources[]) {
    for (int i = 0; i < N; i++) {
        maxKernelResources[i] = rand() % (MAX - MIN + 1) + MIN;
    }
}
void kernel_resources_init(prPtr *kernelResources) {
    (*kernelResources) = malloc(sizeof(process_resources));
    prPtr ans = (*kernelResources);
    int maxKernelResources[N];
    generateRandomKernelResources(maxKernelResources);
    for (int i = 0; i < N; i++) {
        ans->allocatedRes[i] = maxKernelResources[i];
        ans->maxRes[i] = maxKernelResources[i];
        ans->needRes[i] = 0;
    }
    sem_init(&ans->resourceSem, 0, 1);
    ans->pid = -1;
    printf("Kernel Resources:");
    printResources(ans);
}
void terminate_process(puPtr currProcess) {
    sem_wait(&currProcess->processSem);
    currProcess->request->isRequesting = -1;
    sem_post(&currProcess->processSem);
    sem_wait(&printLock);
    printf("Terminating the process:%d\n", currProcess->process_resources->pid);
    sem_post(&printLock);
}

void terminate_process_from_kernel(puPtr currProcess, pthread_t procThread) {
    printf("Inside kernel termination\n");
    currProcess->request->isRequesting = -1;
    for (int i = 0; i < N; i++) {
        currProcess->request->requestToken[i] = 0;
        currProcess->process_resources->needRes[i] = 0;
    }
    pthread_cancel(procThread);
    sem_wait(&printLock);
    printf("Kernel terminated the process:%d\n", currProcess->process_resources->pid);

    sem_post(&printLock);
}

void generateRequest(puPtr currProcess) {
    sem_wait(&currProcess->processSem);
    int sum = 0;
    int *currNeed = currProcess->process_resources->needRes;
    int *currToken = currProcess->request->requestToken;
    do {
        for (int i = 0; i < N; i++) {
            currToken[i] = rand() % (currNeed[i] + 1);
            sum += currToken[i];
        }
    } while (sum == 0);
    currProcess->request->isRequesting = 1;
    printRequest(currProcess->request);
    sem_post(&currProcess->processSem);
}
void *simulateProcess(void *_a) {
    puPtr currProcess = *(puPtr *)(_a);
    int sum = 0;
    printf("pid: %d\n", currProcess->process_resources->pid);
    sem_wait(&currProcess->processSem);
    for (int i = 0; i < N; i++) sum += currProcess->process_resources->needRes[i];
    sem_post(&currProcess->processSem);
    while (sum != 0) {
        sum = 0;
        generateRequest(currProcess);
        sem_wait(&currProcess->process_resources->resourceSem);
        // if (currProcess->request->canAllocate == 0) {
        //     for (int i = 0; i < N; i++) sum += currProcess->process_resources->needRes[i];
        //     continue;
        // }
        int sleepTime = rand() % 3;
        sem_wait(&printLock);
        printf("Process: %d sleeping for %d\n", currProcess->process_resources->pid, sleepTime);
        sem_post(&printLock);
        // sleep(sleepTime);
        sem_wait(&currProcess->processSem);
        sem_wait(&printLock);
        printf("Process: %d came from sleep\n", currProcess->process_resources->pid);
        sem_post(&printLock);
        for (int i = 0; i < N; i++) sum += currProcess->process_resources->needRes[i];
        printf("sum:%d\n", sum);
        sem_post(&currProcess->processSem);
    }
    terminate_process(currProcess);
}
void printMoreThanNeed(puPtr currProcess) {
    sem_wait(&printLock);
    printf("%d is requesting for more resources than Need, starting termination of process.", currProcess->process_resources->pid);
    sem_post(&printLock);
}

void printAllocatingResources(puPtr currProcess) {
    sem_wait(&printLock);
    rrPtr currRequest = currProcess->request;
    prPtr currResources = currProcess->process_resources;
    printf("Allocating resources to process:%d, The request being:\n", currRequest->pid);
    for (int i = 0; i < N; i++) {
        printf("%d ", currRequest->requestToken[i]);
    }
    printf("\nThe need is:");
    for (int i = 0; i < N; i++) {
        printf("%d ", currResources->needRes[i]);
    }
    printf("\nAllocated[]:");
    for (int i = 0; i < N; i++) {
        printf("%d ", currResources->allocatedRes[i]);
    }
    printf("\n");
    sem_post(&printLock);
}
void subtractRequest(int alloactedRes[], int requestToken[]) {
    for (int i = 0; i < N; i++) {
        alloactedRes[i] -= requestToken[i];
    }
}
void addRequest(int alloctedRes[], int requestToken[]) {
    for (int i = 0; i < N; i++) {
        alloctedRes[i] += requestToken[i];
    }
}
void *simulate_kernel(void *_a) {
    kernelArg *threadArg = ((kernelArg *)(_a));
    puPtr *allProcesses = threadArg->allP;
    prPtr kernelResources = threadArg->kernelRes;
    pthread_t *processThreads = threadArg->procThreads;
    bool terminate = true;
    int canAllocate = -1;
    do {
        terminate = true;
        canAllocate = -1;
        for (int i = 0; i < M; i++) {
            sem_wait(&allProcesses[i]->processSem);
            // printf("Checking id:%d\n", allProcesses[i]->process_resources->pid);
            if (allProcesses[i]->request->isRequesting < 0) {
                if (allProcesses[i]->request->isRequesting == -1) {
                    sem_wait(&printLock);
                    printf("Taking all resources from process:%d\nWith kernel resources:", allProcesses[i]->process_resources->pid);
                    for (int j = 0; j < N; j++) {
                        printf("%d ", kernelResources->allocatedRes[j]);
                    }

                    printf("\nMax Res[]=");
                    for (int j = 0; j < N; j++) {
                        printf("%d ", allProcesses[i]->process_resources->maxRes[j]);
                    }
                    printf("\n");
                    sem_post(&printLock);
                    allProcesses[i]->request->isRequesting = -2;
                    for (int j = 0; j < N; j++) {
                        kernelResources->allocatedRes[j] += allProcesses[i]->process_resources->maxRes[j];
                        allProcesses[i]->process_resources->needRes[j] = allProcesses[i]->process_resources->allocatedRes[j];
                        allProcesses[i]->process_resources->allocatedRes[j] = 0;
                        allProcesses[i]->request->requestToken[j] = 0;
                    }
                    sem_wait(&printLock);
                    printf("Done taking all resources from process:%d\n,with kernel resoruces:", allProcesses[i]->process_resources->pid);
                    for (int j = 0; j < N; j++) {
                        printf("%d ", kernelResources->allocatedRes[j]);
                    }
                    printf("\n");
                    sem_post(&printLock);
                }
                sem_post(&allProcesses[i]->processSem);
                continue;
            }
            terminate = false;
            if (allProcesses[i]->request->isRequesting == 0) {
                canAllocate = 1;
                sem_post(&allProcesses[i]->processSem);
                continue;
            } else if (allProcesses[i]->request->isRequesting == 1) {
                if (canAllocate == -1) canAllocate = 0;
                if (!isLessThan(allProcesses[i]->request->requestToken, allProcesses[i]->process_resources->needRes)) {
                    printMoreThanNeed(allProcesses[i]);
                    terminate_process_from_kernel(allProcesses[i], processThreads[i]);
                    sem_post(&allProcesses[i]->process_resources->resourceSem);
                    sem_post(&allProcesses[i]->processSem);
                    continue;
                }
                if (!isLessThan(allProcesses[i]->request->requestToken, kernelResources->allocatedRes)) {
                    sem_post(&allProcesses[i]->processSem);
                    continue;
                }
                canAllocate = 1;
                subtractRequest(allProcesses[i]->process_resources->needRes, allProcesses[i]->request->requestToken);
                addRequest(allProcesses[i]->process_resources->allocatedRes, allProcesses[i]->request->requestToken);
                subtractRequest(kernelResources->allocatedRes, allProcesses[i]->request->requestToken);
                // printRequest(allProcesses[i]->request);
                sem_post(&allProcesses[i]->processSem);
            }
        }
        if (canAllocate == 0) {
            printf("Deadlock state reached, so exiting\n");
            exit(0);
        }
    } while (!terminate);
}
void kernel_thread_init(pthread_t *_kernalThread, prPtr kernelResources, puPtr allProcesses[], pthread_t processThreads[]) {
    kernelArg *threadArg = (kernelArg *)malloc(sizeof(kernelArg));
    threadArg->allP = allProcesses;
    threadArg->kernelRes = kernelResources;
    threadArg->procThreads = processThreads;
    pthread_create(_kernalThread, 0, simulate_kernel, threadArg);
}
void kernel_thread_join(pthread_t kernelThread) {
    pthread_join(kernelThread, NULL);
}
void process_threads_init(pthread_t processThreads[], puPtr allProcesses[]) {
    for (int i = 0; i < M; i++) {
        pthread_create(&processThreads[i], NULL, (void *)simulateProcess, &allProcesses[i]);
    }
}
void process_threads_join(pthread_t processThreads[]) {
    for (int i = 0; i < M; i++) {
        pthread_join(processThreads[i], NULL);
    }
}

void detection_thread_init(pthread_t *_detectThread, puPtr allProcesses[], prPtr kernelResources) {
}
int main() {
    printf("Starting code\n");
    init_code();
    printf("Intialised all of the vairables\n");
    prPtr kernelResources;
    kernel_resources_init(&kernelResources);
    puPtr allProcesses[M];
    all_processes_init(allProcesses, kernelResources);
    pthread_t processThreads[M];
    process_threads_init(processThreads, allProcesses);
    pthread_t kernelThread;
    // pthread_t detectionThread;
    // detection_thread_init(&detectionThread, allProcesses, kernelResources);
    kernel_thread_init(&kernelThread, kernelResources, allProcesses, processThreads);
    process_threads_join(processThreads);
    kernel_thread_join(kernelThread);
    printf("completed\n");
}