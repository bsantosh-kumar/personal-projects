#include <stdio.h>
void cp(char *from, char *to) {
    FILE *fi1 = fopen(from, "r");
    FILE *fi2 = fopen(to, "w");
    char buff[256];
    while (fgets(buff, 255, fi1)) {
        fprintf(fi2, buff);
    }
    fclose(fi2);
    fclose(fi1);
}
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Expected command line arguments 2, found %d \n", argc - 1);
        printf("Syntax is ./cp-clone <filename to copy from> <one or more filenames to copy to>\n");
        return 1;
    }
    if (fopen(argv[1], "r") == NULL) {
        printf("Not able to open file \"%s\" \nPlease check if it is present\n", argv[1]);
        return 0;
    }
    for (int i = 2; i < argc; i++) {
        cp(argv[1], argv[i]);
    }
    return 1;
}
