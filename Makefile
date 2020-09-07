output: InputValidator.o Initialize.o FileOperation.o CustomCommandHelper.o CommandAction.o SignalHandler.o
	gcc InputValidator.o Initialize.o FileOperation.o CustomCommandHelper.o CommandAction.o SignalHandler.o -o myShell

Initialize.o: Initialize.c
	gcc -c Initialize.c

InputValidator.o: InputValidator.c InputValidator.h
	gcc -c InputValidator.c

FileOperation.o: FileOperation.c FileOperation.h
	gcc -c FileOperation.c

CustomCommandHelper.o: CustomCommandHelper.c CustomCommandHelper.h
	gcc -c CustomCommandHelper.c

CommandAction.o: CommandAction.c CommandAction.h
	gcc -c CommandAction.c

SignalHandler.o: SignalHandler.c SignalHandler.h
	gcc -c SignalHandler.c

clean:
	rm *.o myShell
target: dependencies
	action


