#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <sys/stat.h>
#include<unistd.h>
#include<string.h>
#include "frame_list.h"
void redirectInputOutput(char InputFile[],char outputFile[]){
    int fdInput=open(InputFile,O_RDONLY);
    struct stat buf;
    if(strcmp(InputFile,outputFile)==0){
        printf("Input and output files cannot be same\n");
        exit(0);
    }
    if (fstat(fdInput, &buf) == -1) {
        printf("%s file is not present\n",InputFile);
        exit(0);
    }
    int fdOutput=creat(outputFile,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fdOutput==-1){
        printf("%s file cannot be opened\n",outputFile);
        exit(0);
    }
    dup2(fdInput,STDIN_FILENO);
    dup2(fdOutput,STDOUT_FILENO);
}
void printAlreadyPresent(int currPage){
    printf("%d is already present in the main memory....\n",currPage);
}
void printAddingNewPage(int currPage){
    printf("%d is brought in...\n",currPage);
}
void printReplacedPage(int removedPage,int currPage){
    printf("%d is being removed, and %d is brought in...\n",removedPage,currPage);
}
void printPagesInMemory(FrameList *frame){
    printf("The pages in memory are: ");
    printFrameList(frame);
    printf("\n");
}
int findNoOfPageFaults(int noOfFrames,int noOfPages){
    FrameList *frame=malloc(sizeof(FrameList));
    intializeFrameList(frame);
    int noOfPageDefaults=0;
    for(int i=0;i<noOfPages;i++){
        int currPage;
        scanf("%d",&currPage);
        printf("Current page is:%d\n",currPage);
        lptr currNode=findInFrameList(frame,currPage);
        if(currNode!=NULL) {
            deleteFromFrame(frame,&currNode);
            addToFrameList(frame,currPage);
            printAlreadyPresent(currPage);
        }
        else{
            noOfPageDefaults++;
            if(frame->nfe<noOfFrames){
                addToFrameList(frame,currPage);
                printAddingNewPage(currPage);
            }
            else{
                int removedPage=deleteLRU(frame);
                printReplacedPage(removedPage,currPage);
                addToFrameList(frame,currPage);
            }
        }
        printPagesInMemory(frame);
    }
    return noOfPageDefaults;
}
int main(int argc,char *argv[]){
    if(argc!=3){
        printf("The input format is ./lru <input-file> <output-file>\n");
        exit(0);
    }
    redirectInputOutput(argv[1],argv[2]);
    //no of frames
    int noOfFrames;
    scanf("%d",&noOfFrames);
    //no of pages
    int noOfPages;
    scanf("%d",&noOfPages);
    int noOfPageFaults=findNoOfPageFaults(noOfFrames,noOfPages);
    printf("No of page faults are: %d\n",noOfPageFaults);
}