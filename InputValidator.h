#ifndef validator
#define validator
int parseArg(char * input, char ** argList);
int getExpectedNumberOfArgs(char** argList);
int validate(char** argList, int argListSize);
int checkIfAsyncTask(char *input);
int checkIfPipe(char * input);
int checkIfRedirection(char * input);
char** parseRedirection(char * input);
int setupExecute(char * input, char** argList);
int isAlphaNumeric(char c);
int isFilechar(char c);
char * trim (char * input);
#endif