#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
void printWelcome() {
    printf("WELCOME TO THE COMMAND INTERPRETER\n\n\n");
}
void splitInputIntoWords(char *input, int len, char ***allWords, int *argumentsLength, int *maxLen) {
    char *word;
    char *savePointer1 = NULL;
    char *savedInput1 = (char *)calloc(len + 1, sizeof(char));
    char *savedInput2 = (char *)calloc(len + 1, sizeof(char));
    strcpy(savedInput1, input);
    strcpy(savedInput2, savedInput1);
    savedInput1[len] = savedInput2[len] = '\0';
    char deLimit[3] = " \t\n";
    word = strtok_r(savedInput1, deLimit, &savePointer1);
    int countNoOfWords = 0;
    int maxLenWord = 0;
    while (word != NULL) {
        int len = strlen(word);
        maxLenWord = max(len, maxLenWord);
        word = strtok_r(NULL, deLimit, &savePointer1);
        countNoOfWords++;
    }
    for (int i = 0; i < *argumentsLength; i++) {
        free((*allWords)[i]);
    }
    if ((*allWords)) free(*allWords);
    *allWords = NULL;
    *argumentsLength = 0;
    (*allWords) = (char **)calloc(countNoOfWords, sizeof(char *));
    for (int i = 0; i < countNoOfWords; i++) {
        (*allWords)[i] = (char *)calloc(maxLenWord + 1, sizeof(char));
    }
    char *savePointer2;
    word = strtok_r(savedInput2, deLimit, &savePointer2);
    int index = 0;
    while (word != NULL) {
        int currLen = strlen(word);
        for (int i = 0; i < currLen; i++) {
            (*allWords)[index][i] = word[i];
        }
        (*allWords)[index][currLen] = '\0';
        word = strtok_r(NULL, deLimit, &savePointer2);
        index++;
    }
    *argumentsLength = countNoOfWords;
    *maxLen = maxLenWord;
    free(savedInput2);
    return;
}
int main() {
    printWelcome();
    size_t MAX_LEN = 0;
    char *input = NULL;
    char **allWords = NULL;
    int argumentLength = 0;
    char *absolutePath = (char *)calloc(PATH_MAX, sizeof(char));
    int currPid = getpid();
    while (1) {
        printf("pid: %d $>", currPid);
        if (input)
            free(input);
        MAX_LEN = 0;
        int len = getline(&input, &MAX_LEN, stdin);
        int maxLenWord;
        splitInputIntoWords(input, len, &allWords, &argumentLength, &maxLenWord);
        if (!argumentLength) continue;
        printf("\n");
        if (strcmp(allWords[0], "exit") == 0) {
            break;
        }
        int child = fork();
        if (child != 0) {
            continue;
        } else {
            int parentPid = getppid();
            currPid = getpid();
            printf("\nCurrently in process PID: %d with parent PID: %d\n\n", currPid, parentPid);
            int grandChild = fork();
            if (grandChild != 0) {
                wait(NULL);
                printf("\n\n%d compeleted its execution\n", currPid);
                exit(2);
            } else {
                int index = 0;
                char *copyAllWords[argumentLength + 1];
                while (index < argumentLength) {
                    copyAllWords[index] = (char *)malloc(sizeof(char) * maxLenWord);
                    for (int i = 0; i < maxLenWord + 1; i++)
                        copyAllWords[index][i] = allWords[index][i];
                    index++;
                }
                copyAllWords[argumentLength] = NULL;
                execvp(copyAllWords[0], copyAllWords);

                char errorMessage[30];
                sprintf(errorMessage, "Some error occured in the process %d\n", currPid);
                perror(errorMessage);
            }
        }
    }
}