#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 3
#define maxItems 7

int buffer[BUFFER_SIZE] = {0};
sem_t empty;
sem_t full;
pthread_mutex_t mutex;
void *producer_function(void *_a) {
    int count = maxItems;
    int p = 0;
    while (count--) {
        int currElement = rand() % 100;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[p] = currElement;
        printf("Producing %d\n", currElement);
        p = (p + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer_function(void *_a) {
    int count = maxItems;
    int c = 0;
    while (count--) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int currElement = buffer[c];
        c = (c + 1) % BUFFER_SIZE;
        printf("Consuming %d\n", currElement);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}
int main() {
    pthread_t producer_thread, consumer_thread;
    srand(time(0));
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_create(&producer_thread, NULL, (void *)producer_function, NULL);
    pthread_create(&consumer_thread, NULL, (void *)consumer_function, NULL);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
}