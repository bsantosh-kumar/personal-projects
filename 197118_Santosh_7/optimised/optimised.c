#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <sys/stat.h>
#include<stdbool.h>
#include<unistd.h>
#include<string.h>
FILE *getInputFD(char InputFileName[]){
    FILE *inputFile=fopen(InputFileName,"r");
    if (inputFile==NULL) {
        printf("%s file is not present\n",InputFileName);
        exit(0);
    }
    return inputFile;
}
FILE *getOutputFD(char outputFileName[]){    
    FILE *outputFile=fopen(outputFileName,"w");
    if(outputFile==NULL){
        printf("%s file cannot be opened\n",outputFileName);
        exit(0);
    }
    return outputFile;
}
void printAlreadyPresent(int currPage,FILE *outputFile){
    fprintf(outputFile,"%d is already present in the main memory....\n",currPage);
}
void printAddingNewPage(int currPage,FILE *outputFile){
    fprintf(outputFile,"%d is brought in...\n",currPage);
}
void printReplacedPage(int removedPage,int currPage,FILE *outputFile){
    fprintf(outputFile,"%d is being removed, and %d is brought in...\n",removedPage,currPage);
}
bool findInFrameList(int frame[],int currPage,int size){
    for(int i=0;i<size;i++){
        if(frame[i]==currPage) return 1;
    }
    return 0;
}
int deleteFromFrameList(int frame[],int allPages[],int noOfFrames,int noOfPages,int index){
    int maxTime=0;
    int maxIndex=-1;
    for(int i=0;i<noOfFrames;i++){
        bool flag=true;
        for(int j=index;j<noOfPages;j++){
            if(allPages[j]==frame[i]){
                    // printf("For %d maxTime is: %d, maxTime:%d\n",frame[i],j,maxTime);
                if(j>maxTime){
                    maxIndex=i;
                    maxTime=j;
                }
                flag=false;
                break;
            }
        }
        if(flag) return i;
    }
    if(maxIndex==-1) return 0;
    return maxIndex;
}
void printPagesInMemory(int frame[],int size,FILE *outputFile){
    fprintf(outputFile,"The pages in memory are: ");
    for(int i=0;i<size;i++){
        fprintf(outputFile,"%d ",frame[i]);
    }
    fprintf(outputFile,"\n");
}
void getAllPages(int allPages[],int noOfPages,FILE *inputFile){
    for(int i=0;i<noOfPages;i++){
        fscanf(inputFile,"%d",&allPages[i]);
    }
}
int findNoOfPageFaults(int noOfFrames,int noOfPages,FILE *inputFile,FILE *outputFile){
    int frame[noOfFrames];
    int allPages[noOfPages];
    getAllPages(allPages,noOfPages,inputFile);
    int noOfPageDefaults=0;
    int currSize=0;
    for(int i=0;i<noOfPages;i++){
        int currPage=allPages[i];
        fprintf(outputFile,"Current page is:%d\n",currPage);
        bool currNode=findInFrameList(frame,currPage,currSize);
        if(currNode==true) {
            printAlreadyPresent(currPage,outputFile);
        }
        else{
            noOfPageDefaults++;
            if(currSize<noOfFrames){
                frame[currSize++]=currPage;
                printAddingNewPage(currPage,outputFile);
            }
            else{
                int nextFrameIndex=deleteFromFrameList(frame,allPages,noOfFrames,noOfPages,i+1);
                int removedPage=frame[nextFrameIndex];
                frame[nextFrameIndex]=currPage;
                printReplacedPage(removedPage,currPage,outputFile);
            }
        }
        printPagesInMemory(frame,currSize,outputFile);
    }
    return noOfPageDefaults;
}
int calculateNoOfPages(FILE *inputFile){
    int count=0;
    int temp=0;
    while(!feof(inputFile)){
        fscanf(inputFile,"%d",&temp);
        count++;
    }
    fseek(inputFile,0,SEEK_SET);
    return count;
}
int main(int argc,char *argv[]){
    if(argc!=3){
        printf("The input format is ./lru <input-file> <output-file>\n");
        exit(0);
    }
    FILE *inputFile=getInputFD(argv[1]);
    FILE *outputFile=getOutputFD(argv[2]);
    //no of frames
    int noOfFrames=3; //just a constant
    //no of pages
    int noOfPages=calculateNoOfPages(inputFile);

    int noOfPageFaults=findNoOfPageFaults(noOfFrames,noOfPages,inputFile,outputFile);
    fprintf(outputFile,"No of page faults are: %d\n",noOfPageFaults);
}