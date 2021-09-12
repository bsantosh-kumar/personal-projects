#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main() {
    char buffer[] = "This is a line123";
    int fd = open("file1.txt", O_RDONLY);
    printf("fd=%d\n", fd);
    dup2(fd, STDIN_FILENO);
    scanf("%s", buffer);
    printf("%s\n", buffer);
    int child = fork();
    if (child == 0) {
        printf("In chlid\n");
        scanf("%s", buffer);
        printf("%s\n", buffer);
    }
}