#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
// TODO remove lexer.h
#include "lookupTable.h"
#include "parser.h"

#define MAX_BUFFER_SIZE 10000

FILE *openFile(char *fileName);
int checkBufferSize(char *buf);

// Take the input file name and buffer size at command line
int main(int argc, char *argv[])
{
	// Checks if program receives 2 arguments from command line
	if (argc != 3) {
		printf("Please enter the filename and buffer size as arguments respectively.\n");
		exit(EXIT_FAILURE);
	}

	// All of these are global variables declared in lexerDef.h file and defined in lexer.c
	fp = openFile(argv[1]);
	bufferSize = checkBufferSize(argv[2]);

	removeComments(argv[1], "cleaned");
	initLexer();

	initParser();
	// TODO Reading and throwing away tokens for testing
	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		// TODO free space used by token structs
		getNextToken();
		handleWhitespaces();
		if (!currToken)
			continue;

		printf("\t%-12s\n", tokenMap[currToken->token]);

		fflush(stdout);

		/*TODO remove this*/
		free(currToken);
	}
	if (syntaxCorrect)
		printf("\nInput source code is syntactically correct.\n");

	clearHeap();

	return 0;
}

// Checks if file path is valid and opens file
FILE *openFile(char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (!file) {
		printf("File does not exist or unable to access file.\n");
		exit(EXIT_FAILURE);
	}
	return file;
}

// Checks if buffer size is valid
int checkBufferSize(char *buf)
{
	char *endPtr;
	int size = (int)strtol(buf, &endPtr, 10);
	if (*buf == '\0' || *endPtr != '\0') {
		printf("Invalid buffer size.\n");
		exit(EXIT_FAILURE);
	}
	// checks max buffer size
	if (size > MAX_BUFFER_SIZE) {
		printf("Buffer size too large.\n");
		exit(EXIT_FAILURE);
	}
	return size;
}
