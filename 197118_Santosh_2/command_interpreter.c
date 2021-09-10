#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
void printWelcome() {
    printf("WELCOME TO THE COMMAND INTERPRETER\n\n\n");
}
void splitInputIntoWords(char *input, int len, char ***allWords, int *argumentsLength) {
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
    char deLimit[3] = " \t\n";
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
    (*allWords) = (char **)calloc(countNoOfWords, sizeof(char *));
    for (int i = 0; i < countNoOfWords; i++) {
        (*allWords)[i] = (char *)calloc(maxLenWord + 1, sizeof(char));
    }
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
        printf("word: %s", word);
        (*allWords)[index][currLen] = '\0';
        word = strtok_r(NULL, deLimit, &savePointer2);
        index++;
    }
    printf("\n");
    (*argumentsLength) = countNoOfWords;
    return;
}
int main() {
    printWelcome();
    size_t MAX_LEN = 0;
    char *input = NULL;
    char **allWords = NULL;
    int argumentLength = 0;
    char *pwd = (char *)calloc(PATH_MAX, sizeof(char));
    char *absolutePath = (char *)calloc(PATH_MAX, sizeof(char));
    while (1) {
        getcwd(pwd, PATH_MAX);
        printf("%s $>", pwd);
        if (input)
            free(input);
        MAX_LEN = 0;
        int len = getline(&input, &MAX_LEN, stdin);                   //get input from the stdin
        splitInputIntoWords(input, len, &allWords, &argumentLength);  //This function is used to split into arguments
        if (!argumentLength) continue;                                //if there are no arguments then just continue
        printf("The arguments are:\n");
        for (int i = 0; i < argumentLength; i++) {
            printf("%s\n", allWords[i]);
        }
        printf("\n");
        if (realpath(allWords[0], absolutePath) != NULL) {  //this function is used to get the absolute path
            if (strcmp(allWords[0], absolutePath) == 0) {   //If this is true then given argument is itself absolute path
                printf("%s is the absolute path \n", allWords[0]);
            } else {
                printf("%s is not the absolute path\nThe absolute path is: %s\n", allWords[0], absolutePath);
            }
        } else {
            printf("some error occured %d\n", errno);  //some error might occur like did not find, access not given
        }
        printf("\n");
    }
}