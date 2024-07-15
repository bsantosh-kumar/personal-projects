
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define FORMAT "%5s %10s\t%8s %s\n"

int main(int argc, char** argv) {
    struct dirent* d;
    DIR* dh = opendir("/proc");
    if (dh == NULL) {
        if (errno == ENOENT) {
            printf("'/proc doesnot exist\n");
        } else {
            printf("unable to read /proc directory\n");
        }
        return 0;
    }
    printf(FORMAT, "pid", "TTY", "TIME", "CMD");
    char currPath[256], statFile[256];
    char ttySelf[256];
    char* ttyCurr;
    char processName[256], currTime[256];
    int pid, utime, stime, runningTime;
    char temp[256];
    int fdSelf, fdCurr;
    FILE* currStat;
    fdSelf = open("/proc/self/fd/0", O_RDONLY);
    ttyname_r(fdSelf, ttySelf, 256);
    while (d = readdir(dh)) {
        int currLen = strlen(d->d_name);
        bool flag = true;
        for (int i = 0; i < currLen; i++) {
            if (!isdigit(d->d_name[i])) {
                flag = false;
                break;
            }
        }
        if (!flag) continue;
        sprintf(currPath, "/proc/%s/fd/0", d->d_name);
        fdCurr = open(currPath, O_RDONLY);
        ttyCurr = ttyname(fdCurr);
        if (1) {
            sprintf(statFile, "/proc/%s/stat", d->d_name);
            currStat = fopen(statFile, "r");
            fscanf(currStat, "%d%s%c%c%c", &pid, processName, &flag, &flag, &flag);
            for (int i = 1; i <= 10; i++) {
                fscanf(currStat, "%s ", temp);
            }
            fscanf(currStat, "%d", &utime);
            fscanf(currStat, "%d", &stime);
            runningTime = (int)((double)(utime + stime)) / sysconf(_SC_CLK_TCK);
            sprintf(currTime, "%02d:%02d:%02d", (runningTime / 3600) % 3600, (runningTime / 60) % 60, runningTime % 60);
            char pidString[256];
            sprintf(pidString, "%d", pid);
            processName[strlen(processName) - 1] = '\0';
            if (ttyCurr)
                printf(FORMAT, pidString, ttyCurr + 5, currTime, processName + 1);
            // else
            //     printf(FORMAT, pidString, "?", currTime, processName + 1);

            // printf("%s here \n", d->d_name);
            fclose(currStat);
        }
        close(fdCurr);
    }
    close(fdSelf);
}