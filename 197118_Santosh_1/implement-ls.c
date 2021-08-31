#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <langinfo.h>
#include <locale.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
struct dirent *dp;
struct stat statbuf;
struct passwd *pwd;
struct group *grp;
struct tm *tm;
char permissions[10];
char datestring[256];
void getPermissions(mode_t currMode, char *permissions) {
    for (int i = 0; i < 10; i++) permissions[i] = '-';
    if (S_ISDIR(currMode)) permissions[0] = 'd';
    int macros[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
    char symbol[] = "rwx";
    for (int i = 0; i < 9; i++) {
        if (!(currMode & macros[i])) continue;
        permissions[i + 1] = symbol[i % 3];
    }
    // printf("%s \n", permissions);
}
void printItem(struct dirent *dp, int lPresent, char *currItem) {
    if (!lPresent) {
        printf("%s ", dp->d_name);
        return;
    }
    if (stat(currItem, &statbuf) == -1)
        return;

    /* Print out type, permissions, and number of links. */
    getPermissions(statbuf.st_mode, permissions);
    // printf("%s ", permissions);
    printf("%10.10s ", permissions);
    printf("%4d ", statbuf.st_nlink);

    /* Print out owner's name if it is found using getpwuid(). */
    if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
        printf(" %-8.8s ", pwd->pw_name);
    else
        printf(" %-8d ", statbuf.st_uid);

    /* Print out group name if it is found using getgrgid(). */
    if ((grp = getgrgid(statbuf.st_gid)) != NULL)
        printf(" %-8.8s ", grp->gr_name);
    else
        printf(" %-8d ", statbuf.st_gid);

    /* Print size of file. */
    printf(" %9jd ", (intmax_t)statbuf.st_size);

    tm = localtime(&statbuf.st_mtime);

    /* Get localized date string. */
    strftime(datestring, sizeof(datestring), "%b %d %H:%M", tm);

    printf(" %s %s\n", datestring, dp->d_name);
    // printf("returning from hhere\n");
}
void ls(char *path, int aPresent, int lPresent, int RPresent) {
    struct dirent *d;
    DIR *dh = opendir(path);
    if (dh == NULL) {
        if (errno == ENOENT) {
            printf("'%s' directory doesnot exist\n", path);
        } else {
            printf("Unable to read '%s' directory\n", path);
        }
        return;
    }
    DIR *temp;
    printf("%s:\n", path);
    while ((d = readdir(dh)) != NULL) {
        if (!aPresent && d->d_name[0] == '.') continue;
        char curr[strlen(path) + 2 + strlen(d->d_name)];
        strcpy(curr, path);
        strcat(curr, "/");
        strcat(curr, d->d_name);
        printItem(d, lPresent, curr);
    }
    printf("\n \n");
    closedir(dh);
    temp = opendir(path);
    if (!RPresent) return;
    while ((d = readdir(temp)) != NULL) {
        if (strcmp(".", d->d_name) == 0 || strcmp("..", d->d_name) == 0) continue;
        struct stat s;
        char curr[strlen(path) + 2 + strlen(d->d_name)];
        strcpy(curr, path);
        if (strcmp(curr, "/") != 0)
            strcat(curr, "/");
        strcat(curr, d->d_name);
        if (stat(curr, &s) != 0) continue;
        if (!S_ISDIR(s.st_mode)) continue;
        ls(curr, aPresent, lPresent, RPresent);
    }
    closedir(temp);
}
int main(int argc, char **argv) {
    int aPresent = 0, lPresent = 0, RPresent = 0;
    int count = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') continue;
        count++;
        int len = strlen(argv[i]);
        for (int j = 1; j < len; j++) {
            if (argv[i][j] == 'a')
                aPresent = 1;
            else if (argv[i][j] == 'l')
                lPresent = 1;
            else if (argv[i][j] == 'R')
                RPresent = 1;
            else {
                printf("flag %c is not present\n", argv[i][j]);
                return 0;
            }
        }
    }
    if (count == argc) {
        ls(".", aPresent, lPresent, RPresent);
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') continue;
        ls(argv[i], aPresent, lPresent, RPresent);
    }
}