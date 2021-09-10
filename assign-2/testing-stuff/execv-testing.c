#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    char *argv[] = {"cd", "..", NULL};
    int child = fork();
    int x;
    if (child == 0) {
        printf("Camer here\n");
        x = execvp("cd", argv);
        printf("x=%d \n", x);
    } else
        printf("This %d\n", x);
    // printf("HERE %d\n", child);
}