#include <stdio.h>
#define MAX_LIMIT 4096
void cat(char *filePrint) {
    FILE *fi = fopen(filePrint, "r");
    if (fi == NULL) {
        printf("Not able to open the file \"%s\"\n", filePrint);
        return;
    }
    printf("%s : \n", filePrint);
    char c;
    while ((c = fgetc(fi)) != EOF) {
        printf("%c", c);
    }
    printf("\n");
}
int main(int argc, char **argv) {
    if (argc == 1) {
        while (1) {
            char buff[MAX_LIMIT];
            fgets(buff, MAX_LIMIT, stdin);
            printf("%s", buff);
        }
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        cat(argv[i]);
    }
    return 1;
}
