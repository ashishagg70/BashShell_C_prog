#include "CommandAction.h"
#include"CustomCommandHelper.h"
#include"InputValidator.h"
#include"FileOperation.h"
#include <stdio.h>
#include<string.h>
#include<sys/wait.h> 
#include <stdlib.h>
#include"Constants.h"
#include <unistd.h>
#include<stdio.h> 
#include<fcntl.h>
#include<errno.h>
void execute(char *input, int pipeFd[], char* writeFile, char *readFile, int isPipe, int isreadEnd, int append){
    char * argList[MAX_ARGLIST_SIZE];
    char * copyInput=malloc (INPUT_BUFFER_SIZE * sizeof(char));
    char * input1=malloc (INPUT_BUFFER_SIZE * sizeof(char));
    char * input2=malloc (INPUT_BUFFER_SIZE * sizeof(char));


    strcpy(copyInput, input);
    //printf("input to asyncTas: %s",copyInput)
    int asyncTask=checkIfAsyncTask(copyInput);
    //printf("asyncTask: %d\n",asyncTask);
    if(asyncTask==1){
        strcpy(copyInput, input);
        char *tempinput1= strtok(copyInput, ";");
        char * tempinput2=strtok(NULL, ";");
        //printf("tempinput1: %s\n",tempinput1);
        //printf("tempinput2: %s\n",tempinput2);
        strcpy(input1,tempinput1);
        strcpy(input2,tempinput2);
        strcat(input1,"\n");
        executeCommandAsync(input1, input2, pipeFd, writeFile, readFile, isPipe, isreadEnd, append);
        return;
    }
    else if (asyncTask==-1){
        return;
    }

    strcpy(copyInput, input);
    int pipeStatement= checkIfPipe(copyInput);
    if(pipeStatement==1){
        strcpy(copyInput, input);
        char *tempinput1= strtok(copyInput, "|");
        char * tempinput2=strtok(NULL, "|");
        //printf("tempinput1: %s\n",tempinput1);
        //printf("tempinput2: %s\n",tempinput2);
        strcpy(input1,tempinput1);
        strcpy(input2,tempinput2);
        strcat(input1,"\n");
        executePipeCommand(input1,input2,pipeFd, writeFile, readFile, isPipe, isreadEnd, append);
       // executeCommandAsync(input1, input2);
        return;
    }
    else if(pipeStatement==-1){
        return;
    }
    strcpy(copyInput, input);
    int redirectStatement=checkIfRedirection(copyInput);
    if(redirectStatement==-1)
        return;
    else if(redirectStatement==1){
        strcpy(copyInput, input);
        char **redirectBuffer=parseRedirection(copyInput);
        readFile=redirectBuffer[0];
        if(readFile!=NULL){
            int readFd=open(readFile, O_RDONLY);
            if(readFd<0){
                printf("Illegal command or arguments\n");
                return;
            }
        }
        writeFile=redirectBuffer[1];
        append=(redirectBuffer[2]!=NULL);
        
        
    }
    

    strcpy(copyInput, input);
    int argListSize=setupExecute(copyInput, argList);
    if(argListSize==-1)
        return;
    if(argListSize==-2){
        executeCommand(argList, argListSize,1, pipeFd, writeFile,readFile,isPipe,isreadEnd,append);
        return;
    }
    if(strcmp(argList[0],"executeCommands")==0){
        FILE *fileFd = fopen(argList[1], "r");
        if(fileFd==NULL){
            printf("%s\n",strerror(errno));
            return;
        }
        char command[MAX_NO_OF_COMMANDS][INPUT_BUFFER_SIZE];
        size_t length = 0;
        char c;
        int i=0,j=0;
        //command=malloc (INPUT_BUFFER_SIZE * sizeof(char));
        while((c = fgetc(fileFd)) != EOF) {
            //printf("%c %d",c,c);
            command[j][i++] = c;
            if (c == '\n'){
                command[j][i++]='\n';
                command[j][i++]='\0';
                i=0;
                static int y=0;
                //printf("callingexecute: %d %s\n",y,command[j]);
                j++;
                y++;
            }
        }
        int max=j;
        if(writeFile!=NULL && append!=1){
            int writeFd=open(writeFile, O_CREAT|O_TRUNC|O_WRONLY|O_CLOEXEC, 0666);
            close(writeFd);
        }
        append=1;
        while(j){
            execute(command[max-j],pipeFd,writeFile,NULL,isPipe,isreadEnd,append);
            j--;
        }
        fclose(fileFd);
    }
    else
    {
        //printf("command: %s  append: %d\n", copyInput, append);
        executeCommand(argList, argListSize,0, pipeFd, writeFile,readFile,isPipe,isreadEnd,append);
    }
    if(copyInput)
        free(copyInput);
    if(input1)
        free(input1);
    if(input2)
        free(input2);
            
}


