#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    int fd = open("temp1.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    printf("%d\n", fd);
}