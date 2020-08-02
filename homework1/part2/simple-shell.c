#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CHARACTER_SIZE 80 

int fetchInputString(char *bufferString) {
	char charBuffer;
	int countChar = 0;

	while (((charBuffer = getchar()) != '\n') && (countChar < MAX_CHARACTER_SIZE + 1)) {
	 	bufferString[countChar] = charBuffer;
		countChar++;
	}

	if (charBuffer != '\n' && countChar == MAX_CHARACTER_SIZE) {
	 	printf("ERROR: Command length exceeds maximum\n");

		return -1;
	} else {
	 	bufferString[countChar] = 0;
	}

	return countChar;
}

void printHistory(char history[5][MAX_CHARACTER_SIZE + 1], int countHistory) {
    int i;
    int j;

 	if (countHistory == 0) {
	 	printf("ERROR: No history exists\n");

		return;
	}

	for (i = countHistory, j = 5; i > 0 && j > 0; i--, j--) {
		printf("%5d %s\n", i, history[i % 5]);
	}
}

int parseArgument(char *buffer, int length, char **argument) {
 	int countArgument = 0;
    int indexPreargumentLastChar = -1;
    int i;

	argument[0] = NULL;
	
	for (i = 0; i <= length; ++i) {
		if (buffer[i] && !isspace(buffer[i])) {
			continue;
		}
		else {
			if (indexPreargumentLastChar != i - 1) {
				argument[countArgument] = (char *) malloc(sizeof(char) * (i - indexPreargumentLastChar));

				if (argument[countArgument] == NULL) {
					printf("ERROR: Failed to allocate memory\n");

					return 1;
				}
	
				memcpy(argument[countArgument],
                    &buffer[indexPreargumentLastChar + 1],
                    i - indexPreargumentLastChar - 1
                );

				argument[countArgument][i - indexPreargumentLastChar] = 0;
				argument[countArgument] = NULL;
                countArgument++;
			}

			indexPreargumentLastChar = i;
		}
	}

	return countArgument;
}

int createChildProcess(char **argument, int isBackgroundProcess) {
	pid_t pid;
    int child;
	
	pid = fork();

	if (pid < 0) {
		printf("ERROR: Failed to create process\n");

		return 1;
	}
	
	if (pid == 0) {
		child = execvp(argument[0], argument);

		if (child == -1) {
			printf("ERROR: Failed to execute the command\n");
		}

		return 0;
	} else {
		if (isBackgroundProcess == 0) {
			wait(&child);
		}
	}
}

int main(void) {
    int shouldRun = 1;
    int length;
    char buffer[MAX_CHARACTER_SIZE + 1];
    int countHistory = 0;
    char history[5][MAX_CHARACTER_SIZE + 1];
    int countArgument;
	char *argument[MAX_CHARACTER_SIZE/2 + 1];

	memset(buffer, 0, sizeof(buffer));

	while (shouldRun) {
        printf("simple-shell> ");
		fflush(stdout);
		
		length = fetchInputString(buffer);

		if (length == -1) {
			continue;
		}

		if (!strcmp(buffer, "!!")) {
			if (!(countHistory > 0)) {
				printf("ERROR: No command exists in History");

				continue;
			}
			
			memcpy(buffer, history[countHistory % 5], MAX_CHARACTER_SIZE + 1);

			length = strlen(buffer);
		}
		
		countArgument = parseArgument(buffer, length, argument);

		if (!countArgument) {
			continue;
		}

		if (argument[0][0] == '!') {
			int historyTargetNumber = atoi(&argument[0][1]);

			if (!(historyTargetNumber > 0
            && historyTargetNumber >= countHistory - 4
            && historyTargetNumber <= countHistory)
            ) {
				printf("ERROR: No such command exists in history\n");
                
				continue;
			}
			
			countHistory++;

			memcpy(buffer, history[historyTargetNumber % 5], MAX_CHARACTER_SIZE + 1);

			length = strlen(buffer);
			countArgument = parseArgument(buffer, length, argument); 
		}

		if (!strcmp(argument[0], "exit")) {
			shouldRun = 0;

			continue;
		}

		if (!strcmp(argument[0], "history")) {
			printHistory(history, countHistory);

			countHistory++;

			memcpy(history[countHistory % 5], buffer, MAX_CHARACTER_SIZE + 1);
			
            continue;
		}

		countHistory++;

		memcpy(history[countHistory % 5], buffer, MAX_CHARACTER_SIZE + 1);

		int isBackgroundProcess = 0;

		if (!strcmp(argument[countArgument - 1], "&")) {
			isBackgroundProcess = 1;
			argument[countArgument - 1] = NULL;
			countArgument--;
		}

		createChildProcess(argument, isBackgroundProcess);
    }

	return 0;
}
