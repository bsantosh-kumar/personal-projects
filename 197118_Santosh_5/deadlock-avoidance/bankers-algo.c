#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#define N 3
struct shared_resources_t {
    int maxRes[N];        // maximum Resources
    int allocatedRes[N];  // allocated resources
    int needRes[N];       // Need resources, that a process needs
};
typedef struct shared_resources_t shared_resources;
typedef shared_resources *srPtr;  // shared resources pointer
srPtr shared_resources_init(int maxResources[]) {
    srPtr ans = malloc(sizeof(shared_resources));
    for (int i = 0; i < N; i++) {
        ans->maxRes[i] = maxResources[i];
        ans->needRes[i] = maxResources[i];
        ans->allocatedRes[i] = 0;
    }
    return ans;
}
void printResources(srPtr resources, int index) {
    printf("\n\n");
    printf("index:%d\n", index);
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
}
int main() {
    int maxResources[] = {1, 2, 3};
    srPtr processResources = shared_resources_init(maxResources);
    printResources(processResources, 0);
}