
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
int SIZE; /* Size of matrices */
int N;    /* number of threads */
// int thread_args[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int thread_args[64] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                       32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};

int **A, **B, **C;
pthread_barrier_t barr;

void fill_matrix(int** m) {
    int i, j, n = 0;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            m[i][j] = n++;
}
void init_matrix(int*** m) {
    (*m) = (int**)calloc(SIZE, sizeof(int*));
    for (int i = 0; i < SIZE; i++) (*m)[i] = (int*)calloc(SIZE, sizeof(int));
}
void print_matrix(int m[SIZE][SIZE]) {
    int i, j = 0;

    for (i = 0; i < SIZE; i++) {
        printf("\n\t| ");
        for (j = 0; j < SIZE; j++)
            printf("%2d ", m[i][j]);
        printf("|");
    }
}

void* mmult(void* slice) {
    int s = *(int*)slice;
    int from = (s * SIZE) / N;     /* note that this 'slicing' works fine */
    int to = ((s + 1) * SIZE) / N; /* even if SIZE is not divisible by N */
    int i, j, k;

    pthread_barrier_wait(&barr);
    // printf("computing slice %d (from row %d to %d)\n", s, from, to - 1);
    for (i = from; i < to; i++) {
        if (i % 10 == 0) {
            // printf("Computing: %d\n", i);
        }
        for (j = 0; j < SIZE; j++) {
            C[i][j] = 0;
            for (k = 0; k < SIZE; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    }

    // printf("finished slice %d\n", s);
    return 0;
}

int main(int argc, char* argv[]) {
    pthread_t thread[64];
    int i, mcmodel;
    clock_t t1, t2, t3, t4;

    if (!(argc == 3)) {
        printf("Usage: %s <NPROC> <SIZE>\n", argv[0]);
        exit(-1);
    }
    N = atoi(argv[1]);
    SIZE = atoi(argv[2]);
    init_matrix(&A);
    init_matrix(&B);
    init_matrix(&C);
    fill_matrix(A);
    fill_matrix(B);
    pthread_barrier_init(&barr, NULL, N);
    t1 = clock();
    for (i = 1; i < N; i++) {
        if (pthread_create(&thread[i], NULL, mmult, (void*)&thread_args[i]) != 0) {
            printf("Can't create thread");
            exit(-1);
        }
    }
    /* master thread is thread 0 so: */
    t2 = clock();
    mmult(&thread_args[0]);
    t3 = clock();
    // printf("Waiting for the child threads\n");
    for (i = 1; i < N; i++) pthread_join(thread[i], NULL);
    t4 = clock();
    double timeTaken = (t4 - t1 + 0.0) / CLOCKS_PER_SEC;
    timeTaken = timeTaken * 1000000.0;
    printf("%lf\n", timeTaken);

    //  printf("\n\n");
    // print_matrix(A);
    // printf("\n\n\t       * \n");
    // print_matrix(B);
    // printf("\n\n\t       = \n");
    // print_matrix(C);
    // printf("\n\n");

    return 0;
}
