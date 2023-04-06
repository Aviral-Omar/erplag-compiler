/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "symbolTable.h"
#include "tree.h"

#define MAX_BUFFER_SIZE 10000

// Take the input file name and buffer size at command line
int main(int argc, char *argv[])
{
	printf("============Implementation Status============\n");
	printf("1) FIRST and FOLLOW set computation automated.\n");
	printf("2) Both lexical and syntax analysis modules implemented and working.\n");
	printf("3) Parse Tree constructed.\n");
	printf("4) Modules work for all given test cases.\n");
	printf("5) First & Follow sets, Parse Table, Parse Tree can be printed in separate files by calling already implemented functions.\n");
	printf("6) All Lexical and Syntactic errors are being detected for given test cases.\n");

	// Checks if program receives 2 arguments from command line
	if (argc != 4) {
		printf("Please enter the source filename, parse tree output filename and buffer size as arguments respectively.\n");
		exit(EXIT_FAILURE);
	}


	// Checking if buffer size input is valid
	char *endPtr;
	bufferSize = (int)strtol(argv[3], &endPtr, 10);
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
		printf("\n============ Select Option ============\n");
		printf("0: For exit\n");
		printf("1: For removal of comments\n");
		printf("2: For printing the token list\n");
		printf("3: For parsing\n");
		printf("4: For printing the total time taken\n");
		printf("5: For printing First & Follow sets, Parse Table in respective txt files\n");
		printf("6: For generating and printing Abstract Syntax Tree\n");
		printf("7: For printing the number of nodes in AST, memory allocated and compression ratio\n");
		printf("8: For generating and printing symbol table\n\n");
		printf("Enter option choice: ");
		scanf(" %d", &option);
		printf("\n");

		if (option == 1) {
			removeComments(argv[1], "cleaned");
		} else if (option == 2) {
			runOnlyLexer(argv[1]);
		} else if (option == 3) {
			runParser(argv[1], argv[2]);
			if (parseTreeRoot)
				deleteParseTree(parseTreeRoot);
			parseTreeRoot = NULL;
		} else if (option == 4) {
			clock_t start_time, end_time;

			double total_CPU_time, total_CPU_time_in_seconds;

			start_time = clock();

			runParser(argv[1], argv[2]);
			if (parseTreeRoot)
				deleteParseTree(parseTreeRoot);
			parseTreeRoot = NULL;

			end_time = clock();

			total_CPU_time = (double)(end_time - start_time);

			total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

			printf("Total CPU Time: %f\n", total_CPU_time);
			printf("Total CPU Time in seconds: %f\n\n", total_CPU_time_in_seconds);
		} else if (option == 5) {
			initParser();
			printFirstSets();
			printFollowSets();
			printParseTable();
			clearParserData();
		} else if (option == 6) {
			runParser(argv[1], argv[2]);
			if (lexerCorrect && parserCorrect) {
				createAST();
				printAST(astRoot);
			}
		} else if (option == 7) {
			runParser(argv[1], argv[2]);
			if (lexerCorrect && parserCorrect) {
				createAST();
			}
			printf("Nodes in Parse Tree = %d\n", ptNodes);
			int ptMem = ptNodes * sizeof(ParseTNode);
			printf("Memory allocated to Parse Tree = %d\n", ptMem);

			printf("Nodes in Abstract Syntax Tree = %d\n", astNodes);
			int astMem = astNodes * sizeof(ASTNode);
			printf("Memory allocated to Abstract Syntax Tree = %d\n", astMem);
			printf("Compression Ratio = %lf\n", (double)(ptMem - astMem) * 100 / ptMem);
		} else if (option == 8) {
			runParser(argv[1], argv[2]);
			if (lexerCorrect && parserCorrect) {
				createAST();
				createSymbolTables();
			}
		}
	} while (option);


	return 0;
}
