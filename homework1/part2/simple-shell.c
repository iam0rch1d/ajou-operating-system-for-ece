#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX_CHARSIZE 80 

int fetch_input(char *stringBuffer) {
	char charBuffer;
	
	int countChar = 0;
	while (((charBuffer = getchar()) != '\n') && (countChar < MAX_CHARSIZE + 1)) {
	 	stringBuffer[countChar] = charBuffer;
		++countChar;
	}

	if (charBuffer != '\n' && countChar == MAX_CHARSIZE) {
	 	printf("ERROR-00: Command length exceeds maximum\n");
		countChar = -1;
	}
	else {
	 	stringBuffer[countChar] = 0;
	}

	return countChar;
}

void print_history(char history[5][MAX_CHARSIZE + 1], int countHistory) {
 	if (!countHistory) {
	 	printf("ERROR-01: No history exists\n");
		return;
	}

	int i, j;
	for (i = countHistory, j = 5; i > 0 && j > 0; i--, j--) {
		printf("%5d %s\n", i, history[i % 5]);
	}
}

int parse_argument(char *buffer, int length, char **argument) {
 	int countArgument = 0, indexPreargumentLastChar = -1, i;
	argument[0] = NULL;
	
	for (i = 0; i <= length; ++i) {
		if (buffer[i] && !isspace(buffer[i])) {
			continue;
		}
		else {
			if (indexPreargumentLastChar != i - 1) {
				argument[countArgument] = (char *) malloc(sizeof(char) * (i - indexPreargumentLastChar));
				if (argument[countArgument] == NULL) {
					printf("ERROR-02: Failed to allocate memory\n");
					return 1;
				}
	
				memcpy(argument[countArgument], &buffer[indexPreargumentLastChar + 1], i - indexPreargumentLastChar - 1);
				argument[countArgument][i - indexPreargumentLastChar] = 0;
				
				++countArgument;
				argument[countArgument] = NULL;
			}
				
			indexPreargumentLastChar = i;
		}
	}

	return countArgument;
}

int create_childprocess(char **argument, int background) {
	pid_t pid;
	
	pid = fork();
	if (pid < 0) {
		printf("ERROR-03: Failed to create process\n");
		return 1;
	}
	
	int child;
	if (pid == 0) {
		child = execvp(argument[0], argument);

		if (child == -1) {
			printf("ERROR-04: Failed to execute the command\n");
		}

		return 0;
	}
	else {
		if (!background) {
			wait(&child);
		}
	}
}

int main(void) {
	char *argument[MAX_CHARSIZE/2 + 1];

	char history[5][MAX_CHARSIZE + 1];
	int countHistory = 0;

	char buffer[MAX_CHARSIZE + 1];
	memset(buffer, 0, sizeof(buffer));
	int length, countArgument;
	
	int should_run = 1;
	while (should_run) {
        printf("simple-shell> ");
		fflush(stdout);
		
		length = fetch_input(buffer);
		if (length == -1) {
			continue;
		}

		if (!strcmp(buffer, "!!")) {
			if (!(countHistory > 0)) {
				printf("ERROR-05: No command exists in History");
				continue;
			}
			
			memcpy(buffer, history[countHistory % 5], MAX_CHARSIZE + 1);
			length = strlen(buffer);
		}
		
		countArgument = parse_argument(buffer, length, argument);
		if (!countArgument) {
			continue;
		}

		if (argument[0][0] == '!') {
			int historyTargetNumber = atoi(&argument[0][1]);
			if (!(historyTargetNumber > 0 && historyTargetNumber >= countHistory - 4 && historyTargetNumber <= countHistory)) {
				printf("ERROR-06: No such command exists in history\n");
				continue;
			}
			
			++countHistory;
			memcpy(buffer, history[historyTargetNumber % 5], MAX_CHARSIZE + 1);
			length = strlen(buffer);
			countArgument = parse_argument(buffer, length, argument); 
		}

		if (!strcmp(argument[0], "exit")) {
			should_run = 0;
			continue;
		}

		if (!strcmp(argument[0], "history")) {
			print_history(history, countHistory);

			++countHistory;
			memcpy(history[countHistory % 5], buffer, MAX_CHARSIZE + 1);
			continue;
		}

		++countHistory;
		memcpy(history[countHistory % 5], buffer, MAX_CHARSIZE + 1);

		int background = 0;
		if (!strcmp(argument[countArgument - 1], "&")) {
			background = 1;
			argument[countArgument - 1] = NULL;
			countArgument--;
		}

		create_childprocess(argument, background);
    }
    
	return 0;
}
