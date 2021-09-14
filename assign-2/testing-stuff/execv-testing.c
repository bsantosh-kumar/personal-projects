#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    char *argv[] = {"cat", NULL};
    int child = fork();
    int x;
    if (child == 0) {
        printf("Camer here\n");
        x = execvp(argv[0], argv);
        printf("x=%d \n", x);
    } else {
        wait();
        printf("This %d\n", x);
    }
    // printf("HERE %d\n", child);
}