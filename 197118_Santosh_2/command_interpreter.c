#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
void printWelcome() {
    printf("WELCOME TO THE COMMAND INTERPRETER\n\n\n");
}
void splitInputIntoWords(char *input, int len, char ***allWords, int *argumentsLength, char *delimiter) {
    char *word;
    char *savePointer1 = NULL;
    /*
        As the string changes with strtok_r function 
        copying it into two temporary strings
    */
    char *savedInput1 = (char *)calloc(len + 1, sizeof(char));
    char *savedInput2 = (char *)calloc(len + 1, sizeof(char));
    strcpy(savedInput1, input);
    strcpy(savedInput2, savedInput1);
    savedInput1[len] = savedInput2[len] = '\0';
    char *deLimit = delimiter;
    word = strtok_r(savedInput1, deLimit, &savePointer1);
    int countNoOfWords = 0;
    int maxLenWord = 0;
    /*
        First we will calculate the number of words, inside it
        also the maximum length of the words
    */
    while (word != NULL) {
        int len = strlen(word);
        maxLenWord = max(len, maxLenWord);
        word = strtok_r(NULL, deLimit, &savePointer1);
        countNoOfWords++;
    }
    /*Freeing all the previous words*/
    printf("countNoOfWords=%d\n", countNoOfWords);
    printf("argumentsLength=%d\n", (*argumentsLength));
    for (int i = 0; i < (*argumentsLength); i++) {
        free((*allWords)[i]);
    }
    if ((*allWords)) free(*allWords);
    (*allWords) = NULL;
    (*argumentsLength) = 0;
    (*allWords) = (char **)calloc(countNoOfWords + 1, sizeof(char *));
    for (int i = 0; i < countNoOfWords; i++) {
        (*allWords)[i] = (char *)calloc(maxLenWord + 1, sizeof(char));
    }
    (*allWords)[countNoOfWords] = NULL;
    char *savePointer2 = NULL;
    word = strtok_r(savedInput2, deLimit, &savePointer2);
    int index = 0;
    /*
        Now, we insert all the new words
    */
    while (word != NULL) {
        printf("word=%s\n", word);
        int currLen = strlen(word);
        for (int i = 0; i < currLen; i++) {
            (*allWords)[index][i] = word[i];
        }
        (*allWords)[index][currLen] = '\0';
        word = strtok_r(NULL, deLimit, &savePointer2);
        index++;
    }
    printf("\n");
    (*argumentsLength) = countNoOfWords;
    return;
}
bool checkIfFile(char *path) {
    struct stat buffer;
    int exists = stat(path, &buffer);
    if (exists == 0 && (S_IFREG & buffer.st_mode)) {
        return 1;
    }
    return 0;
}
void checkInCurrDir(char *commandName, char **filePath) {
    if (checkIfFile(commandName)) {
        char *temp = NULL;
        filePath = realpath(commandName, (temp));
    }
    return;
}
void checkInPATH(char *commandName, char **filePath) {
    char *tempPath = getenv("PATH");
    char *fullPath = (char *)calloc(strlen(tempPath), sizeof(char));
    strcpy(fullPath, tempPath);
    char *token = strtok(fullPath, ":");
    struct stat buffer;
    while (token != NULL) {
        char *currItem = (char *)calloc(strlen(commandName) + strlen(token), sizeof(char));
        sprintf(currItem, "%s/%s", token, commandName);
        if (checkIfFile(currItem)) {
            char *temp = NULL;
            (*filePath) = realpath(currItem, (temp));

            return;
        }
        token = strtok(NULL, ":");
    }
    return;
}
void getActualPath(char *commandName, char **filePath) {
    checkInCurrDir(commandName, filePath);
    if ((*filePath) != NULL) return;
    checkInPATH(commandName, filePath);

    return;
}
void createChild(char *filePath, char **allWords, int argumentLength, int index, int p[][2], int pipesLength) {
    close(p[index][1]);
    int childPID = fork();
    if (childPID == 0) {
        printf("filePath=%s index=%d\n", filePath, index);
        if (index != 0) {
            dup2(p[index][0], STDIN_FILENO);
        }
        if (index + 1 != pipesLength) {
            dup2(p[index + 1][1], STDOUT_FILENO);
        }
        int onError = execvp(allWords[0], allWords);
        printf("Error occured\n");
        exit(0);
    } else {
        wait();
    }
}
void removeWhiteSpaces(char *str) {
    int i = 0, j = 0;
    while (str[i] != '\0') {
        printf("%d ", str[i]);
        if (str[i] != ' ' && str[i] != '\n')
            str[j++] = str[i];
        i++;
    }
    printf("j=%d\n", j);
    str[j] = '\0';
    printf("RETURNING: \"%s\"\n", str);
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
    // if (noOfFiles[1] + noOfFiles[2] == 0) {
    //     tempFileOutFD = STDOUT_FILENO;
    // }
    int child = fork();
    if (child != 0) {
        wait();
        // if (noOfFiles[1] + noOfFiles[2] == 0) {
        //     exit(0);
        // }
        printf("CAME HERE\n");
        for (int i = 1; i <= 2; i++) {
            lseek(tempFileOutFD, 0, SEEK_SET);
            char c;
            int count = 1;
            printf("noOfFiles[%d]=%d\n", i, noOfFiles[i]);
            for (int j = 0; j < noOfFiles[i]; j++) {
                printf("Writing to %d\n", fileDescpt[i][j]);
                lseek(tempFileOutFD, 0, SEEK_SET);
                while ((count = read(tempFileOutFD, &c, 1)) > 0)
                    write(fileDescpt[i][j], &c, 1);
            }
            if (i == 1) {
                int j = noOfFiles[i];
                lseek(tempFileOutFD, 0, SEEK_SET);
                printf("index=%d n=%d\n", index, n);
                count = 1;
                if (index + 1 == n && noOfFiles[1] + noOfFiles[2] == 0) {
                    printf("IN THIS ONE\n");
                    while ((count = read(tempFileOutFD, &c, 1)) > 0)
                        write(STDOUT_FILENO, &c, 1);
                    continue;
                }
                while ((count = read(tempFileOutFD, &c, 1)) > 0)
                    write(fileDescpt[i][j], &c, 1);
                c = EOF;
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
        printf("index=%d n=%d\n", index, n);
        if (index == 0) {
            printf("Came here for input\n");
            if (noOfFiles[0] == 0)
                tempFileInFD = STDIN_FILENO;
        } else {
            char c;
            while ((count = read(fileDescpt[0][noOfFiles[0]], &c, 1)) == 1 && c != -1) {
                printf("%c", c);
                write(tempFileInFD, &c, 1);
            }
            printf("\n");
        }
        printf("Came out of if\n");
        lseek(tempFileInFD, 0, SEEK_SET);
        dup2(tempFileOutFD, STDOUT_FILENO);
        if (!(index == 0 && noOfFiles[0] == 0))
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
void executeCommand(char **allWords, int argumentLength, int index, int p[][2], int pipesLength) {
    if (!argumentLength) return;  //if there are no arguments then just continue
    printf("The arguments are:\n");
    for (int j = 0; j < argumentLength; j++) {
        printf("%s\n", allWords[j]);
    }
    // printf("\n");
    char *commandName = (char *)calloc(strlen(allWords[0]), sizeof(char));
    strcpy(commandName, allWords[0]);
    char *filePath = NULL;
    getActualPath(commandName, &filePath);
    if (filePath == NULL) {
        printf("Command %s not found\n", commandName);
        return;
    }
    printf("\n");
    createChild(filePath, allWords, argumentLength, index, p, pipesLength);
    close(p[index][0]);
}
void startCommandExecution(char *input, int len) {
    char **allWords = NULL;
    char **allPipes = NULL;
    int pipesLength = 0;
    char *delimiterForCommand = " \t\n";
    char *delimiterForPipe = "|";

    splitInputIntoWords(input, len, &allPipes, &pipesLength, delimiterForPipe);
    printf("came to start command execution\n");
    int p[pipesLength + 1][2];
    for (int i = 0; i < pipesLength + 1; i++) {
        pipe(p[i]);
        if (i != pipesLength)
            printf("%s\n", allPipes[i]);
    }
    for (int pipeIndex = 0; pipeIndex < pipesLength; pipeIndex++) {
        close(p[pipeIndex][1]);
        int argumentLength = 0;
        char *command = (char *)calloc(strlen(allPipes[pipeIndex]), sizeof(char));
        strcpy(command, allPipes[pipeIndex]);
        char **allFiles[3];
        int noOfFiles[3] = {0};
        int *fileDescpt[3] = {0};
        char *actualCommand = "";
        getAllFileDescriptors(command, &actualCommand, allFiles, noOfFiles, fileDescpt);
        // removeWhiteSpaces(actualCommand);
        printf("\"%s\"\n", actualCommand);
        char **allArguments = NULL;
        splitInputIntoWords(actualCommand, strlen(actualCommand), &allArguments, &argumentLength, delimiterForCommand);
        printf("Came IN INPUT\n");
        for (int i = 0; i < argumentLength; i++) {
            printf("'%s' ", allArguments[i]);
        }
        printf("\n");
        fileDescpt[0][noOfFiles[0]] = p[pipeIndex][0];
        fileDescpt[1][noOfFiles[1]] = p[pipeIndex + 1][1];
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
            completeCommandExecutionWithRedirection(allArguments[0], fileDescpt, noOfFiles, allFiles, allArguments, pipeIndex, pipesLength);
            // printf("IN CHILD\n");
            // execvp(arguments[0], arguments);
        } else {
            wait();
        }
        // splitInputIntoWords(allPipes[i], len, &allWords, &argumentLength, delimiterForCommand);  //This function is used to split into arguments
        // if (!argumentLength) continue;                                                           //if there are no arguments then just continue
        // printf("The arguments are:\n");
        // for (int j = 0; j < argumentLength; j++) {
        //     printf("%s\n", allWords[j]);
        // }
        // printf("\n");
        // char *commandName = (char *)calloc(strlen(allWords[0]), sizeof(char));
        // strcpy(commandName, allWords[0]);
        // char *filePath = NULL;
        // getActualPath(commandName, &filePath);
        // if (filePath == NULL) {
        //     printf("Command %s not found\n", commandName);
        //     continue;
        // }
        // printf("\n");
        // createChild(filePath, allWords, argumentLength);
        // executeCommand(allWords, argumentLength, i, p, pipesLength);
    }
}
int main() {
    printWelcome();
    size_t MAX_LEN = 0;
    char *input = NULL;
    char **allWords = NULL;
    char **allPipes = NULL;
    int pipesLength = 0;
    int argumentLength = 0;
    char *pwd = (char *)calloc(PATH_MAX, sizeof(char));
    char *absolutePath = (char *)calloc(PATH_MAX, sizeof(char));
    char *delimiterForCommand = " \t\n";
    char *delimiterForPipe = "|";
    while (1) {
        printf("cmd >", pwd);
        if (input)
            free(input);
        MAX_LEN = 0;
        int len = getline(&input, &MAX_LEN, stdin);  //get input from the stdin
        //     splitInputIntoWords(input, len, &allPipes, &pipesLength, delimiterForPipe);
        //     int p[pipesLength][2];
        //     for (int i = 0; i < pipesLength; i++) {
        //         pipe(p[i]);
        //         printf("%s\n", allPipes[i]);
        //     }
        //     for (int i = 0; i < pipesLength; i++) {
        //         splitInputIntoWords(allPipes[i], len, &allWords, &argumentLength, delimiterForCommand);  //This function is used to split into arguments
        //         // if (!argumentLength) continue;                                                           //if there are no arguments then just continue
        //         // printf("The arguments are:\n");
        //         // for (int j = 0; j < argumentLength; j++) {
        //         //     printf("%s\n", allWords[j]);
        //         // }
        //         // printf("\n");
        //         // char *commandName = (char *)calloc(strlen(allWords[0]), sizeof(char));
        //         // strcpy(commandName, allWords[0]);
        //         // char *filePath = NULL;
        //         // getActualPath(commandName, &filePath);
        //         // if (filePath == NULL) {
        //         //     printf("Command %s not found\n", commandName);
        //         //     continue;
        //         // }
        //         // printf("\n");
        //         // createChild(filePath, allWords, argumentLength);
        //         executeCommand(allWords, argumentLength, i, p, pipesLength);
        //     }
        bool bgExec = false;
        for (int i = len - 1; i >= 0; i--) {
            if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n')
                continue;
            else {
                if (input[i] == '&') bgExec = true;
                break;
            }
        }
        int startChild = fork();
        if (startChild == 0)
            startCommandExecution(input, len);
        else {
            if (!bgExec) {
                wait();
            }
        }
    }
}