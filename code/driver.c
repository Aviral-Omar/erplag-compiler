#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "lookupTable.h"

#define MAX_BUFFER_SIZE 100000

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
	buf1 = (char *)malloc(bufferSize * sizeof(char));
	buf2 = (char *)malloc(bufferSize * sizeof(char));
	lexemeBegin = (currBuffer == FIRST ? buf1 : buf2);
	forward = lexemeBegin;
	removeComments(argv[1], "cleaned");

	/* TODO Move to parser function*/
	populateLookupTable();
	// First read is unconditional
	getStream();
	// To remove initial whitespaces
	handleWhitespaces();
	// TODO Reading and throwing away tokens for testing
	while (charsRead == bufferSize || lexemeBegin < (currBuffer == FIRST ? buf1 : buf2) + charsRead) {
		// TODO Store tokens in LinkedList in future
		// TODO free space used by token structs
		TokenInfo *tk = getNextToken();

		printf("%d ", tk->token);
		fflush(stdout);

		/*TODO remove this*/
		free(tk);
	}

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
	/* TODO buffer size check*/
	if (size > MAX_BUFFER_SIZE) {
		printf("Buffer size too large.\n");
		exit(EXIT_FAILURE);
	}
	return size;
}
