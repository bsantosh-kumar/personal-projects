#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define NO_OF_CUSTOMERS 5
#define NO_OF_CHAIRS 2
int chairs[NO_OF_CHAIRS];
sem_t barber_cust;
sem_t mutex;
int curr_full = 0;
int next_chair_cust = 0;
int no_of_cust_left = 0;
int next_chair_barber = 0;
void *customer_function(void *id) {
    int index = *(int *)id;
    sleep(rand() % NO_OF_CUSTOMERS);
    sem_wait(&mutex);
    printf("%d arrived\n", index);
    if (curr_full == NO_OF_CHAIRS) {
        printf("%d leaving because all are filled\n", index);
        no_of_cust_left++;
        sem_post(&mutex);
        return NULL;
    }
    chairs[next_chair_cust] = index;
    int flag = curr_full == 0 && next_chair_barber == next_chair_cust;
    next_chair_cust = (next_chair_cust + 1) % NO_OF_CHAIRS;
    curr_full++;
    if (flag == 1) {
        printf("%d Waking up barber\n", index);
        sem_post(&barber_cust);
    }
    sem_post(&mutex);
}
void *barber_function(void *argv) {
    int no_of_cust_served = 0;
    while (true) {
        sem_wait(&mutex);
        if (curr_full == 0) {
            printf("barder sleeping...zzz\n");
            sem_post(&mutex);
            sem_wait(&barber_cust);
            sem_wait(&mutex);
        }
        printf("Serving %d\n", chairs[next_chair_barber]);
        curr_full--;
        sem_post(&mutex);
        sleep(rand() % 3);
        sem_wait(&mutex);
        no_of_cust_served++;
        next_chair_barber = (next_chair_barber + 1) % NO_OF_CHAIRS;
        if (no_of_cust_served + no_of_cust_left == NO_OF_CUSTOMERS) {
            sem_post(&mutex);
            break;
        }
        sem_post(&mutex);
    }
}
int main() {
    srand(time(0));
    pthread_t customers[NO_OF_CUSTOMERS];
    pthread_t barber;
    sem_init(&barber_cust, 0, 0);
    sem_init(&mutex, 0, 1);
    pthread_create(&barber, NULL, barber_function, NULL);
    int customer_id[NO_OF_CUSTOMERS];
    for (int i = 0; i < NO_OF_CUSTOMERS; i++) {
        customer_id[i] = i;
        pthread_create(&customers[i], NULL, customer_function, &customer_id[i]);
    }
    printf("No Of Customers: %d\nNo Of Chairs: %d\n", NO_OF_CUSTOMERS, NO_OF_CHAIRS);
    pthread_join(barber, NULL);
    for (int i = 0; i < NO_OF_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }
}