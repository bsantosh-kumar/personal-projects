#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NO_OF_PHILOSOPHERS 5

pthread_cond_t wait[NO_OF_PHILOSOPHERS] = {PTHREAD_COND_INITIALIZER};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;
enum { EATING,
       THINKING,
       HUNGRY } currState[NO_OF_PHILOSOPHERS] = {THINKING};
void test(void *id) {
    int index = *(int *)id;
    int maxEats = 3;
    while (maxEats) {
        sleep(1);
        printf("%d hungry\n", index);
        pthread_mutex_lock(&mutex);
        currState[index] = HUNGRY;
        while (currState[(index + 1) % NO_OF_PHILOSOPHERS] == EATING || currState[(index + NO_OF_PHILOSOPHERS - 1) % NO_OF_PHILOSOPHERS] == EATING) {
            pthread_cond_wait(&wait[index], &mutex);
        }
        currState[index] = EATING;
        printf("%d eating\n", index);
        maxEats--;
        pthread_mutex_unlock(&mutex);
        int eating_time = rand() % 3;
        sleep(eating_time);
        printf("%d Eating done\n", index);
        pthread_mutex_lock(&mutex);
        currState[index] = THINKING;
        printf("%d thinking\n", index);
        pthread_cond_signal(&wait[(index + 1) % NO_OF_PHILOSOPHERS]);
        pthread_cond_signal(&wait[(index + NO_OF_PHILOSOPHERS - 1) % NO_OF_PHILOSOPHERS]);
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    srand(time(0));
    pthread_t philosophers[NO_OF_PHILOSOPHERS];
    int thread_id[NO_OF_PHILOSOPHERS];
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        currState[i] = THINKING;
        thread_id[i] = i;
        pthread_create(&philosophers[i], NULL, (void *)test, &thread_id[i]);
    }
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
}