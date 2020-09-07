#include "FileOperation.h"
#include"Constants.h"
#include <stdio.h> 
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
long int readNthNumber(char * path, int n){
    //printf("%s\n",path);
    int fd = open(path, O_RDONLY);
    n=n-1;
    int count=0;
    char c;
    while(count<n){
        read(fd,&c,1);
        if (isspace(c)!=0){
            do{
                read(fd,&c,1);
            }while(isspace(c)!=0);
            count++;
        }
    }
    long int ans=0;
    do{
        int ctoint= c-'0';
        ans=ans*10+ctoint;
        read(fd,&c,1);
        if(isspace(c)!=0)
            break;
    }while (1);
    close(fd);
    return ans;
}


long int sumNumberInRange(char * path, int s , int f){
    //printf("%s\n",path);
    int fd = open(path, O_RDONLY);
    s=s-1;

    int count=0;
    char c;
    while(count<s){
        read(fd,&c,1);
        if (isspace(c)!=0){ //!=0 means it is a space
            do{
                read(fd,&c,1);
            }while(isspace(c)!=0);
            count++;
        }
    }
    long int sum=0;
    do{
        long int ans=0;
        do{
            int ctoint= c-'0';
            ans=ans*10+ctoint;
            read(fd,&c,1);
            if(isspace(c)!=0)
            {
                do{
                    read(fd,&c,1);
                }while(isspace(c)!=0);
                count++;
                break;
            }
        }while (1);
        //printf("ans: %ld",ans);
        sum+=ans;
    }while(count<f);
    close(fd);
    return sum;
}

int getNumberOfCpuCore(){
    return sysconf(_SC_NPROCESSORS_ONLN);
}

char * readLinefromFile(char * input){
    input = malloc (INPUT_BUFFER_SIZE * sizeof(char));
    int i=0;
    char c;
    while((c = fgetc(stdin)) != EOF) {
        input[i++] = c;
        if (c == '\n')
            break;
    }
    if(c==EOF){
        input[i++]=EOF;
    }
    else{
        input[i++]='\0';
    }
    return input;

}