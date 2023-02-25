#include <stdio.h>
#include <stdlib.h>

#include "parserDef.h"

/* TODO change later*/
#define RULE_COUNT 100
#define NON_TERMINAL_COUNT 50
#define TERMINAL_COUNT 58
#define GRAMMAR_BUFFER_SIZE 1000

/* TODO clean up grammar later*/
LexicalSymbol *grammar[RULE_COUNT];
/* TODO populate this*/
char *nonTerminalMap[NON_TERMINAL_COUNT] = {"N_Program"};
char *nonTerminalMap[TERMINAL_COUNT] = {"T_ID"};

FILE *openFile(char *fileName);
void readGrammar();
int findSymbol(char *symbol);
void computeFirstAndFollowSets();

FILE *openFile(char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (!file) {
		fprintf(stderr, "grammar.txt does not exist.\n");
		exit(EXIT_FAILURE);
	}
	return file;
}

int findSymbol(char *symbol)
{
	char map;
}

void readGrammar()
{
	FILE *g = openFile("grammar.txt");
	char grammarBuf[GRAMMAR_BUFFER_SIZE];
	fread(grammarBuf, sizeof(char), GRAMMAR_BUFFER_SIZE, g);
	if (fclose(fp) == EOF)
		fprintf(stderr, "Error closing file.\n");

	char *ptr = grammarBuf;
	// TODO change this
	char wordBuf[30];
	for (int i = 0; i < RULE_COUNT; i++) {
		LexicalSymbol *head = (LexicalSymbol *)malloc(sizeof(LexicalSymbol));
		char *bufPtr = wordBuf;
		while (*ptr != ' ') {
			*bufPtr = *ptr;
			ptr++;
			bufPtr++;
		}
		bufPtr = '\0';
		head->type = 'N';
	}
}
