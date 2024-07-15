#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int killFun(bool signalSet, int signal, int pid) {
    if (signalSet) return kill(pid, signal);
    return kill(pid, SIGTERM);
}
int main(int argc, char **argv) {
    bool signalSet = false;
    int signal;
    int start = 1;
    if (argv[start][0] == '-') {
        signalSet = true;
        char temp[strlen(argv[start])];
        strncpy(temp, argv[start] + 1, strlen(argv[start]) - 1);
        signal = atoi(temp);
        start = 2;
    }
    printf("start::%d\n", start);
    int errno;
    for (int i = start; i < argc; i++) {
        int returnedValue = killFun(signalSet, signal, atoi(argv[i]));
        if (returnedValue == -1) {
            strerror(errno);
            if (errno == EINVAL) {
                printf("DIDNOT KILL process %s because invalid signal was specified\n", argv[i]);
            }
            if (errno == EPERM) {
                printf(
                    "DIDNOT KILL process %s because calling process does not have permission to send the signal to any of the target process\n",
                    argv[i]);
            }
            if (errno == ESRCH) {
                printf("DIDNOT KILL process %s because process does not exist\n", argv[i]);
            }

        } else {
            printf("returned value:: %d", returnedValue);
            printf("Process %s killed successfully\n", argv[i]);
        }
    }
}