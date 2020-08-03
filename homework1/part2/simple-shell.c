#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CHAR_SIZE 80 

int fetchInputString(char *bufferString) {
    char buffer;
    int countChar = 0;

    while (((buffer = getchar()) != '\n') && countChar < MAX_CHAR_SIZE + 1) {
     	bufferString[countChar++] = buffer;
    }

    if (buffer != '\n' && countChar == MAX_CHAR_SIZE) {
 	printf("ERROR: Command length exceeds maximum\n");

    	return -1;
    } else {
     	bufferString[countChar] = 0;
    }

    return countChar;
}

void printHistory(char history[5][MAX_CHAR_SIZE + 1], int countHistory) {
    int i;
    int j;

    if (!countHistory) {
     	printf("ERROR: No history exists\n");

    	return;
    }

    for (i = countHistory, j = 5; i > 0 && j > 0; i--, j--) {
    	printf("%5d %s\n", i, history[i % 5]);
    }
}

int parseArgument(char *bufferString, int length, char **argumentString) {
    int countArgument = 0;
    int indexPreargumentLastChar = -1;
    int i;

    argumentString[0] = NULL;
	
    for (i = 0; i <= length; i++) {
    	if (bufferString[i] && !isspace(bufferString[i])) {
	    continue;
	} else {
	    if (indexPreargumentLastChar != i - 1) {
		argumentString[countArgument] = (char *) malloc(sizeof(char) * (i - indexPreargumentLastChar));

		if (argumentString[countArgument] == NULL) {
		    printf("ERROR: Failed to allocate memory\n");

		    return 1;
		}

		memcpy(argumentString[countArgument],
		    &bufferString[indexPreargumentLastChar + 1],
		    i - indexPreargumentLastChar - 1
		);

		argumentString[countArgument++][i - indexPreargumentLastChar] = 0;
		argumentString[countArgument] = NULL;
	    }

	    indexPreargumentLastChar = i;
	}
    }

    return countArgument;
}

int createChildProcess(char **argumentString, int isBackgroundProcess) {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
	printf("ERROR: Failed to create process\n");

	return 1;
    }

    int childPid;

    if (pid == 0) {
	childPid = execvp(argumentString[0], argumentString);

	if (childPid == -1) {
	    printf("ERROR: Failed to execute the command\n");
	}

	return 0;
    } else {
	if (isBackgroundProcess == 0) {
	    wait(&childPid);
	}
    }
}

int main(void) {
    char bufferString[MAX_CHAR_SIZE + 1];
    int countHistory = 0;
    char historyString[5][MAX_CHAR_SIZE + 1];
    int countArgument;
    char *argumentString[MAX_CHAR_SIZE / 2 + 1];

    memset(bufferString, 0, sizeof(bufferString));

    while (1) {
        printf("simple-shell> ");
	fflush(stdout);
		
	int bufferStringLength = fetchInputString(bufferString);

	if (bufferStringLength == -1) {
	    continue;
	}

	if (strcmp(bufferString, "!!") == 0) {
	    if (countHistory == 0) {
		printf("ERROR: No command exists in history\n");

		continue;
	    }
			
	    memcpy(bufferString, historyString[countHistory % 5], MAX_CHAR_SIZE + 1);

	    bufferStringLength = strlen(bufferString);
	}
		
	countArgument = parseArgument(bufferString, bufferStringLength, argumentString);

	if (countArgument == 0) {
	    continue;
	}

	if (argumentString[0][0] == '!') {
	    int historyTargetNumber = atoi(&argumentString[0][1]);

	    if (historyTargetNumber <= 0
	    || historyTargetNumber < countHistory - 4
	    || historyTargetNumber > countHistory
            ) {
		printf("ERROR: No such command exists in history\n");
                
		continue;
	    }

	    countHistory++;

	    memcpy(bufferString, historyString[historyTargetNumber % 5], MAX_CHAR_SIZE + 1);

	    bufferStringLength = strlen(bufferString);
	    countArgument = parseArgument(bufferString, bufferStringLength, argumentString); 
	}

	if (strcmp(argumentString[0], "exit") == 0) {
	    return 0;
	}

	if (strcmp(argumentString[0], "history") == 0) {
	    printHistory(historyString, countHistory);

	    memcpy(historyString[++countHistory % 5], bufferString, MAX_CHAR_SIZE + 1);

            continue;
	}

	memcpy(historyString[++countHistory % 5], bufferString, MAX_CHAR_SIZE + 1);

	int isBackgroundProcess = 0;

	if (strcmp(argumentString[countArgument - 1], "&") == 0) {
	    isBackgroundProcess = 1;
	    argumentString[countArgument-- - 1] = NULL;
	}

	createChildProcess(argumentString, isBackgroundProcess);
    }

    return 0;
}
