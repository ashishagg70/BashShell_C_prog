#include"InputValidator.h"
#include"Constants.h"
#include <stdio.h>
#include<string.h>
#include<sys/wait.h> 
#include <stdlib.h>
#include"Constants.h"
#include <unistd.h>
#include<stdio.h> 
#include<fcntl.h>
#include<errno.h>
#define sizeExistingCmd 6
#define sizeNewCmd 6

int validate(char** argList, int argListSize){
    char* alreadyExistingCmdList[sizeExistingCmd] ={"cat","grep","sort","clear","ls","rm"};
    int expectedArgs=0;
    char* newCommandsToImplement[sizeNewCmd] ={"exit","checkcpupercentage","checkresidentmemory","listFiles","sortFile","executeCommands"};
    for(int i=0;i<sizeNewCmd;i++){
        if(strcmp(argList[0], newCommandsToImplement[i])==0){
            expectedArgs= getExpectedNumberOfArgs(argList);
            
            if((argListSize-1)==expectedArgs){
                if(strcmp(argList[0],"checkcpupercentage")==0 || strcmp(argList[0],"checkresidentmemory")==0)
                {
                    int pid=atoi(argList[1]);
                    if(pid==0)
                        return -1;
                    else if(kill(pid,0)==-1)
                        return -1;
                }
                argList[argListSize]=NULL;
                return expectedArgs;
            }
                
        }
    }
    
    for(int i =0; i<sizeExistingCmd;i++){
        if(strcmp(argList[0],alreadyExistingCmdList[i])==0)
        {
            //expectedArgs= getExpectedNumberOfArgs(argList);
            //if((argListSize-1)==expectedArgs){
                argList[argListSize]=NULL;
                return -2;
            //}
                
        }
    }
    
    return -1;
}


int getExpectedNumberOfArgs(char** argList)
{
    if(strcmp(argList[0], "checkcpupercentage")==0){
        return 1;
    }
    else if(strcmp(argList[0], "checkresidentmemory")==0){
        return 1;
    }
    else if(strcmp(argList[0], "listFiles")==0){
        return 0;
    }
    else if(strcmp(argList[0], "sortFile")==0){
        return 1;
    }
    else if(strcmp(argList[0], "executeCommands")==0){
        return 1;
    }
    else if(strcmp(argList[0], "cat")==0){
        return 1;
    }
    else if(strcmp(argList[0], "grep")==0){
        return 1;
    }
    else if(strcmp(argList[0], "sort")==0){
        return 0;
    }
    else if(strcmp(argList[0], "rm")==0){
        return 1;
    }
    else{
        return 0;
    }

}

int parseArg(char * input, char ** argList){
    int i=0;
    int argSize=0;
    while(input[i]!='\n'&&input[i]!='<' &&input[i]!='>')i++;
    char delm[2]={input[i],'\0'};
    input = strtok(input, delm);
 
    char * token = strtok(input, " ");
    while( token != NULL ) {
        //printf( " %s\n", token );
        if(argList!=NULL){
            argList[argSize]=token;
        }
        argSize++;
        token = strtok(NULL, " ");
      
    }
    return argSize;
}

int setupExecute(char * input, char** argList){
    int argListSize=parseArg(input, argList);
    int validateReturnValue=validate(argList, argListSize);
    if(validateReturnValue==-1){
        printf("Illegal command or arguments\n");
        return -1;
    }
    if(strcmp(argList[0],"exit")==0){
        wait(NULL);
        exit(0);
    }
    if(validateReturnValue==-2){
        return -2;
    }
    return argListSize;
}
/*async with two arg=1
async with more than two arg=-1
not async return 0
*/
int checkIfAsyncTask(char *input){
    int i=0;
    int returnVal=0;
    int count=0;
    //printf("%s",input);
    while(input[i]!='\n'){
        if(input[i]==';')
            count++;
        i++;
    }
    if(count>1){
        printf("Illegal command or arguments\n");
        returnVal=-1;
    }
    if(count==1){
        // input = strtok(input, "\n");
        // char* input1= strtok(input, ";");
        // char* input2=strtok(NULL, ";");
        // char * argList[MAX_ARGLIST_SIZE];
        // int argListSize1=validate(argList, parseArg(input1,argList));
        // int argListSize2=validate(argList, parseArg(input2,argList));
        // if(argListSize1==-1 && argListSize2==-1){
        //     printf("Illegal command or arguments\n");
        //     returnVal=-1;
        // }
        // else{
        //     returnVal=1;
        // }
        returnVal=1;

    }
    return returnVal;
}

