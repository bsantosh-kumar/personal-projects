#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    int fd = open("file1.txt", O_WRONLY | O_APPEND);
    int temp;
    dup2(STDOUT_FILENO, temp);
    dup2(fd, STDOUT_FILENO);
    printf("This is a line\n");
    close(fd);
    dup2(0, STDOUT_FILENO);
    printf("this is after closing\n");
}