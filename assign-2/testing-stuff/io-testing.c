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
    for (int i = 0; i < *argumentsLength; i++) {
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
void remove_white_spaces(char *str) {
    int i = 0, j = 0;
    while (str[i]) {
        if (str[i] != ' ')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
    return;
}
int main() {
    char command[] = "cat file1.txt > file2.txt";
    char **allWords = NULL;
    int argumentLength = 0;
    char *delimit[] = {">", " \t\n"};
    splitInputIntoWords(command, 26, &allWords, &argumentLength, delimit[0]);
    char **argv = NULL;
    int len = 0;
    splitInputIntoWords(allWords[0], strlen(allWords[0]), &argv, &len, delimit[1]);
    int child = fork();
    if (child == 0) {
        printf("arguementLength=%d\n", argumentLength);
        if (argumentLength != 1) {
            remove_white_spaces(allWords[1]);
            printf("file=%s\n", allWords[1]);
            printf("came here\n");
            ca dup2(fd, STDOUT_FILENO);
        }
        execvp(argv[0], argv);
        printf("Error occured\n");
        return 0;
    } else {
    }
}