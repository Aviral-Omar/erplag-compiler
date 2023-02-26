#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parserDef.h"

/* TODO change later*/
#define GRAMMAR_FILE "grammar.txt"
#define RULE_COUNT 142
#define NON_TERMINAL_COUNT 73
#define TERMINAL_COUNT 58
#define GRAMMAR_BUFFER_SIZE 4300

/* TODO clean up grammar later*/
LexicalSymbol *grammar[RULE_COUNT];
/* TODO populate this*/
char *nonTerminalMap[NON_TERMINAL_COUNT] = {"N_program",
											"N_moduleDeclarations",
											"N_otherModules",
											"N_driverModule",
											"N_moduleDeclaration",
											"N_module",
											"N_moduleDef",
											"N_inputPlist",
											"N_ret",
											"N_outputPlist",
											"N_dataType",
											"N_N1",
											"N_type",
											"N_N2",
											"N_rangeArrays",
											"N_indexArr",
											"N_statements",
											"N_statement",
											"N_ioStmt",
											"N_simpleStmt",
											"N_declareStmt",
											"N_conditionalStmt",
											"N_iterativeStmt",
											"N_varPrint",
											"N_P1",
											"N_boolConstt",
											"N_idNumRnum",
											"N_arrayElementForPrint",
											"N_newIndex",
											"N_moduleReuseStmt",
											"N_assignmentStmt",
											"N_whichStmt",
											"N_lvalueIDStmt",
											"N_lvalueARRStmt",
											"N_expression",
											"N_elementIndexWithExpressions",
											"N_sign",
											"N_optional",
											"N_actualParaList",
											"N_N11",
											"N_idList",
											"N_N3",
											"N_arithmeticOrBooleanExpr",
											"N_U",
											"N_unaryOp",
											"N_newNT",
											"N_arithmeticExpr",
											"N_varIDNum",
											"N_anyTerm",
											"N_N7",
											"N_logicalOp",
											"N_N8",
											"N_relationalOp",
											"N_term",
											"N_N4",
											"N_op1",
											"N_factor",
											"N_N5",
											"N_op2",
											"N_N10",
											"N_arrExpr",
											"N_arrN4",
											"N_arrTerm",
											"N_arrFactor",
											"N_arrN5",
											"N_caseStmts",
											"N_default",
											"N_value",
											"N_N9",
											"N_rangeForLoop",
											"N_indexForLoop",
											"N_signForLoop",
											"N_newIndexForLoop"};
char *terminalMap[TERMINAL_COUNT] = {"T_INTEGER",
									 "T_REAL",
									 "T_BOOLEAN",
									 "T_OF",
									 "T_ARRAY",
									 "T_START",
									 "T_END",
									 "T_DECLARE",
									 "T_MODULE",
									 "T_DRIVER",
									 "T_PROGRAM",
									 "T_GET_VALUE",
									 "T_PRINT",
									 "T_USE",
									 "T_WITH",
									 "T_PARAMETERS",
									 "T_TAKES",
									 "T_INPUT",
									 "T_RETURNS",
									 "T_FOR",
									 "T_IN",
									 "T_SWITCH",
									 "T_CASE",
									 "T_BREAK",
									 "T_DEFAULT",
									 "T_WHILE",
									 "T_ID",
									 "T_NUM",
									 "T_RNUM",
									 "T_AND",
									 "T_OR",
									 "T_TRUE",
									 "T_FALSE",
									 "T_PLUS",
									 "T_MINUS",
									 "T_MUL",
									 "T_DIV",
									 "T_LT",
									 "T_LE",
									 "T_GE",
									 "T_GT",
									 "T_EQ",
									 "T_NE",
									 "T_DEF",
									 "T_ENDDEF",
									 "T_DRIVERDEF",
									 "T_DRIVERENDDEF",
									 "T_COLON",
									 "T_RANGEOP",
									 "T_SEMICOL",
									 "T_COMMA",
									 "T_ASSIGNOP",
									 "T_SQBO",
									 "T_SQBC",
									 "T_BO",
									 "T_BC",
									 "epsilon"};


ParseTableEntry PT[NON_TERMINAL_COUNT][TERMINAL_COUNT]; // will be filled by CreateParseTable()
FirstFollowEntry FF_Table[NON_TERMINAL_COUNT];  //will be populated by function calculating first and follow sets

int findSymbol(char *symbol);
void readGrammar();
void printGrammar();
void computeFirstAndFollowSets();
void CreateParseTable();


int findSymbol(char *symbol)
{
	fflush(stdout);
	int i = 0;
	if (*symbol == 'N') {
		while (strcmp(symbol, nonTerminalMap[i]))
			i++;
		return i;
	}
	while (strcmp(symbol, terminalMap[i]))
		i++;
	return i;
}

void readGrammar()
{
	FILE *g = fopen(GRAMMAR_FILE, "r");
	if (!g) {
		fprintf(stderr, "grammar.txt does not exist.\n");
		exit(EXIT_FAILURE);
	}
	char grammarBuf[GRAMMAR_BUFFER_SIZE] = {'\0'};
	int fileLen = fread(grammarBuf, sizeof(char), GRAMMAR_BUFFER_SIZE, g);
	if (fclose(g) == EOF)
		fprintf(stderr, "Error closing file.\n");

	char *ptr = grammarBuf;
	// TODO change this
	char wordBuf[31];
	for (int i = 0; i < RULE_COUNT; i++) {
		LexicalSymbol *head = (LexicalSymbol *)malloc(sizeof(LexicalSymbol));
		char *bufPtr = wordBuf;
		while (*ptr != ' ') {
			*bufPtr = *ptr;
			ptr++;
			bufPtr++;
		}
		*bufPtr = '\0';
		head->type = 'N';
		head->data.nt = findSymbol(wordBuf);
		head->next = NULL;
		grammar[i] = head;
		LexicalSymbol *nextPtr = head;
		while (*ptr != '\n') {
			ptr++;
			nextPtr->next = (LexicalSymbol *)malloc(sizeof(LexicalSymbol));
			nextPtr = nextPtr->next;
			bufPtr = wordBuf;
			while (*ptr != ' ' && *ptr != '\n' && ptr < grammarBuf + fileLen) {
				*bufPtr = *ptr;
				ptr++;
				bufPtr++;
			}
			*bufPtr = '\0';
			// TODO confirm if different symbol is needed for epsilon
			nextPtr->type = (*wordBuf == 'N' ? 'N' : 'T');
			if (nextPtr->type == 'N')
				nextPtr->data.nt = findSymbol(wordBuf);
			else
				nextPtr->data.t = findSymbol(wordBuf);
			nextPtr->next = NULL;
		}
		ptr++;
	}
}

void printGrammar()
{
	for (int i = 0; i < RULE_COUNT; i++) {
		LexicalSymbol *ptr = grammar[i];
		while (ptr) {
			if (ptr->type == 'N')
				printf("%s ", nonTerminalMap[ptr->data.nt]);
			else
				printf("%s ", terminalMap[ptr->data.t]);
			ptr = ptr->next;
		}
		printf("\n");
	}
}

void computeFirstAndFollowSets()
{
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