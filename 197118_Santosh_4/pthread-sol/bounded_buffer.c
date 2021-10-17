#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BUFFER_SIZE 3
#define maxItems 7
int buffer[BUFFER_SIZE] = {0};
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;
int empty = BUFFER_SIZE;
int full = 0;
void *producer_function(void *_a) {
    int count = maxItems;
    int p = 0;
    while (count--) {
        int currElement = rand() % 100;
        pthread_mutex_lock(&count_mutex);
        if (empty == 0)
            pthread_cond_wait(&full_cond, &count_mutex);
        buffer[p] = currElement;
        printf("Producing %d\n", currElement);
        p = (p + 1) % BUFFER_SIZE;
        empty--;
        full++;
        pthread_cond_signal(&empty_cond);
        pthread_mutex_unlock(&count_mutex);
    }
}
void *consumer_function(void *_a) {
    int count = maxItems;
    int c = 0;
    while (count--) {
        pthread_mutex_lock(&count_mutex);
        if (full == 0)
            pthread_cond_wait(&empty_cond, &count_mutex);
        int currElement = buffer[c];
        c = (c + 1) % BUFFER_SIZE;
        printf("Consuming %d\n", currElement);
        fflush(stdout);
        empty++;
        full--;
        pthread_cond_signal(&full_cond);
        pthread_mutex_unlock(&count_mutex);
    }
}
int main() {
    pthread_t producer_thread, consumer_thread;
    srand(time(0));
    pthread_create(&producer_thread, NULL, (void *)producer_function, NULL);
    pthread_create(&consumer_thread, NULL, (void *)consumer_function, NULL);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
}