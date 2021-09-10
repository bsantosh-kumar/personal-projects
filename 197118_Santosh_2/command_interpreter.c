#include <errno.h>
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
void createChild(char *filePath, char **allWords, int argumentLength) {
    int childPID = fork();
    if (childPID == 0) {
        printf("filePath=%s\n", filePath);
        int onError = execvp(filePath, allWords);
        printf("Error occured\n");
        exit(0);
    } else {
        wait();
    }
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
        char *commandName = (char *)calloc(strlen(allWords[0]), sizeof(char));
        strcpy(commandName, allWords[0]);
        char *filePath = NULL;
        getActualPath(commandName, &filePath);
        if (filePath == NULL) {
            printf("Command %s not found\n", commandName);
            continue;
        }
        printf("\n");
        createChild(filePath, allWords, argumentLength);
    }
}