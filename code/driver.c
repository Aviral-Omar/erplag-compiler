#include <stdio.h>
#include <stdlib.h>

// Take the input file name and buffer size at command line
int main(int argc, char *argv[])
{
	// Checks if program receives 2 arguments from command line
	if (argc != 3) {
		printf("Please enter the filename and buffer size as arguments respectively.");
		exit(EXIT_FAILURE);
		/* TODO Delete
		Run "man 3 exit"
		*/
	}

	char *fileName = argv[1];
	FILE *fp = fopen(fileName, "r");
	// Checks if file path is valid
	if (!fp) {
		printf("File does not exist or unable to access file.");
		exit(EXIT_FAILURE);
	}

	char *endPtr;
	int bufferSize = (int)strtol(argv[2], &endPtr, 10);
	// Checks if buffer size is valid
	if (!(*argv[2] != '\0' && *endPtr == '\0')) {
		printf("Invalid buffer size.");
		exit(EXIT_FAILURE);
	}


	return 0;
}