void executeCommand(char** argList, int argSize, int exectueWithExecutable, int *pipeFd, char* writeFile, char *readFile, int isPipe, int isReadEnd,int append){
  
    if(strcmp(argList[0],"checkresidentmemory")==0 && (pipeFd==NULL ||(writeFile==NULL && isPipe && isReadEnd))){
        int fd[2];
        pipeFd=fd;
        if(pipe(pipeFd)<0){
            printf("%s\n",strerror(errno));
        }
    }
    int child;
    int childExitStatus=0;
    child=fork();
    if(child==0){
        if(exectueWithExecutable==1){
            executeSystemCommand(argList, pipeFd, writeFile, readFile, isPipe, isReadEnd, append);
        }
        else if(strcmp(argList[0],"checkcpupercentage")==0)
            calculateCpuPercentage(argList[1],pipeFd, writeFile, isPipe, isReadEnd, append);
        else if(strcmp(argList[0],"checkresidentmemory")==0){
            //printf("enterd child");
            printRSSforPid(argList[1], pipeFd, writeFile, isPipe, isReadEnd, append);
        }
        else if(strcmp(argList[0],"listFiles")==0){
            executeListFilesCommand(pipeFd, writeFile, readFile, isPipe, isReadEnd, append);
        }
        else if(strcmp(argList[0],"sortFile")==0){
            executeSortFilesCommand(argList[1],pipeFd, writeFile, isPipe, isReadEnd, append);
        }
        exit(0);
    }
    else if(child>0){
        wait(NULL);
        //printf("out of chld");
        if(strcmp(argList[0],"checkresidentmemory")==0){
            if(writeFile==NULL && (!isPipe||(isReadEnd&&isPipe))){
                //printf("p1, ispipe: %d,isreadEnd: %d \n", isPipe, isReadEnd);
                close(pipeFd[1]);
                char residentMemory[10];
                read(pipeFd[0], residentMemory, 10);
                printf("%d\n",atoi(residentMemory));
                close(pipeFd[0]);
            }
            else if(writeFile!=NULL&&(!isPipe || (isReadEnd && isPipe))){
                //printf("p2, ispipe: %d,isreadEnd: %d \n", isPipe, isReadEnd);
                char *arg[6]={"sed","-r","s/\\s+//g","-i",writeFile,NULL};
                executeCommand(arg, 6,1,NULL, NULL,NULL, 0,0,0);
            }
        }
    }
    else{
        printf("%s\n",strerror(errno));
    }
}


void executeCommandAsync(char * input1, char * input2, int pipeFd[], char* writeFile, char *readFile, int isPipe, int isreadEnd, int append){
    int child1=0;
    int child2=0;
    int exitstatus=0;
    child1=fork();
    if(child1==0){
        execute(input1,pipeFd,writeFile,readFile,isPipe,isreadEnd,append);
        exit(0);
    }
    else if(child1>0){
        child2=fork();
        if(child2==0){
            execute(input2,pipeFd,writeFile,readFile,isPipe,isreadEnd,append);
            exit(0);
        }
        else if(child2>0){
            waitpid(child1, NULL, 0);
            waitpid(child2, NULL, 0);
        }
        else{
            printf("%s\n",strerror(errno));
        }
        //execute(input2);
        //wait(NULL);
        //waitpid(child, &exitstatus, 0);
    }
    else{
        printf("%s\n",strerror(errno));
    }
}


void executePipeCommand(char *input1, char* input2, int pipeFd[], char* writeFile, char *readFile, int isPipe, int isreadEnd, int append){
    //printf("%s",input1);
    //printf("%s",input2);
    if(pipeFd==NULL)
    {
        int pipeFd1[2];
        pipeFd=pipeFd1;
        if(pipe(pipeFd)<0)
            printf("%s\n",strerror(errno));
    }
    execute(input1, pipeFd, writeFile,NULL, 1,0,append); //0 means write end
    close(pipeFd[1]);
    execute(input2, pipeFd, writeFile,NULL,1,1,append);
    close(pipeFd[0]);


}





