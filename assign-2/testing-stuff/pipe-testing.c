#include <stdio.h>
#include <unistd.h>
int main() {
    int p[2];
    pipe(p);
    char buff[23] = "This is a message";
    write(p[1], buff, 17);

    char ans[23];
    read(p[0], ans, 17);
    printf("%s\n", ans);
    sprintf(buff, "This is second message");
    write(p[1], buff, 23);
    read(p[0], ans, 23);
    printf("%s\n", ans);
}