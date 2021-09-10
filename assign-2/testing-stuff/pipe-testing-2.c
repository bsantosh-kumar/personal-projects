#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    char *commands[2][3] = {{"cat", "file1.txt", (char *)NULL}, {"grep", "file1", (char *)NULL}};
    // int pre = STDIN_FILENO;
    int p[2][2];
    pipe(p[0]);
    pipe(p[1]);
    // int child = fork();
    // char ans[50];
    // if (child == 0) {
    //     write(p2[1], "This", 5);
    //     dup2(p2[0], STDIN_FILENO);
    //     char buff[26];
    //     scanf("%4s", buff);
    //     if (buff)
    //         printf("%s \n", buff);
    // } else {
    //     printf("this is parent process\n");
    // }
    for (int i = 0; i < 2; i++) {
        int child = fork();
        printf("i=%d %d\n", i, getpid());
        if (child == 0) {
            if (i != 0) {
                dup2(p[i][0], STDIN_FILENO);
            }
            printf("i+1=%d %d\n", i + 1, getpid());
            if (i + 1 != 2) {
                dup2(p[i + 1][1], STDOUT_FILENO);
            }
            printf("command[%d]=%s command[%d]=%s\n", i, commands[i][0], i, commands[i][1]);
            execvp(commands[i][0], commands[i]);
            printf("came here %d\n", getpid());
            exit(0);
        } else {
            wait(0);
        }
    }
}