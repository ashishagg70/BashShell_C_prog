#ifndef commandhelper
#define commandhelper
void calculateCpuPercentage(char* pid, int pipeFd[], char *writeFile,int isPipe, int isReadEnd, int append);
void printRSSforPid(char *s, int pipeFd[], char *writeFile,int isPipe, int isReadEnd, int append);
void executeListFilesCommand();
void executeSortFilesCommand(char * filename, int pipeFd[], char *writeFile,int isPipe, int isReadEnd, int append);
void handleRedirections(int pipeFd[], char *writeFile, char* readFile, int isPipe, int isReadEnd, int append);
void executeSystemCommand(char **argList, int pipeFd[], char *writeFile, char* readFile,int isPipe, int isReadEnd, int append);
#endif