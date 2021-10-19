#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NO_OF_PHILOSOPHERS 5

sem_t wait[NO_OF_PHILOSOPHERS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t wait_mutex;
enum { EATING,
       THINKING,
       HUNGRY } currState[NO_OF_PHILOSOPHERS] = {THINKING};
void test(int index) {
    pthread_mutex_lock(&mutex);
    if (currState[(index + 1) % NO_OF_PHILOSOPHERS] != EATING && currState[(index + NO_OF_PHILOSOPHERS - 1) % NO_OF_PHILOSOPHERS] != EATING) {
        currState[index] = EATING;
        sem_post(&wait[index]);
    }
    pthread_mutex_unlock(&mutex);
}
void philosopher_function(void *id) {
    int index = *(int *)id;
    int maxEats = 1;
    while (maxEats) {
        sleep(1);
        printf("%d hungry\n", index);
        pthread_mutex_lock(&mutex);
        currState[index] = HUNGRY;
        pthread_mutex_unlock(&mutex);
        test(index);
        pthread_mutex_lock(&mutex);
        if (currState[index] != EATING) {
            pthread_mutex_unlock(&mutex);
            sem_wait(&wait[index]);
        } else
            pthread_mutex_unlock(&mutex);
        printf("%d eating\n", index);
        maxEats--;
        int eating_time = rand() % 3;
        sleep(eating_time);
        printf("%d Eating done\n", index);
        pthread_mutex_lock(&mutex);
        currState[index] = THINKING;
        printf("%d thinking\n", index);
        pthread_mutex_unlock(&mutex);
        test((index + 1) % NO_OF_PHILOSOPHERS);
        test((index + NO_OF_PHILOSOPHERS - 1) % NO_OF_PHILOSOPHERS);
    }
}

int main() {
    srand(time(0));
    pthread_t philosophers[NO_OF_PHILOSOPHERS];
    int thread_id[NO_OF_PHILOSOPHERS];
    sem_init(&wait_mutex, 0, 1);
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++)
        sem_init(&wait[i], 0, 1);
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        currState[i] = THINKING;
        thread_id[i] = i;
        pthread_create(&philosophers[i], NULL, (void *)philosopher_function, &thread_id[i]);
    }
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
}