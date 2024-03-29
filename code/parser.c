/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parserDef.h"
#include "stack.h"
#include "stackDef.h"
#include "tree.h"
#include "treeDef.h"

char *nonTerminalMap[NON_TERMINAL_COUNT] = {
	"program",
	"moduleDeclarations",
	"otherModules",
	"driverModule",
	"moduleDeclaration",
	"module",
	"moduleDef",
	"inputPList",
	"ret",
	"outputPList",
	"dataType",
	"iPList2",
	"type",
	"oPList2",
	"arrRange",
	"signedIndex",
	"statements",
	"statement",
	"ioStmt",
	"simpleStmt",
	"declareStmt",
	"conditionalStmt",
	"iterativeStmt",
	"varPrint",
	"arrIndex",
	"boolConst",
	"moduleReuseStmt",
	"assignmentStmt",
	"whichStmt",
	"lValueIDStmt",
	"lValueArrStmt",
	"expression",
	"indexWithExpressions",
	"sign",
	"index",
	"optional",
	"actualPList",
	"param",
	"actualPList2",
	"arrIndexWithExpressions",
	"idList",
	"idList2",
	"arithmeticOrLogicalExpr",
	"unaryOpExpr",
	"unaryOp",
	"unsignedArithExpr",
	"arithmeticExpr",
	"varIDNum",
	"anyTerm",
	"logicalOpExpr",
	"logicalOp",
	"relationOpExpr",
	"relationalOp",
	"term",
	"addSubExpr",
	"addSubOp",
	"factor",
	"mulDivExpr",
	"mulDivOp",
	"arrExpr",
	"arrTerm",
	"arrAddSubExpr",
	"arrFactor",
	"arrMulDivExpr",
	"caseStmts",
	"defaultCase",
	"caseValue",
	"caseStmts2",
	"forLoopRange",
	"forLoopIndex"};

char *nonTerminalStrings[NON_TERMINAL_COUNT] = {
	"N_program",
	"N_moduleDeclarations",
	"N_otherModules",
	"N_driverModule",
	"N_moduleDeclaration",
	"N_module",
	"N_moduleDef",
	"N_inputPList",
	"N_ret",
	"N_outputPList",
	"N_dataType",
	"N_iPList2",
	"N_type",
	"N_oPList2",
	"N_arrRange",
	"N_signedIndex",
	"N_statements",
	"N_statement",
	"N_ioStmt",
	"N_simpleStmt",
	"N_declareStmt",
	"N_conditionalStmt",
	"N_iterativeStmt",
	"N_varPrint",
	"N_arrIndex",
	"N_boolConst",
	"N_moduleReuseStmt",
	"N_assignmentStmt",
	"N_whichStmt",
	"N_lValueIDStmt",
	"N_lValueArrStmt",
	"N_expression",
	"N_indexWithExpressions",
	"N_sign",
	"N_index",
	"N_optional",
	"N_actualPList",
	"N_param",
	"N_actualPList2",
	"N_arrIndexWithExpressions",
	"N_idList",
	"N_idList2",
	"N_arithmeticOrLogicalExpr",
	"N_unaryOpExpr",
	"N_unaryOp",
	"N_unsignedArithExpr",
	"N_arithmeticExpr",
	"N_varIDNum",
	"N_anyTerm",
	"N_logicalOpExpr",
	"N_logicalOp",
	"N_relationOpExpr",
	"N_relationalOp",
	"N_term",
	"N_addSubExpr",
	"N_addSubOp",
	"N_factor",
	"N_mulDivExpr",
	"N_mulDivOp",
	"N_arrExpr",
	"N_arrTerm",
	"N_arrAddSubExpr",
	"N_arrFactor",
	"N_arrMulDivExpr",
	"N_caseStmts",
	"N_defaultCase",
	"N_caseValue",
	"N_caseStmts2",
	"N_forLoopRange",
	"N_forLoopIndex"};

