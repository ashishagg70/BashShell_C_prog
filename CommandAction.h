#ifndef cmdaction
#define cmdaction

void executeCommand(char** argList, int argSize, int exectueWithExecutable, int pipeFd[], char* wrieFile, char *readFile, int isPipe, int isreadEnd, int append);
void execute(char *input, int pipeFd[], char* writeFile, char *readFile, int isPipe, int isreadEnd, int append);
void executeCommandAsync(char * input1, char * input2, int pipeFd[], char* writeFile, char *readFile, int isPipe, int isreadEnd, int append);
void executePipeCommand(char *input1, char* input2, int pipeFd[], char* writeFile, char *readFile, int isPipe, int isreadEnd, int append);
#endif