#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "lexerDef.h"

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

	// All of these are global variables declared in lexerDef.h file
	const FILE *fp = openFile(argv[1]);
	const int bufferSize = checkBufferSize(argv[2]);

	char buf1[bufferSize], buf2[bufferSize];
	char *lexemeBegin = 0;
	char *forward = 0;
	int lineNumber = 1;
	CurrentBuffer currBuffer = FIRST;

	/* TODO Move to parser function*/
	// First read is unconditional
	getStream();

	// Reading and throwing away tokens for testing
	while (*lexemeBegin != EOF) {
		TokenInfo tk = getNextToken();
	}

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
	if (size > 100000) {
		printf("Buffer size too large.\n");
		exit(EXIT_FAILURE);
	}
	return size;
}
