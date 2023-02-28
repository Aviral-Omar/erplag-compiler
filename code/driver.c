#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

#define MAX_BUFFER_SIZE 10000

// Take the input file name and buffer size at command line
int main(int argc, char *argv[])
{
	printf("FIRST and FOLLOW set computation automated.\n");
	printf("Both lexical and syntax analysis modules implemented and working.\n");
	printf("Parse Tree constructed.\n");
	printf("Modules work for all given test cases.\n\n");

	// Checks if program receives 2 arguments from command line
	if (argc != 3) {
		printf("Please enter the filename and buffer size as arguments respectively.\n");
		exit(EXIT_FAILURE);
	}


	// Checking if buffer size input is valid
	char *endPtr;
	bufferSize = (int)strtol(argv[2], &endPtr, 10);
	if (*argv[2] == '\0' || *endPtr != '\0') {
		printf("Invalid buffer size.\n");
		exit(EXIT_FAILURE);
	}
	if (bufferSize > MAX_BUFFER_SIZE) {
		printf("Buffer size too large.\n");
		exit(EXIT_FAILURE);
	}

	int option;
	do {
		printf("0: For exit\n");
		printf("1: For removal of comments\n");
		printf("2: For printing the token list\n");
		printf("3: For parsing\n");
		printf("4: For printing the total time taken\n\n");
		printf("Enter option choice: ");
		scanf(" %d", &option);
		printf("\n");

		if (option == 1) {
			removeComments(argv[1], "cleaned");
		} else if (option == 2) {
			// Checks if file path is valid and opens file
			src = fopen(argv[1], "r");
			if (!src) {
				printf("File does not exist or unable to access file.\n");
				exit(EXIT_FAILURE);
			}

			initLexer();
			lexerPrint = 1;
			printf("Line Number\tLexeme\t\t\tToken Name\n");
			while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
				getNextToken();
				handleWhitespaces();
				if (!currToken)
					continue;

				free(currToken);
			}
			if (syntaxCorrect)
				printf("\nInput source code is syntactically correct.\n");
			lexerPrint = 0;
			clearHeap();
		} else if (option == 3) {
			initParser();
		}

	} while (option);


	return 0;
}