/*
1 -->> pipe
-1 -->> more than one pipe
0 -->>> no pipe*/
int checkIfPipe(char * input){
    int i=0;
    int returnVal=0;
    int count=0;
    //printf("%s",input);
    while(input[i]!='\n'){
        if(input[i]=='|')
            count++;
        i++;
    }
    if(count>1){
        printf("Illegal command or arguments\n");
        returnVal=-1;
    }
    else if(count==1){
        returnVal=1;
    }
    return returnVal;

}
int checkIfRedirection(char * input){
    int i=0;
    int returnVal=0;
    int countappend=0;
    int countwrite=0;
    int countread=0;
    //printf("%s",input);
    while(input[i]!='\n'){
        if(input[i]=='>' && input[i+1]=='>'){
            countappend++;
            i++;
        }
        else if(input[i]=='<')
            countread++;
        else if(input[i]=='>')
            countwrite++;
        i++;
    }
    if(countappend+countwrite>1 || countread>1 ){
        printf("Illegal command or arguments\n");
        returnVal=-1;
    }
    else if(countappend+countwrite==1 || countread==1 ){
        returnVal=1;
    }
    return returnVal;

}
char** parseRedirection(char * input){
    char **inputList = malloc(3 * sizeof(char*));
    int inputLen=0;
    int outputLen=0;
    inputList[0]=inputList[1]=NULL;
    int i=0;
    while(input[i]!='\n'){
        if(input[i]=='<'){
            i++;
            while(input[i]!='\n' && input[i]==' ')i++;
            while(input[i]!='\n'&& isFilechar(input[i])){
                if(inputList[0]==NULL)
                    inputList[0] = malloc(INPUT_BUFFER_SIZE * sizeof(char));
                inputList[0][inputLen++]=input[i];
                i++;
            }
            inputList[0][inputLen]='\0';

        }
        else if(input[i]=='>'){
            i++;
            if(input[i]=='>'){
                if(inputList[2]==NULL)
                    inputList[2] = malloc(INPUT_BUFFER_SIZE * sizeof(char));
                i++;
            }
            while(input[i]!='\n' && input[i]==' ')i++;
            while(input[i]!='\n'&& isFilechar(input[i])){
                if(inputList[1]==NULL)
                    inputList[1] = malloc(INPUT_BUFFER_SIZE * sizeof(char));
                inputList[1][outputLen++]=input[i];
                i++;
            }
            inputList[1][outputLen]='\0';
        }
        if(input[i]=='\n')break;
        i++;
    }


    return inputList;
    

}

int isAlphaNumeric(char c){
    if('0'<=c && c<='9'){
        return 1;
    }
    if('a'<=c && c<='z'){
        return 1;
    }
    if('A'<=c && c<='Z'){
        return 1;
    }
    return 0;

}

int isFilechar(char c){
    if(c!='/'&&c!=' '&&c!=';'&&c!=':'&&c!='>'&&c!='<'&&c!='&'&&c!='|')
        return 1;
    return 0;
}

char * trim (char * input){
    char * inputtrimmed= malloc(INPUT_BUFFER_SIZE*sizeof(char));
    int i=0;
    int j=0;
    while(input[i]=='"'&&input[i]!='\0'&&input[i]!='\n')i++;
    while(input[i]!='"'&&input[i]!='\0'&&input[i]!='\n'){
        inputtrimmed[j]=input[i];
        j++;
        i++;
    }
    inputtrimmed[j]='\0';
    return inputtrimmed;
}


