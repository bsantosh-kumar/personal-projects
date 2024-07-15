#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
void removeWhiteSpaces(char *str) {
    int i = 0, j = 0;
    while (str[i]) {
        if (str[i] != ' ')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
    return;
}
void segregateFiles(char *command, int noOfFiles[], char **allFiles[]) {
    char *savedInput1 = (char *)calloc(strlen(command), sizeof(char));
    char *savedInput2 = (char *)calloc(strlen(command), sizeof(char));
    strcpy(savedInput1, command);
    strcpy(savedInput2, command);
    int index = 0;
    int maxLen = 0;
    // noOfFiles[0] = noOfFiles[1] = 1;
    int lenOfString = strlen(command);
    for (; index < lenOfString; index++) {
        if (savedInput1[index] == ' ' || savedInput1[index] == '\t' || savedInput1[index] == '\n') continue;
        int j = index;
        printf("index=%d\n", index);

        int count = 0;
        for (j = index; j < lenOfString; j++) {
            if (savedInput1[j] != '>') {
                if (savedInput1[j] == '<') j++;
                break;
            } else {
                printf("j as;lfjka %d\n", j);
            }
            count = count + 1;
            printf("count=%d\n", count);
        }
        if (count > 3) {
            printf("There is some error here %s\n", command);
            exit(0);
        }
        while (++j && j < lenOfString) {
            if (savedInput1[j] == ' ' || savedInput1[j] == '\t' || savedInput1[j] == '\n')
                continue;
            else
                break;
        }
        printf("j here=%d\n", j);
        index = j;
        printf("count=%d index=%d\n", count, index);
        noOfFiles[count]++;
        while (j + 1 < lenOfString && savedInput1[j + 1] != ' ' && savedInput1[j + 1] != '>' && savedInput1[j + 1] != '<') j++;
        printf("j=%d\n", j);
        maxLen = max(maxLen, j - index + 1);
        index = j;
    }
    for (int i = 0; i < 3; i++) {
        allFiles[i] = (char **)calloc(noOfFiles[i], sizeof(char *));
        for (int j = 0; j < noOfFiles[i]; j++) {
            allFiles[i][j] = (char *)calloc(maxLen + 1, sizeof(char));
        }
    }
    index = 0;
    int temp[3] = {0};
    printf("came until here\n");
    for (; index < lenOfString; index++) {
        if (savedInput1[index] == ' ' || savedInput1[index] == '\t' || savedInput1[index] == '\n') continue;
        int count = 0;
        int j = index;
        printf("index=%d\n", index);
        for (j = index; j < lenOfString; j++) {
            if (savedInput1[j] != '>') {
                if (savedInput1[j] == '<') j++;
                break;
            } else {
                printf("j as;lfjka %d\n", j);
            }
            count = count + 1;
            printf("count=%d\n", count);
        }
        if (count > 3) {
            printf("There is some error here %s\n", command);
            exit(0);
        }
        while (++j && j < lenOfString) {
            if (savedInput1[j] == ' ' || savedInput1[j] == '\t' || savedInput1[j] == '\n')
                continue;
            else {
                j--;
                break;
            }
        }
        printf("j here=%d\n", j);
        index = j;
        printf("count=%d index=%d\n", count, index);
        int currIndex = temp[count];
        while (j + 1 < lenOfString && savedInput1[j + 1] != ' ' && savedInput1[j + 1] != '>' && savedInput1[j + 1] != '<') {
            j++;
        }
        printf("j=%d\n", j);
        for (int k = index; k <= j; k++) {
            allFiles[count][currIndex][k - index] = savedInput1[k];
        }
        allFiles[count][currIndex][j + 1 - index] = '\0';
        removeWhiteSpaces(allFiles[count][currIndex]);
        temp[count]++;
        maxLen = max(maxLen, j - index + 1);
        index = j;
    }
}
int getCommand(char *input, int len) {
    for (int i = 0; i < len; i++) {
        if (input[i] == '>' || input[i] == '<') return i;
    }
    return len;
}
void getFDs(char **allFiles[], int noOfFiles[], int *fileDescpt[]) {
    fileDescpt[0] = (int *)calloc(noOfFiles[0] + 1, sizeof(int));
    fileDescpt[1] = (int *)calloc(noOfFiles[1] + 1, sizeof(int));
    fileDescpt[2] = (int *)calloc(noOfFiles[2], sizeof(int));
    for (int i = 0; i < noOfFiles[0]; i++) {
        int fd = open(allFiles[0][i], O_RDONLY);
        if (fd == -1) {
            printf("cannot open the file %s\n", allFiles[0][i]);
            // exit(0);
        }
        fileDescpt[0][i] = fd;
    }
    for (int i = 0; i < noOfFiles[1]; i++) {
        {
            FILE *f;
            f = fopen(allFiles[1][i], "w");
            fclose(f);
        }
        int fd = open(allFiles[1][i], O_WRONLY | O_APPEND);
        if (fd == -1) {
            printf("cannot open the file %s\n", allFiles[1][i]);
            // exit(0);
        }
        fileDescpt[1][i] = fd;
    }
    for (int i = 0; i < noOfFiles[2]; i++) {
        {
            FILE *f;
            printf("allFiles=%s\n", allFiles[2][i]);
            f = fopen(allFiles[2][i], "a");
            fclose(f);
        }
        int fd = open(allFiles[2][i], O_WRONLY | O_APPEND);
        if (fd == -1) {
            printf("cannot open the file %s\n", allFiles[2][i]);
            // exit(0);
        }
        fileDescpt[2][i] = fd;
    }
}
void getAllFileDescriptors(char *command, char **actualCommand, char **allFiles[], int noOfFiles[], int *fileDescpt[]) {
    int index = getCommand(command, strlen(command));
    char *fileString = command + index;
    (*actualCommand) = (char *)calloc(index + 1, sizeof(char));
    for (int i = 0; i < index; i++) {
        (*actualCommand)[i] = command[i];
    }
    (*actualCommand)[index] = '\0';
    printf("fileString=%s command=%s\n", fileString, command);
    segregateFiles(fileString, noOfFiles, allFiles);
    for (int i = 0; i < 3; i++) {
        printf("length[%d]=%d\n", i, noOfFiles[i]);
        for (int j = 0; j < noOfFiles[i]; j++) {
            printf("%s\n", allFiles[i][j]);
        }
        printf("done\n");
    }
    getFDs(allFiles, noOfFiles, fileDescpt);
}
void completeCommandExecutionWithRedirection(char *actualCommand, int *fileDescpt[], int noOfFiles[], char **allFiles[], char *arguments[], int index, int n) {
    char tempFileOut[] = "/tmp/tempFile-XXXXXX";
    int tempFileOutFD = mkstemp(tempFileOut);
    char tempFileIn[] = "/tmp/tempFile-XXXXXX";
    int tempFileInFD = mkstemp(tempFileIn);
    printf("Over here look at me!!\n");
    unlink(tempFileIn);
    unlink(tempFileOut);
    if (noOfFiles[1] + noOfFiles[2] == 0) {
        tempFileOutFD = STDOUT_FILENO;
    }
    int child = fork();
    if (child != 0) {
        wait();
        if (noOfFiles[1] + noOfFiles[2] == 0) {
            exit(0);
        }
        printf("CAME HERE\n");
        for (int i = 1; i <= 2; i++) {
            lseek(tempFileOutFD, 0, SEEK_SET);
            char c;
            int count = 1;
            for (int j = 0; j < noOfFiles[i]; j++) {
                lseek(tempFileOutFD, 0, SEEK_SET);
                while ((count = read(tempFileOutFD, &c, 1)) > 0)
                    write(fileDescpt[i][j], &c, 1);
            }
            if (i == 1) {
                int j = noOfFiles[i];
                lseek(tempFileOutFD, 0, SEEK_SET);
                printf("index=%d n=%d\n", index, n);
                count = 1;
                if (index + 1 == n) {
                    while ((count = read(tempFileOutFD, &c, 1)) > 0)
                        printf("%c", c);
                    continue;
                }
                while ((count = read(tempFileOutFD, &c, 1)) > 0)
                    write(fileDescpt[i][j], &c, 1);
            }
        }
        // continue;
    } else {
        printf("tempFileOutFD=%d\n", tempFileOutFD);
        int count = 1;
        for (int i = 0; i < 1; i++) {
            char c;
            for (int j = 0; j < noOfFiles[i]; j++) {
                if (fileDescpt[i][j] < 0) continue;
                while ((count = read(fileDescpt[i][j], &c, 1)) > 0)
                    write(tempFileInFD, &c, 1);
            }
        }
        if (index == 0) {
            printf("Came here for input\n");
            if (noOfFiles[0] == 0)
                tempFileInFD = STDIN_FILENO;
        } else {
            char c;
            while ((count = read(fileDescpt[0][noOfFiles[0]], &c, 1)) > 0)
                write(tempFileInFD, &c, 1);
        }
        printf("Came out of if\n");
        lseek(tempFileInFD, 0, SEEK_SET);
        dup2(tempFileOutFD, STDOUT_FILENO);
        dup2(tempFileInFD, STDIN_FILENO);
        execvp(actualCommand, arguments);
        printf("Some error occured while executing\n");
        exit(0);
        // for (int k = 0; k < noOfFiles[0] + 1; k++) {
        //     if (k != noOfFiles[0])
        //         printf("%s GETTING IT\n", allFiles[0][k]);
        //     else
        //         printf("bla bla bla\n");
        //     int grand = fork();
        //     if (grand != 0) {
        //         wait();
        //         continue;
        //     }
        //     int fd = open(allFiles[0][k], O_RDONLY);
        //     if (k == noOfFiles[0] && index == 0) {
        //         if (noOfFiles[0] != 0) exit(0);
        //         printf("no of files are zero\n");
        //         printf("actualCommand=%s\n", actualCommand);
        //         execvp(actualCommand, arguments);
        // printf("Some error occured while executing with input file %s\n", fileDescpt[0][k]);
        // exit(0);
        //     }
        //     dup2(fd, STDIN_FILENO);
        //     // printf("\t input=%s\n", allFiles[0][k]);
        //     execvp(actualCommand, arguments);
        //     printf("Some error occured while executing with input file %s\n", fileDescpt[0][k]);
        //     exit(0);
        // }
    }
}
int main() {
    int p[2][2];
    pipe(p[0]);
    pipe(p[1]);
    int pipeIndex = 0;
    int pipeLength = 5;
    char *command = "cat<file1.txt >file2.txt<file5.txt>>file3.txt>>file4.txt";
    char **allFiles[3];
    int noOfFiles[3] = {0};
    int *fileDescpt[3] = {0};
    char *actualCommand = "";
    getAllFileDescriptors(command, &actualCommand, allFiles, noOfFiles, fileDescpt);
    printf("\"%s\"\n", actualCommand);
    removeWhiteSpaces(actualCommand);
    char *arguments[] = {actualCommand, NULL};
    fileDescpt[0][noOfFiles[0]] = p[0][0];
    fileDescpt[1][noOfFiles[1]] = p[1][1];
    for (int i = 0; i < 2; i++) {
        printf("%d %d\n", p[i][0], p[i][1]);
    }
    printf("\"%s\"\n", actualCommand);
    // close(p[0][1]);
    for (int i = 0; i < 3; i++) {
        printf("i=%d ", i);
        for (int j = 0; j < noOfFiles[i]; j++) {
            printf("%d ", fileDescpt[i][j]);
        }
        if (i == 0 || i == 1) {
            printf("%d ", fileDescpt[i][noOfFiles[i]]);
        }
        printf("\n");
    }
    int child = fork();
    if (child == 0) {
        printf("Came INTO CHILD\n");
        completeCommandExecutionWithRedirection(arguments[0], fileDescpt, noOfFiles, allFiles, arguments, pipeIndex, pipeLength);
        // printf("IN CHILD\n");
        // execvp(arguments[0], arguments);
    } else {
        wait();
    }
}