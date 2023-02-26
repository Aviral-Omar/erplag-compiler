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

ParseTableEntry PT[NON_TERMINAL_COUNT][TERMINAL_COUNT]; // will be filled by CreateParseTable()
FirstFollowEntry FF_Table[NON_TERMINAL_COUNT];  //will be populated by function calculating first and follow sets

FILE *openFile(char *fileName);
void readGrammar();
int findSymbol(char *symbol);
void computeFirstAndFollowSets();
void CreateParseTable();

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

void CreateParseTable(){   //Requires First and Follow sets of all non terminals
	
	for(int i=0; i<NON_TERMINAL_COUNT; i++){
		for(int j=0; j<TERMINAL_COUNT; j++){
			PT[i][j].rule = -1; // set everything to error
		}
	}

	FirstFollowEntry ff_entry;
	TerminalInfo* currFirst;
	TerminalInfo* currFollow;
	//we have to iterate through FF table for each non termi
	for(int non_termi=0; non_termi<NON_TERMINAL_COUNT; non_termi++){
		//Note that i represents enum of a Non Terminal
		ff_entry = FF_Table[non_termi];
		currFirst = ff_entry.first;
		currFollow = ff_entry.follow;

		int wasEpsilon = 0;

		TerminalInfo* curr_first_info = currFirst->tr; //1st element of First(X). X is FF_table[i]
		while(curr_first_info){
			if(curr_first_info->tr == EPSILON){
				//now take a look at follow of X too.
				wasEpsilon=1;
			}
			else{
				PT[non_termi][curr_first_info->tr].rule = curr_first_info->rule;
				curr_first_info = curr_first_info->next;
			}
		}
		if(wasEpsilon){
			TerminalInfo* curr_follow_info = currFollow->tr;
			while(curr_follow_info){
				PT[non_termi][curr_follow_info->tr].rule = curr_follow_info->rule;
				curr_follow_info = curr_follow_info->next;
			}
			//TODO handle case of $, have to add $ to TerminalInfo data struct somehow
		}
	}
}