char *terminalStrings[TERMINAL_COUNT] = {
	"T_DECLARE",
	"T_MODULE",
	"T_ID",
	"T_SEMICOL",
	"T_DRIVERDEF",
	"T_DRIVER",
	"T_PROGRAM",
	"T_DRIVERENDDEF",
	"T_DEF",
	"T_ENDDEF",
	"T_TAKES",
	"T_INPUT",
	"T_SQBO",
	"T_SQBC",
	"T_RETURNS",
	"T_COLON",
	"T_COMMA",
	"T_INTEGER",
	"T_REAL",
	"T_BOOLEAN",
	"T_ARRAY",
	"T_OF",
	"T_RANGEOP",
	"T_START",
	"T_END",
	"T_GET_VALUE",
	"T_BO",
	"T_BC",
	"T_PRINT",
	"T_NUM",
	"T_RNUM",
	"T_TRUE",
	"T_FALSE",
	"T_ASSIGNOP",
	"T_PLUS",
	"T_MINUS",
	"T_USE",
	"T_WITH",
	"T_PARAMETERS",
	"T_MUL",
	"T_DIV",
	"T_AND",
	"T_OR",
	"T_LT",
	"T_LE",
	"T_GT",
	"T_GE",
	"T_EQ",
	"T_NE",
	"T_SWITCH",
	"T_CASE",
	"T_BREAK",
	"T_DEFAULT",
	"T_FOR",
	"T_IN",
	"T_WHILE",
	"dollar",
	"epsilon"};

LexicalSymbol *grammar[RULE_COUNT];
FirstFollowEntry ffTable[NON_TERMINAL_COUNT] = {{NULL, NULL}};	 // will be populated by function calculating first and follow sets
ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];	 // will be filled by CreateParseTable()
int parserCorrect;

void initParser();
void clearParserData();
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
void printParseTable();
void populateSyn();
void errorRecovery();
void runParser(char *srcFilename, char *outFilename);
void pushRuleTokens(Stack *s, LexicalSymbol *RHS, ParseTNode *parent, int ruleNum);
void parseCurrToken();

void runParser(char *srcFilename, char *outFilename)
{
	parserCorrect = 1;
	ptNodes = 0;
	// Assuming Stack and Tree are already initialized
	src = fopen(srcFilename, "r");
	if (!src) {
		printf("File does not exist or unable to access file.\n");
		exit(EXIT_FAILURE);
	}
	lexerPrint = 0;	 // because lexer output not needed
	initLexer();
	initParser();

	s = createStack();
	Symbol temp0, temp1;
	temp0.t = DOLLAR;
	pushTok(s, temp0, 'T');	 // pushing dollar to the stack initially

	temp1.nt = 0;
	SNode *root = pushTok(s, temp1, 'N');  // pushing start symbol
	// printf("     %d     %d    \n",top(s)->data.nt,s->size);

	root->treenode = createParseTree(temp1, 'N');
	parseTreeRoot = root->treenode;

	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		getNextToken();
		handleWhitespaces();
		if (!currToken)
			continue;

		parseCurrToken();
	}
	while (!isEmpty(s)) {
		if (top(s)->type == 'N') {
			// printf("%s\n", nonTerminalStrings[top(s)->data.nt]);
			int ruleNumber = parseTable[top(s)->data.nt][DOLLAR];
			// printf("Rule %d applied.\n", ruleNumber);
			if (ruleNumber == -1) {
				parserCorrect = 0;
				printf("Syntax Error: Incomplete input, bottom of stack not reached.\n");
				break;
			} else {
				// printf("Accepted: %s\n\n", nonTerminalStrings[top(s)->data.nt]);
				ParseTNode *parent = top(s)->treenode;
				parent->info.ruleNum = ruleNumber;
				pop(s);
				pushRuleTokens(s, grammar[ruleNumber]->next, parent, ruleNumber);
			}
		} else if (top(s)->data.t != DOLLAR) {
			parserCorrect = 0;
			printf("Syntax Error: Incomplete input, bottom of stack not reached.\n");
			break;
		} else {
			pop(s);
		}
	}

	FILE *outFile = fopen(outFilename, "w");
	if (outFile == NULL) {
		printf("Error opening output file.\n");
		exit(EXIT_FAILURE);
	}

	// Initialize columns of parse tree printing table
	fprintf(outFile, "%-24s\t", "Lexeme");
	fprintf(outFile, "%-10s\t", "Line No.");
	fprintf(outFile, "%-13s\t", "Token Name");
	fprintf(outFile, "%-16s\t", "Value");
	fprintf(outFile, "%-30s\t", "Parent Node Symbol");
	fprintf(outFile, "%-6s\t", "IsLeaf");
	fprintf(outFile, "%-30s\t\n", "Node Symbol");
	fprintf(outFile, "-----------------------------------------------------");
	fprintf(outFile, "-----------------------------------------------------");
	fprintf(outFile, "-----------------------------------------------------\n");

	fflush(outFile);

	printParseTree(parseTreeRoot, outFile);	 // prints inorder parse tree
	fclose(outFile);


	if (lexerCorrect)
		printf("\nInput source code is lexically correct.\n");
	if (parserCorrect)
		printf("\nInput source code is syntactically correct.\n\n");
	fflush(stdout);

	clearLexerData();
	clearParserData();
}

