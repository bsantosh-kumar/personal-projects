#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
int main() {
    int parentPid = getpid();
    int pipeFile[2];
    int pipeContent[2];
    pipe(pipeFile);
    pipe(pipeContent);
    sem_t *ready = sem_open("Ready", O_CREAT, 0600, 0);
    sem_t *writeDone = sem_open("Write Done", O_CREAT, 0600, 0);
    // sem_init(&ready, 1, 0);
    // sem_init(&writeDone, 1, 0);
    int childPid = fork();
    if (childPid == 0) {
        char fileName[PATH_MAX];
        sem_wait(writeDone);
        read(pipeFile[0], fileName, PATH_MAX);
        dup2(pipeContent[1], STDOUT_FILENO);
        char *allWords[] = {"cat", fileName, NULL};
        execvp(allWords[0], allWords);
        exit(0);
    } else {
        printf("Created a process with pid:%d\n", childPid);
        char fileName[PATH_MAX];
        for (int i = 0; i < PATH_MAX; i++) fileName[i] = '\0';
        printf("Enter fileName:");
        scanf("%s", fileName);
        write(pipeFile[1], fileName, PATH_MAX);
        sem_post(writeDone);
        char c;
        wait(NULL);
        close(pipeContent[1]);
        while (read(pipeContent[0], &c, 1) != 0) {
            printf("%c", c);
        }
    }
}