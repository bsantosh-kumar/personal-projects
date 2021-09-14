#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    int p[2];
    pipe(p);
    write(p[1], "This", 5);
    char a[5];
    write(p[1], "isll", 3);
    read(p[0], a, 5);
    printf("a=%s\n", a);
    fseek(p[0], 0, SEEK_SET);
}