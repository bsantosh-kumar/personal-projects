#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define NN 0x400000
//#define SQRTN	2048
int SQRTN;
int P;
int N;
// int args[64] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
int args[64] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};

char flags[NN + 1];

pthread_barrier_t barr;
pthread_mutex_t mut;
void *strike_factors(void *arg) {
    int l = *(int *)arg;
    int start, last;
    int prime = 1, i, incr, k;

    start = (l * N) / P + 1;
    last = ((l + 1) * N) / P;
    // printf("strike - Thread id : %d - start : %d - last : %d\n",l,start, last);
    do {
        // printf("Before pthread barrier wait\n");
        pthread_barrier_wait(&barr);
        // Choose the next prime number
        prime++;

        while (flags[prime] == 1 && prime < N) prime++;

        // printf("strike - prime : %d\n", prime);

        i = (start / prime) * prime;
        if (i < start) i = i + prime;
        if (i == prime) i = i + prime;

        // incr = P*prime;
        // printf("Starting to mark i = %d - prime = %d, last : %d\n", i, prime, last);
        while (i <= last) {
            // printf("Marking  : %d\n",i);
            flags[i] = 1;
            i += prime;
        }
    } while (prime <= SQRTN);  // This is to be verified

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thr[64];
    int i, mcmodel;
    struct timeval t2, t1, t3, t4;

    if (argc != 3) {
        printf("Usage: seive <NOPROC> <NELEMENTS>\n");
        exit(0);
    }

    P = atoi(argv[1]);
    N = atoi(argv[2]);
    SQRTN = sqrt((double)N);

    pthread_mutex_init(&mut, NULL);
    pthread_barrier_init(&barr, NULL, P);
    gettimeofday(&t1, NULL);
    for (i = 1; i < P; i++)
        pthread_create(&thr[i], NULL, strike_factors, &args[i]);

    gettimeofday(&t2, NULL);
    strike_factors(&args[0]);
    gettimeofday(&t3, NULL);

    for (i = 1; i < P; i++)
        pthread_join(thr[i], NULL);
    gettimeofday(&t4, NULL);

    printf("%d\n", (int)t4.tv_usec - (int)t1.tv_usec);
    return 0;
}
