#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(int argc, char **argv) {
    if (argc == 1) {
        return 0;
    }
    char *commandName = calloc(strlen(argv[1]), sizeof(char));
    strcpy(commandName, argv[1]);
    char *fullPath = getenv("PATH");
    char *token = strtok(fullPath, ":");
    struct stat buffer;
    while (token != NULL) {
        char *currItem = calloc(strlen(commandName) + strlen(token), sizeof(char));
        sprintf(currItem, "%s/%s", token, commandName);
        int exists = stat(currItem, &buffer);
        if (exists == 0 && (S_IFREG & buffer.st_mode)) {
            printf("%s exists here\n");
            return 0;
        }
        token = strtok(NULL, ":");
    }
    return 0;
}