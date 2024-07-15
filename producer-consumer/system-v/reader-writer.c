#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NO_OF_READERS 3
sem_t rw_mutex;
sem_t r_mutex;
int readers_count = 0;
void *writer_function(void *argv) {
    int no_of_writes = 5;
    while (no_of_writes) {
        sem_wait(&rw_mutex);
        printf("Writing...\n");
        no_of_writes--;
        sleep(2);
        sem_post(&rw_mutex);
        sleep(5);
    }
}
void *reader_function(void *id) {
    int no_of_readings = 2;
    int index = *(int *)id;
    while (no_of_readings) {
        sem_wait(&r_mutex);
        readers_count++;
        if (readers_count == 1)
            sem_wait(&rw_mutex);
        sem_post(&r_mutex);
        printf("%d reading...\n", index);
        no_of_readings--;
        sleep(rand() % 3);
        sem_wait(&r_mutex);
        readers_count--;
        if (readers_count == 0)
            sem_post(&rw_mutex);
        sem_post(&r_mutex);
    }
}
int main() {
    srand(time(0));
    pthread_t writer;
    pthread_t readers[NO_OF_READERS];
    sem_init(&rw_mutex, 0, 1);
    sem_init(&r_mutex, 0, 1);
    pthread_create(&writer, NULL, writer_function, NULL);
    int reader_id[NO_OF_READERS];
    for (int i = 0; i < NO_OF_READERS; i++) {
        reader_id[i] = i;
        pthread_create(&readers[i], NULL, reader_function, &reader_id[i]);
    }
    pthread_join(writer, NULL);
    for (int i = 0; i < NO_OF_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
}
