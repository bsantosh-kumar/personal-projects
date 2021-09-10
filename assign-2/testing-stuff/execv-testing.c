#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    char *argv[] = {"ls", "-l", NULL};
    int fd = open("file1.txt", O_WRONLY);
    dup2(fd, STDOUT_FILENO);
    int child = fork();
    int x;
    if (child == 0) {
        x = execvp("ls", argv);
        printf("x=%d \n", x);
    } else
        printf("This %d\n", x);
    // printf("HERE %d\n", child);
}