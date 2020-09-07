#include"Constants.h"
#include"CustomCommandHelper.h"
#include"FileOperation.h"
#include<stdlib.h>
#include <unistd.h>
#include<stdio.h> 
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h> 

int main(int argc,char* argv[]){
    int child1=0;
    //printf("%s\n",argv[0]);
    child1=fork();
    fflush(stdout);
    if(child1==0){
        if(argc>1 && argv[2]!=NULL){
            int newFileFd=-1;
            if(argc>2 && argv[3]!=NULL && strcmp(argv[3],"append")==0){
                newFileFd=open(argv[2], O_WRONLY|O_APPEND|O_CREAT, 0666);
            }
            else{
                newFileFd=open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0666);
            }
            fflush(stdout);
            close(1);
            dup2(newFileFd,1);
        }
        // if(argc>3 && argv[4]!=NULL){
        //     int fileFd=open(argv[4], O_RDONLY);
        //     fflush(stdin);
        //     close(0);
        //     dup2(fileFd,0);
        // }
        calculateCpuPercentage(argv[1]);
    }
    else if(child1>0){
        wait(NULL);
    }
}
