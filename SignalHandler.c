#include "SignalHandler.h"
#include<string.h>
#include <unistd.h>
#include<stdlib.h>
void signalHandler(int signal){
    char *output="the program is interrupted, do you want to exit [Y/N]";
    int len=strlen(output);
    write(1, output, len+1);
    char c;
    read(0,&c,1);
    if(c=='Y' || c=='y'){
        exit(0);
    }
}

void orderToKill(int signal){
    char *output="Got SIGTERM-Leaving";
    int len=strlen(output);
    write(1, output, len+1);
    exit(signal);
}