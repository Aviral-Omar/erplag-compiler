#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parserDef.h"

/* TODO clean up grammar later*/
LexicalSymbol *grammar[RULE_COUNT];
ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];	 // will be filled by CreateParseTable()
FirstFollowEntry ffTable[NON_TERMINAL_COUNT] = {{NULL, NULL}};	 // will be populated by function calculating first and follow sets

char *nonTerminalMap[NON_TERMINAL_COUNT] = {
	"N_program",
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
	"N_boolConst",
	"N_moduleReuseStmt",
	"N_assignmentStmt",
	"N_whichStmt",
	"N_lvalueIDStmt",
	"N_lvalueARRStmt",
	"N_expression",
	"N_elementIndexWithExpressions",
	"N_sign",
	"N_newIndex",
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

char *terminalMap[TERMINAL_COUNT] = {
	"T_INTEGER",
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
	"dollar",
	"epsilon"};

int findSymbol(char *symbol);
void readGrammar();
void printGrammar();
void insertIntoFirst(NonTerminal nt, Terminal t);
void computeFirstSets();
void printFirstSets();
void insertIntoFollow(NonTerminal nt, Terminal t);
int insertFirstIntoFollow(NonTerminal firstNT, NonTerminal followNT);
void insertFollowIntoFollow(NonTerminal nt1, NonTerminal nt2);
void computeFollowSets();
void printFollowSets();
void computeFirstAndFollowSets();
void createParseTable();


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
		head->followCalculated = '0';
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
			nextPtr->type = *wordBuf;
			if (nextPtr->type == 'N')
				nextPtr->data.nt = findSymbol(wordBuf);
			else
				nextPtr->data.t = findSymbol(wordBuf);
			nextPtr->next = NULL;
			nextPtr->followCalculated = '0';
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

void insertIntoFirst(NonTerminal nt, Terminal t)
{
	TerminalInfo *ti;

	TerminalInfo *firstPtr = ffTable[nt].first;
	if (firstPtr == NULL) {
		ti = (TerminalInfo *)malloc(sizeof(TerminalInfo));
		ti->tr = t;
		ti->next = NULL;
		ffTable[nt].first = ti;
	} else {
		while (firstPtr->next != NULL && firstPtr->tr != t)
			firstPtr = firstPtr->next;
		if (firstPtr->tr != t) {
			ti = (TerminalInfo *)malloc(sizeof(TerminalInfo));
			ti->tr = t;
			ti->next = NULL;
			firstPtr->next = ti;
		}
	}
}

void computeFirstSets()
{
	int rulesRemaining[NON_TERMINAL_COUNT] = {0};
	for (int i = 0; i < RULE_COUNT; i++)
		rulesRemaining[grammar[i]->data.nt]++;

	int ruleFlag[RULE_COUNT] = {0};

	LexicalSymbol *symbolPtr;
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		int changeFlag = 0;
		for (int j = 0; j < RULE_COUNT; j++) {
			if (ruleFlag[j])
				continue;
			changeFlag = 1;

			NonTerminal nt = grammar[j]->data.nt;

			symbolPtr = grammar[j]->next;
			if (symbolPtr->type == 'e') {
				insertIntoFirst(nt, EPSILON);
				ruleFlag[j] = 1;
				rulesRemaining[nt]--;
				continue;
			}

			// Checks whether all NTs in rule have FIRST set calculated
			// Ignores if the remaining NT is recursive
			while (symbolPtr && symbolPtr->type == 'N') {
				if (rulesRemaining[symbolPtr->data.nt] && symbolPtr->data.nt != nt)
					break;
				symbolPtr = symbolPtr->next;
			}
			// This means loop ended because some FIRST for some NT is left
			if (symbolPtr && symbolPtr->type == 'N')
				continue;

			symbolPtr = grammar[j]->next;
			int insertEpsilon = 0, toContinue = 0;
			do {
				toContinue = 0;
				if (symbolPtr->type == 'N') {
					TerminalInfo *tPtr = ffTable[symbolPtr->data.nt].first;
					insertEpsilon = 0;
					while (tPtr) {
						if (tPtr->tr != EPSILON)
							insertIntoFirst(nt, tPtr->tr);
						else {
							insertEpsilon = 1;
							toContinue = 1;
						}
						tPtr = tPtr->next;
					}
				} else {
					insertEpsilon = 0;
					insertIntoFirst(nt, symbolPtr->data.t);
				}
				symbolPtr = symbolPtr->next;
			} while (toContinue && symbolPtr);

			if (insertEpsilon)
				insertIntoFirst(nt, EPSILON);
			ruleFlag[j] = 1;
			rulesRemaining[nt]--;
		}

		// breaks loop when there are no more changes from last iteration
		if (!changeFlag)
			break;
	}
}

void printFirstSets()
{
	printf("\nFIRST Sets:\n");
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		printf("%s: ", nonTerminalMap[i]);
		TerminalInfo *tiPtr = ffTable[i].first;
		while (tiPtr) {
			printf("%s ", terminalMap[tiPtr->tr]);
			tiPtr = tiPtr->next;
		}
		printf("\n");
	}
	printf("\n");
}

void insertIntoFollow(NonTerminal nt, Terminal t)
{
	TerminalInfo *ti;

	TerminalInfo *followPtr = ffTable[nt].follow;
	if (followPtr == NULL) {
		ti = (TerminalInfo *)malloc(sizeof(TerminalInfo));
		ti->tr = t;
		ti->next = NULL;
		ffTable[nt].follow = ti;
	} else {
		while (followPtr->next != NULL && followPtr->tr != t)
			followPtr = followPtr->next;
		if (followPtr->tr != t) {
			ti = (TerminalInfo *)malloc(sizeof(TerminalInfo));
			ti->tr = t;
			ti->next = NULL;
			followPtr->next = ti;
		}
	}
}

