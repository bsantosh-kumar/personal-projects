#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
void checkAndPrintAbsolutePath(char *path) {
    char absolute_path[256];
    realpath(path, absolute_path);
    if (strcmp(absolute_path, path) != 0) {  //if provided path is not absolute path, print absolute path
        printf("    absolute path of the command file is %s\n\n", absolute_path);
    }
}

int max(int a, int b) {
    return a < b ? b : a;
}

void getSeparated(char ***args, int *numargs, char *line, char *delimiter) {
    char linecopy[256];
    strcpy(linecopy, line);  //bcoz strtok truncates the line and we may need the full line later

    //both space and newline as delimiters
    char *word = strtok(linecopy, delimiter);

    int maxlength = 0;
    while (word != NULL) {
        // printf("    %s\n",word);
        (*numargs)++;
        maxlength = max(maxlength, strlen(word));
        word = strtok(NULL, delimiter);
    }

    //initiliasing the 2d array to hold the words of 'line' separated by spaces
    (*args) = (char **)calloc(((*numargs) + 1), sizeof(char *));
    for (int i = 0; i < (*numargs); i++) {
        (*args)[i] = (char *)calloc((maxlength + 1), sizeof(char));
    }
    (*args)[(*numargs)] = NULL;

    // printf("2d array made\n");
    strcpy(linecopy, line);

    word = strtok(linecopy, delimiter);
    int itr = 0;
    while (word) {
        int len = strlen(word);
        strcpy(((*args)[itr]), word);
        (*args)[itr++][len] = '\0';

        word = strtok(NULL, delimiter);
    }
}

void execute_command(char **args) {
    int pid = fork();
    if (pid == 0) {
        //in child
        if (execvp(args[0], args) == -1) {
            printf("Some error occured while executing the command\n");
        }
        return;
    } else {
        wait(NULL);
    }
}

int main() {
    char line[256];
    printf("$ ");
    char *pipe_delimiter = "|\n";
    char *command_delimiter = " \n";
    while (fgets(line, 256, stdin)) {
        if (strcmp(line, "exit\n") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        if (line[0] == '\n') {
            printf("\n$ ");
            continue;
        }
        int num_commands = 0;
        char **commands;
        getSeparated(&commands, &num_commands, line, pipe_delimiter);
        printf("Number of commands separated by pipes : %d\n", num_commands);

        for (int i = 0; i < num_commands; i++) {
            printf("|%s|,", commands[i]);
        }
        printf("\n");

        int n = num_commands;
        int p[n][2];
        for (int i = 0; i < n; i++) pipe(p[i]);
        for (int i = 0; i < n; i++) {
            char **args;
            int num_args = 0;
            getSeparated(&args, &num_args, commands[i], command_delimiter);
            // printf("%d command : %s :::\n",i,args[0]);
            printf("command %s has %d arguments\n", args[0], num_args);
            for (int j = 0; j < num_args; j++) {
                printf("|%s|,", args[j]);
            }
            printf("\n");
            if (i != 0) close(p[i][1]);

            //first word will be command name
            // checkAndPrintAbsolutePath(args[0]);

            int cid = fork();
            if (cid == 0) {
                if (i != 0) {
                    dup2(p[i][0], STDIN_FILENO);
                }
                if (i != n - 1) {
                    dup2(p[i + 1][1], STDOUT_FILENO);
                }
                if (execvp(args[0], args) == -1) {
                    printf("error occured..returning");
                }
                return 0;
            } else {
                wait(NULL);
            }
            free(args);
        }
        free(commands);
        printf("\n$ ");
    }
    return 0;
}

/*
 man page for strtok function : 
 https://man7.org/linux/man-pages/man3/strtok.3.html

*/