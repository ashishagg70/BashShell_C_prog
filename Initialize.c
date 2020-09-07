#include"CommandAction.h"
#include"FileOperation.h"
#include"Constants.h"
#include"SignalHandler.h"
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
int main(){
    signal(SIGINT, signalHandler);
    signal(SIGTERM, orderToKill);
    char* input = NULL;
    char * copyInput=malloc (INPUT_BUFFER_SIZE * sizeof(char));
    //printf("%d",getpid());
    while(1){
        printf("myShell> ");
        fflush(stdout);
        fflush(stdin);
        input = readLinefromFile(input);
        if(input[0]=='\n'){
            //printf("Illegal command or arguments\n");
            continue;
        }
        strcpy(copyInput, input);
        execute(copyInput,NULL,NULL,NULL,0,0,-1);

    }//While loop bracket
    if(input)
        free(input);
    if(copyInput)
        free(copyInput);
    return 0;
}