// Returns 1 if first contains epsilon else returns 0
int insertFirstIntoFollow(NonTerminal firstNT, NonTerminal followNT)
{
	TerminalInfo *firstPtr = ffTable[firstNT].first;
	int hasEpsilon = 0;
	while (firstPtr != NULL) {
		if (firstPtr->tr == EPSILON)
			hasEpsilon = 1;
		else
			insertIntoFollow(followNT, firstPtr->tr);
		firstPtr = firstPtr->next;
	}
	return hasEpsilon;
}

void insertFollowIntoFollow(NonTerminal nt1, NonTerminal nt2)
{
	TerminalInfo *followPtr = ffTable[nt1].follow;
	while (followPtr != NULL) {
		// printf("Inserting %s into %s\n", terminalMap[followPtr->tr], nonTerminalMap[nt2]);
		insertIntoFollow(nt2, followPtr->tr);
		followPtr = followPtr->next;
	}
}

void computeFollowSets()
{
	// inserting DOLLAR in start symbol follow set
	TerminalInfo *dollarInfo = (TerminalInfo *)malloc(sizeof(TerminalInfo));
	dollarInfo->tr = DOLLAR;
	dollarInfo->next = NULL;
	ffTable[0].follow = dollarInfo;
	grammar[0]->followCalculated = '1';

	int occurencesRemaining[NON_TERMINAL_COUNT] = {0};
	for (int i = 0; i < RULE_COUNT; i++) {
		LexicalSymbol *symbolPtr = grammar[i]->next;
		while (symbolPtr) {
			if (symbolPtr->type == 'N')
				occurencesRemaining[symbolPtr->data.nt]++;
			symbolPtr = symbolPtr->next;
		}
	}

	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		int changeFlag = 0;
		for (int j = 0; j < RULE_COUNT; j++) {
			LexicalSymbol *symbolPtr = grammar[j]->next;
			while (symbolPtr != NULL) {
				if (symbolPtr->type == 'T' || symbolPtr->type == 'e' || symbolPtr->followCalculated == '1') {
					symbolPtr = symbolPtr->next;
					continue;
				}
				changeFlag = 1;
				LexicalSymbol *forwardPtr = symbolPtr;
				do {
					forwardPtr = forwardPtr->next;
					if (!forwardPtr) {
						if (!occurencesRemaining[grammar[j]->data.nt] || (symbolPtr->data.nt == grammar[j]->data.nt && occurencesRemaining[grammar[j]->data.nt] == 1)) {
							insertFollowIntoFollow(grammar[j]->data.nt, symbolPtr->data.nt);
							symbolPtr->followCalculated = '1';
							occurencesRemaining[symbolPtr->data.nt]--;
						}
					} else if (forwardPtr->type == 'T') {
						insertIntoFollow(symbolPtr->data.nt, forwardPtr->data.t);
						symbolPtr->followCalculated = '1';
						occurencesRemaining[symbolPtr->data.nt]--;
						break;
					} else if (!insertFirstIntoFollow(forwardPtr->data.nt, symbolPtr->data.nt)) {
						symbolPtr->followCalculated = '1';
						occurencesRemaining[symbolPtr->data.nt]--;
						break;
					}
				} while (forwardPtr);
				symbolPtr = symbolPtr->next;
			}
		}
		if (!changeFlag)
			break;
	}
}

void printFollowSets()
{
	printf("\nFOLLOW Sets:\n");
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		printf("%s: ", nonTerminalMap[i]);
		TerminalInfo *tiPtr = ffTable[i].follow;
		while (tiPtr) {
			printf("%s ", terminalMap[tiPtr->tr]);
			tiPtr = tiPtr->next;
		}
		printf("\n");
	}
	printf("\n");
}

void computeFirstAndFollowSets()
{
	computeFirstSets();
	printFirstSets();
	computeFollowSets();
	printFollowSets();
}

void createParseTable()
{  // Requires First and Follow sets of all non terminals

	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		for (int j = 0; j < TERMINAL_COUNT; j++) {
			parseTable[i][j].rule = -1;	 // set everything to error
		}
	}

	FirstFollowEntry ff_entry;
	TerminalInfo *currFirst;
	TerminalInfo *currFollow;
	// we have to iterate through FF table for each non termi
	for (int non_termi = 0; non_termi < NON_TERMINAL_COUNT; non_termi++) {
		// Note that i represents enum of a Non Terminal
		ff_entry = ffTable[non_termi];
		currFirst = ff_entry.first;
		currFollow = ff_entry.follow;

		int wasEpsilon = 0;

		TerminalInfo *curr_first_info = currFirst;	// 1st element of First(X). X is FF_table[i]
		while (curr_first_info) {
			if (curr_first_info->tr == EPSILON) {
				// now take a look at follow of X too.
				wasEpsilon = 1;
			} else {
				// parseTable[non_termi][curr_first_info->tr].rule = curr_first_info->rule;
				curr_first_info = curr_first_info->next;
			}
		}
		if (wasEpsilon) {
			TerminalInfo *curr_follow_info = currFollow;
			while (curr_follow_info) {
				// parseTable[non_termi][curr_follow_info->tr].rule = curr_follow_info->rule;
				curr_follow_info = curr_follow_info->next;
			}
			// TODO handle case of $, have to add $ to TerminalInfo data struct somehow
		}
	}
}
