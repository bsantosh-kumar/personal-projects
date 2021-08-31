#include<stdio.h>
#include<string.h>
#include<stdbool.h>
bool findAllVariables(char *fileName,int countAll[]){
	FILE *fi=fopen(fileName,"r");	
	if(fi==NULL){
		printf("Not able to open the file %s\n", fileName);
		return false;
	}
	char buff[256];
	char curr,pre=' ';
	while((curr=fgetc(fi))!=EOF){
		countAll[2]++;
		if((pre==' '||pre=='\n')&&curr!=' '&&curr!='\n'){
			countAll[1]++;	
		}
		if(curr=='\n'){
			countAll[0]++;
		}
		pre=curr;
	}
	fclose(fi);
	return true;

}
int main(int argc,char **argv){
	if(argc==0){
		printf("No arguments provided\n");
		return 1;
	}
	int countFiles=0;
	bool flags[3];
	flags[0]=flags[1]=flags[2]=false;
	bool flagsPresent=false;
	for(int i=1;i<argc;i++){
		if(argv[i][0]!='-'){
			countFiles++;
			continue;
		}
		if(strlen(argv[i])<2) continue;
		flagsPresent=true;
		for(int j=1;j<strlen(argv[i]);j++){
			if(argv[i][j]=='c'){
				flags[2]=true;
			}
			else if(argv[i][j]=='w'){
				flags[1]=true;
			} else if(argv[i][j]=='l'){
				flags[0]=true;
			}
			else {
				printf("No '%c' flag found\n",argv[i][j]);
				return 0;
			}
		}
	}
	if(!flagsPresent){
		flags[0]=flags[1]=flags[2]=true;
	}
	int calculateTotal[3]={0,0,0};
	for(int i=1;i<argc;i++){
		int countAll[3]={0,0,0};
		if(argv[i][0]=='-') continue;
		bool successful=findAllVariables(argv[i],countAll);
		if(!successful) continue;
		printf("    ");
		for(int j=0;j<3;j++){
			calculateTotal[j]+=countAll[j];
			if(flags[j]){
				printf("%d ",countAll[j]);
			}
		}
		printf("%s\n",argv[i]);
	}
	if(countFiles==1)
		return 0;
	printf("    ");
	for(int j=0;j<3;j++){
		if(flags[j]){
			printf("%d ",calculateTotal[j]);
		}
	}
	printf("total\n");
	return 1;
}
