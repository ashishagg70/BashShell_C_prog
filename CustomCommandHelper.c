#include "CustomCommandHelper.h"
#include "FileOperation.h"
#include"InputValidator.h"
#include"Constants.h"
#include<stdio.h> 
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include <unistd.h>
void calculateCpuPercentage(char* pid, int pipeFd[], char *writeFile,int isPipe, int isReadEnd, int append){
    if(isPipe&&!isReadEnd){
        handleRedirections(pipeFd,NULL,NULL, isPipe, isReadEnd, -1);
    }
    else if(writeFile!=NULL){
        handleRedirections(pipeFd,writeFile,NULL,0,0,append);
    }
    char *procProcessStatFile=malloc (PROCESS_STAT_FILE_PATH_STRING_SIZE * sizeof (char));
    procProcessStatFile[0]='\0';
    char procStatFile[PROCESS_STAT_FILE_PATH_STRING_SIZE]="/proc/stat";
    strcat(procProcessStatFile,"/proc/");
    strcat(procProcessStatFile,pid);
    strcat(procProcessStatFile,"/stat");
    //printf("%s",procProcessStatFile);
    int fd=open(procProcessStatFile,O_RDONLY|O_CLOEXEC);
    if(fd<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
    else
        close(fd);
    long int utimes=readNthNumber(procProcessStatFile, 14);
    long int stimes=readNthNumber(procProcessStatFile, 15);
    long int totalcputickss= sumNumberInRange(procStatFile,2,11);
    sleep(CPU_UTILIZATION_SAMPLE_INTERVAL);
    long int utimef=readNthNumber(procProcessStatFile, 14);
    long int stimef=readNthNumber(procProcessStatFile, 15);
    long int totalcputicksf= sumNumberInRange(procStatFile,2,11);
    //int noOfCPUCore = getNumberOfCpuCore();
    double userUtilTemp=(100.0*(utimef-utimes))/(CPU_UTILIZATION_SAMPLE_INTERVAL*(totalcputicksf-totalcputickss));
    double systemUtilTemp=(100.0*(stimef-stimes))/(CPU_UTILIZATION_SAMPLE_INTERVAL*(totalcputicksf-totalcputickss));
    //printf("end of cpupercentage function");
    printf("user mode cpu percentage: %lf\n",userUtilTemp);
    printf("system mode cpu percentage: %lf\n",systemUtilTemp);
    fflush(stdout);
    free(procProcessStatFile);
    exit(0);
    

}


void executeListFilesCommand(){
    //printf("nes: %d",newFileFd);
    int newFileFd=open("files.txt", O_CREAT|O_TRUNC|O_WRONLY|O_CLOEXEC, 0666);
    if(newFileFd<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
    fflush(stdout);
    //close(1);
    if(dup2(newFileFd,1)<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
    close(newFileFd);
    char* args[]={"ls",NULL};
    int error = execvp(args[0],args);
    if(error<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
}

void executeSortFilesCommand(char * filename, int pipeFd[], char *writeFile,int isPipe, int isReadEnd, int append){
    if(isPipe&&!isReadEnd){
        handleRedirections(pipeFd,NULL,NULL, isPipe, 0, -1);
    }
    else if(writeFile!=NULL){
        handleRedirections(pipeFd,writeFile,NULL,0,0,append);
    }
    
    int fileFd=open(filename, O_RDONLY|O_CLOEXEC);
    if(fileFd<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
    fflush(stdin);
    if(dup2(fileFd,0)<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
    close(fileFd);
    char* args[]={"sort",NULL};
    int error= execvp(args[0],args);
    if(error<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
}

void executeSystemCommand(char **argList,int *pipeFd, char *writeFile, char* readFile, int isPipe, int isReadEnd, int append){
    
    //printf("\npipefd[1]: %d %s\n",pipeFd[1], argList[0]);
    
    // if(isPipe && !isReadEnd){
    //     close(pipeFd[0]);
    //     if(dup2(pipeFd[1],1)<0){
    //         printf("error: %s\n",strerror(errno));
    //         exit(errno);
    //     }
    // }
    if(argList[1]!=NULL)
        argList[1]=trim(argList[1]);
    //printf("arg1: %s\n",argList[1]);
    handleRedirections(pipeFd, writeFile,readFile, isPipe, isReadEnd, append);
    execvp(argList[0],argList);
    printf("%s\n",strerror(errno));
    exit(errno);
}

//ps -fp 1 -o rss= 
//OR
//ps -o rss= 1
void printRSSforPid(char *s, int pipeFd[], char *writeFile,int isPipe, int isReadEnd, int append){
    
    if(writeFile!=NULL && (!isPipe||(isReadEnd &&isPipe))){
        handleRedirections(pipeFd,writeFile,NULL,0,0,append);//write to file
    }
    else if(writeFile==NULL && isPipe && isReadEnd){
        handleRedirections(pipeFd,NULL,NULL, 1, 0, -1);
    }
    else{
        handleRedirections(pipeFd,NULL,NULL, 1, 0, -1);//write to pipe
    }

    char *args[]={"ps","-fp",s,"-o","rss=",NULL};
    // close(writefd[0]);
    // //char *envVar[]={"PATH=/bin:/usr/bin",NULL};

    // if(dup2(writefd[1],1)<0){
    //     printf("%s\n",strerror(errno));
    //     exit(errno);
    // }
    // close(writefd[1]);
    if(execvp(args[0],args)<0){
        printf("%s\n",strerror(errno));
        exit(errno);
    }
    exit(0);

}

void handleRedirections(int *pipeFd, char *writeFile, char* readFile, int isPipe, int isReadEnd, int append){
    
    if(readFile!=NULL){
        int readFd=0;
        readFd=open(readFile, O_RDONLY|O_CLOEXEC);
        if(readFd<0)
        {
            printf("%s\n",strerror(errno));
            exit(errno);
        }
        if(dup2(readFd,0)<0){
            printf("%s\n",strerror(errno));
            exit(errno);
        }
        close(readFd);
    }
    
    if(writeFile!=NULL){
        int writeFd=0;
        if(append==0)
            writeFd=open(writeFile, O_CREAT|O_TRUNC|O_WRONLY|O_CLOEXEC, 0666);
        else
            writeFd=open(writeFile, O_CREAT|O_APPEND|O_WRONLY|O_CLOEXEC, 0666);
        if(writeFd<0)
        {
            printf("%s\n",strerror(errno));
            exit(errno);
        }
        if(dup2(writeFd,1)<0){
            printf("%s\n",strerror(errno));
            exit(errno);
        }
        close(writeFd);
    }
    

    if(isPipe && isReadEnd){
        close(pipeFd[1]);
        if(dup2(pipeFd[0], 0)<0){
            printf("%s\n",strerror(errno));
            exit(errno);
        } 
        close(pipeFd[0]);
    }
    
    if(isPipe && !isReadEnd){
        close(pipeFd[0]);
        if(dup2(pipeFd[1],1)<0){
            printf("error: %s\n",strerror(errno));
            exit(errno);
        }
        close(pipeFd[1]);
    }
}
