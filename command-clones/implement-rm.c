#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
void removeFun(char *file) {
    if (remove(file) == 0) {
        printf("Deleted %s\n", file);
    } else {
        printf("Cannot delete \"%s\"(file may not be present)\n", file);
    }
}
int main(int argc, char **argv) {
    if (argc == 0) {
        printf("rm: missing operand\nTry \"rm --help\" for more information.\n");
        return 1;
    }
    bool isInteractive = false;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') continue;
        if (strlen(argv[i]) < 2) continue;
        for (int j = 1; argv[i][j] != '\0'; j++) {
            if (argv[i][j] == 'i')
                isInteractive = true;
            else {
                printf("flag %c not defined\n", argv[i][j]);
                return 0;
            }
        }
    }
    bool deleteIt = true;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') continue;
        struct stat st;
        if (stat(argv[i], &st) != 0) {
            printf("FILE %s doesnot exist\n", argv[i]);
            continue;
        }
        if (!S_ISREG(st.st_mode)) {
            printf("CANNOT REMOVE %s, as it is a ", argv[i]);
            if (S_ISDIR(st.st_mode)) {
                printf("directory ");
            }
            if (S_ISBLK(st.st_mode)) {
                printf("block device ");
            }
            if (S_ISCHR(st.st_mode)) {
                printf("character device ");
            }
            if (S_ISFIFO(st.st_mode)) {
                printf("pipe ");
            }
            if (S_ISLNK(st.st_mode)) {
                printf("link ");
            }
            printf("\n");
            continue;
        }
        if (isInteractive) {
            do {
                deleteIt = false;
                printf("Are you sure you want to delete \"%s\" ?(y/N)\n", argv[i]);
                char str[5];
                scanf("%5s", str);
                char del;
                if (strlen(str) > 1) {
                    del = 'z';
                } else
                    del = str[0];
                if (del == 'Y' || del == 'y') {
                    deleteIt = true;
                    break;
                } else if (del == 'N' || del == 'n') {
                    deleteIt = false;
                    break;
                }
                printf("Invalid character detected, please try again\n");
                printf("argv=%s", argv[i]);
            } while (1);
        }
        if (deleteIt)
            removeFun(argv[i]);
        else {
            printf("%s is skipped\n", argv[i]);
        }
    }
}