void initParser()
{
	readGrammar();
	computeFirstAndFollowSets();
	createParseTable();
	populateSyn();
	s = NULL;
	parseTreeRoot = NULL;
}

void clearParserData()
{
	// Clearing grammar
	for (int i = 0; i < RULE_COUNT; i++) {
		LexicalSymbol *cur = grammar[i], *next;
		do {
			next = cur->next;
			free(cur);
			cur = next;
		} while (cur);
		grammar[i] = NULL;
	}

	// Clearing First Follow Table
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		TerminalInfo *cur = ffTable[i].first, *next;
		while (cur) {
			next = cur->next;
			free(cur);
			cur = next;
		}
		ffTable[i].first = NULL;
		cur = ffTable[i].follow;
		while (cur) {
			next = cur->next;
			free(cur);
			cur = next;
		}
		ffTable[i].follow = NULL;
	}

	if (s)
		deleteStack(s);
	s = NULL;
}

int findSymbol(char *symbol)
{
	// printf("%s\n", symbol);
	// fflush(stdout);
	int i = 0;
	if (*symbol == 'N') {
		while (strcmp(symbol, nonTerminalStrings[i]))
			i++;
		return i;
	}
	while (strcmp(symbol, terminalStrings[i]))
		i++;
	return i;
}

void readGrammar()
{
	FILE *g = fopen(GRAMMAR_FILE, "r");
	if (!g) {
		printf("%s does not exist.\n", GRAMMAR_FILE);
		exit(EXIT_FAILURE);
	}
	char grammarBuf[GRAMMAR_BUFFER_SIZE] = {'\0'};
	int fileLen = fread(grammarBuf, sizeof(char), GRAMMAR_BUFFER_SIZE, g);
	if (fclose(g) == EOF)
		printf("Error closing file.\n");

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
				printf("%s ", nonTerminalStrings[ptr->data.nt]);
			else
				printf("%s ", terminalStrings[ptr->data.t]);
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
	FILE *outFile = fopen("first.txt", "w");
	if (outFile == NULL) {
		printf("Error opening output file\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		fprintf(outFile, "%s: ", nonTerminalMap[i]);
		TerminalInfo *tiPtr = ffTable[i].first;
		while (tiPtr) {
			fprintf(outFile, "%s ", tokenMap[tiPtr->tr]);
			tiPtr = tiPtr->next;
		}
		fprintf(outFile, "\n");
	}
	fclose(outFile);
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
		// printf("Inserting %s into %s\n", terminalStrings[followPtr->tr], nonTerminalStrings[nt2]);
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
						if (!occurencesRemaining[grammar[j]->data.nt]) {
							insertFollowIntoFollow(grammar[j]->data.nt, symbolPtr->data.nt);
							symbolPtr->followCalculated = '1';
							occurencesRemaining[symbolPtr->data.nt]--;
						} else if (symbolPtr->data.nt == grammar[j]->data.nt && occurencesRemaining[grammar[j]->data.nt] == 1) {
							// Recursive case
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
	FILE *outFile = fopen("follow.txt", "w");
	if (outFile == NULL) {
		printf("Error opening output file\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		fprintf(outFile, "%s: ", nonTerminalMap[i]);
		TerminalInfo *tiPtr = ffTable[i].follow;
		while (tiPtr) {
			fprintf(outFile, "%s ", tokenMap[tiPtr->tr]);
			tiPtr = tiPtr->next;
		}
		fprintf(outFile, "\n");
	}
	fclose(outFile);
}

void computeFirstAndFollowSets()
{
	computeFirstSets();
	computeFollowSets();
}

void printParseTable()
{
	int row = NON_TERMINAL_COUNT;
	int col = TERMINAL_COUNT;
	FILE *outFile = fopen("parseTable.txt", "w");
	if (outFile == NULL) {
		printf("Error opening output file\n");
		exit(EXIT_FAILURE);
	}

	fprintf(outFile, "%-29s", "");
	for (int i = 0; i < col; i++) {
		fprintf(outFile, "%-14s\t", tokenMap[i]);
	}
	fprintf(outFile, "\n");
	for (int i = 0; i < row; i++) {
		fprintf(outFile, "%-29s\t", nonTerminalMap[i]);
		for (int j = 0; j < col; j++) {
			fprintf(outFile, "%-14d\t", parseTable[i][j]);
		}
		fprintf(outFile, "\n");
	}
	fclose(outFile);
}

void createParseTable()
{  // Requires First and Follow sets of all non terminals
	for (int i = 0; i < NON_TERMINAL_COUNT; i++)
		for (int j = 0; j < TERMINAL_COUNT; j++)
			parseTable[i][j] = -1;

	for (int rule = 0; rule < RULE_COUNT; rule++) {
		LexicalSymbol *LHS = grammar[rule];
		LexicalSymbol *RHS = LHS->next;
		int wasEpsilon = 1;
		int needFollow = 0;

		while (wasEpsilon) {
			if (RHS == NULL || RHS->type == 'e') {
				// NULL means we have reached end of RHS
				// go to follow of LHS
				// Tracking epsilon producing rule here
				if (RHS->type == 'e')
					parseTable[LHS->data.nt][EPSILON] = rule;
				needFollow = 1;
				break;
			}
			if (RHS->type == 'N') {
				wasEpsilon = 0;
				TerminalInfo *currFirst = ffTable[RHS->data.nt].first;
				while (currFirst) {
					if (currFirst->tr == EPSILON) {
						wasEpsilon = 1;
					} else {
						if (parseTable[LHS->data.nt][currFirst->tr] != -1) {
							printf("GRAMMAR IS NOT LL(1). %s %s, Rule no. %d\n", nonTerminalMap[LHS->data.nt], tokenMap[currFirst->tr], rule);
						}
						parseTable[LHS->data.nt][currFirst->tr] = rule;
					}
					currFirst = currFirst->next;
				}
				if (wasEpsilon) {
					RHS = RHS->next;
				}
			} else if (RHS->type == 'T') {
				if (parseTable[LHS->data.nt][RHS->data.t] != -1) {
					printf("GRAMMAR IS NOT LL(1),  Rule no. %d\n", rule);
				}
				parseTable[LHS->data.nt][RHS->data.t] = rule;
				wasEpsilon = 0;	 // to Break the while loop
			} else {
				printf("Invalid Type Entered\n");
			}
		}

		if (needFollow) {
			// We need follow of LHS
			TerminalInfo *currFollow = ffTable[LHS->data.nt].follow;
			while (currFollow) {
				parseTable[LHS->data.nt][currFollow->tr] = rule;  // This means we can derive e from LHS directly or indirectly
				currFollow = currFollow->next;
			}
		}
	}
}

void pushRuleTokens(Stack *s, LexicalSymbol *RHS, ParseTNode *parent, int ruleNum)
{
	if (!RHS)
		return;

	ParseTNode *tempT = addNode(parent, RHS->data, RHS->type, ruleNum);

	pushRuleTokens(s, RHS->next, parent, ruleNum);
	if (RHS->type != 'e') {
		SNode *tempS = pushTok(s, RHS->data, RHS->type);
		tempS->treenode = tempT;
	}
}


void parseCurrToken()
{
	Token data;
	char type;
	SNode *stackTop;  // get this from top of stack;
	do {
		stackTop = top(s);
		data = stackTop->data.t;
		type = stackTop->type;
		if (type == 'T') {
			if (data == currToken->token) {
				top(s)->treenode->info.tokIn = currToken;
				// printf("Matched token %s\n", terminalStrings[currToken->token]);
				pop(s);
				break;
			} else if (data == SEMICOL) {
				// If semicolon is on top of stack, skip input till semicolon is reached
				if (currToken->token != END) {
					parserCorrect = 0;
					// printf("Skipping Token %s\n", terminalStrings[currToken->token]);
					break;
				} else {
					removeNode(top(s)->treenode);
					pop(s);
					parserCorrect = 0;
					printf("Line %d: Syntax Error: Missing semicolon\n", currToken->lineNumber);
				}
			} else {
				// Pop terminal if it does not match
				removeNode(top(s)->treenode);
				pop(s);
				parserCorrect = 0;
				printf("Line %d: Syntax Error: Terminal %s present at inappropriate position cannot be matched with %s\n", currToken->lineNumber, tokenMap[currToken->token], tokenMap[data]);
			}
		} else {  // Assuming 'e' is not in stack
			// Case of non-terminal
			// printf("Processing %s and %s\n", nonTerminalStrings[stackTop->data.nt], terminalStrings[currToken->token]);
			int ruleNumber = parseTable[stackTop->data.nt][currToken->token];
			if (ruleNumber == -1) {
				parserCorrect = 0;
				printf("Line %d: Syntax Error: Input symbol %s cannot be derived from top of stack Non Terminal %s\n", currToken->lineNumber, tokenMap[currToken->token], nonTerminalMap[stackTop->data.nt]);

				// If epsilon production is found, apply it
				ruleNumber = parseTable[stackTop->data.nt][EPSILON];
				if (ruleNumber != -1) {
					pop(s);
					pushRuleTokens(s, grammar[ruleNumber]->next, stackTop->treenode, ruleNumber);
					// printf("Applied rule %d\n", ruleNumber);
				} else if (currToken->token == SEMICOL) {
					// If semicolon is found, get next token and pop non terminals till a rule matches in errorRecovery
					errorRecovery();
					ruleNumber = parseTable[top(s)->data.nt][currToken->token];
					LexicalSymbol *LHS = grammar[ruleNumber];
					LexicalSymbol *RHS = LHS->next;
					ParseTNode *parent = top(s)->treenode;

					pop(s);
					pushRuleTokens(s, RHS, parent, ruleNumber);
					// printf("Applied rule %d\n", ruleNumber);
				}
				// Skip tokens until an element in FOLLOW of stack top is seen
				else {
					parserCorrect = 0;
					// printf("Skipping Token %s\n", tokenMap[currToken->token]);
					break;
				}

				// if (errorRecovery() == 1)
				// 	break;
			} else if (ruleNumber == -2) {
				// Uses follow set to sync by popping non terminal from stack
				parserCorrect = 0;
				printf("Line %d: Syntax Error: Input symbol %s can't be derived from top of stack Non Terminal %s\n", currToken->lineNumber, tokenMap[currToken->token], nonTerminalMap[stackTop->data.nt]);
				pop(s);
			} else {
				LexicalSymbol *LHS = grammar[ruleNumber];
				LexicalSymbol *RHS = LHS->next;

				// Now pop stack top and put RHS in reverse order
				ParseTNode *parent = top(s)->treenode;
				parent->info.ruleNum = ruleNumber;
				pop(s);
				pushRuleTokens(s, RHS, parent, ruleNumber);
				// printf("Applied rule %d\n", ruleNumber);
			}
		}
	} while (1);
}

// Populates Syn to the respective fields where the cell is blank after filling parse table
// -2 will indicate presence of Syn in a particular cell
void populateSyn()
{
	// Use rule 2 from book
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		TerminalInfo *currFollow = ffTable[i].follow;
		while (currFollow) {
			if (parseTable[i][currFollow->tr] == -1) {
				parseTable[i][currFollow->tr] = -2;
			}
			currFollow = currFollow->next;
		}
	}
}

void errorRecovery()
{
	// Reads next token after semicolon
	if (currToken) {
		free(currToken);
		currToken = NULL;
	}
	handleWhitespaces();

	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		getNextToken();
		handleWhitespaces();
		if (!currToken)
			continue;
		while (parseTable[top(s)->data.nt][currToken->token] < 0)
			pop(s);
		return;
	}
}
