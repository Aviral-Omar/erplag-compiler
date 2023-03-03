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
	"N_K",
	"N_newParaList",
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

LexicalSymbol *grammar[RULE_COUNT];
FirstFollowEntry ffTable[NON_TERMINAL_COUNT] = {{NULL, NULL}};	 // will be populated by function calculating first and follow sets
ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];	 // will be filled by CreateParseTable()
Stack *s;
ParseTNode *parseTreeParent;
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
void synRecovery();
void runParser(char *srcFilename, char *outFilename);
void pushRuleTokens(Stack *s, LexicalSymbol *RHS, ParseTNode *parent, int ruleNum);
void parseCurrToken();
void printParseTree(ParseTNode *node, FILE *outFile);

void runParser(char *srcFilename, char *outFilename)
{
	parserCorrect = 1;
	// Assuming Stack and Tree are already initialized
	src = fopen(srcFilename, "r");
	if (!src) {
		printf("File does not exist or unable to access file.\n");
		exit(EXIT_FAILURE);
	}
	lexerPrint = 0;	 // because lexer output not needed
	initLexer();
	initParser();
	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		getNextToken();
		handleWhitespaces();
		if (!currToken)
			continue;

		parseCurrToken();

		// if (currToken) {
		// 	free(currToken);
		// 	currToken = NULL;
		// }
	}
	while (!isEmpty(s)) {
		if (top(s)->type == 'N') {
			// printf("%s\n", nonTerminalMap[top(s)->data.nt]);
			int ruleNumber = parseTable[top(s)->data.nt][DOLLAR];
			// printf("Rule %d applied.\n", ruleNumber);
			if (ruleNumber == -1) {
				parserCorrect = 0;
				printf("Syntax Error: Incomplete input, bottom of stack not reached.\n");
				break;
			} else {
				// printf("Accepted: %s\n\n", nonTerminalMap[top(s)->data.nt]);
				ParseTNode *parent = top(s)->treenode;
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

	// TODO pretty printing
	// Initialize columns of parse tree printing table
	fprintf(outFile, "%-24s\t", "lexeme");
	fprintf(outFile, "%-10s\t", "lineNo.");
	fprintf(outFile, "%-13s\t", "TokenName");
	fprintf(outFile, "%-16s\t", "value");
	fprintf(outFile, "%-30s\t", "ParentNodeSymbol");
	fprintf(outFile, "%-6s\t", "isLeaf");
	fprintf(outFile, "%-30s\t", "NodeSymbol");
	fprintf(outFile, "\n");

	fflush(outFile);

	printParseTree(parseTreeParent, outFile);  // prints inorder parse tree
	fclose(outFile);


	if (lexerCorrect)
		printf("\nInput source code is lexically correct.\n");
	if (parserCorrect)
		printf("\nInput source code is syntactically correct.\n");
	fflush(stdout);
	clearLexerData();
	clearParserData();
}

void initParser()
{
	readGrammar();
	computeFirstAndFollowSets();
	createParseTable();
	s = createStack();
	Symbol temp0, temp1;
	temp0.t = DOLLAR;
	pushTok(s, temp0, 'T');	 // pushing dollar to the stack initially

	temp1.nt = N_program;
	SNode *root = pushTok(s, temp1, 'N');  // pushing start symbol
	// printf("     %d     %d    \n",top(s)->data.nt,s->size);

	root->treenode = createParseTree(temp1, 'N');
	parseTreeParent = root->treenode;

	populateSyn();
	// printParseTable();
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

	deleteStack(s);
	deleteParseTree(parseTreeParent);
}

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
	// printFirstSets();
	computeFollowSets();
	// printFollowSets();
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
		fprintf(outFile, "%-14s\t", terminalMap[i]);
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
							printf("GRAMMAR IS NOT LL(1). %s %s, Rule no. %d\n", nonTerminalMap[LHS->data.nt], terminalMap[currFirst->tr], rule);
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
	// TODO add error printfs and parsercorrect appropriately
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
				// printf("Matched token %s\n", terminalMap[currToken->token]);
				pop(s);
				break;
			} else if (data == SEMICOL) {
				// TODO proper error printing
				// If semicolon is on top of stack, skip input till semicolon is reached
				if (currToken->token != END) {
					parserCorrect = 0;
					// printf("Skipping Token %s\n", terminalMap[currToken->token]);
					break;
				} else {
					pop(s);
					parserCorrect = 0;
					printf("Line %d: Syntax Error: Missing semicolon\n");
				}
			} else {
				// Pop terminal if it does not match
				pop(s);
				parserCorrect = 0;
				printf("Line %d: Syntax Error: Terminal %s present at inappropriate position cannot be matched with %s\n", currToken->lineNumber, terminalMap[currToken->token], terminalMap[data]);
			}
		} else {  // Assuming 'e' is not in stack
			// Case of non-terminal
			// printf("Processing %s and %s\n", nonTerminalMap[stackTop->data.nt], terminalMap[currToken->token]);
			int ruleNumber = parseTable[stackTop->data.nt][currToken->token];
			if (ruleNumber == -1) {
				parserCorrect = 0;
				printf("Line %d: Syntax Error: Input symbol %s cannot be derived from top of stack Non Terminal %s\n", currToken->lineNumber, terminalMap[currToken->token], nonTerminalMap[stackTop->data.nt]);

				// If epsilon production is found, apply it
				ruleNumber = parseTable[stackTop->data.nt][EPSILON];
				if (ruleNumber != -1) {
					pop(s);
					pushRuleTokens(s, grammar[ruleNumber]->next, stackTop->treenode, ruleNumber);
					// printf("Applied rule %d\n", ruleNumber);
				} else if (currToken->token == SEMICOL) {
					// If semicolon is found, get next token and pop non terminals till a rule matches in synRecovery
					synRecovery();
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
					// printf("Skipping Token %s\n", terminalMap[currToken->token]);
					break;
				}

				// if (synRecovery() == 1)
				// 	break;
			} else if (ruleNumber == -2) {
				// Uses follow set to sync by popping non terminal from stack
				parserCorrect = 0;
				printf("Line %d: Syntax Error: Input symbol %s can't be derived from top of stack Non Terminal %s\n", currToken->lineNumber, terminalMap[currToken->token], nonTerminalMap[stackTop->data.nt]);
				pop(s);
			} else {
				LexicalSymbol *LHS = grammar[ruleNumber];
				LexicalSymbol *RHS = LHS->next;

				// Now pop stack top and put RHS in reverse order
				ParseTNode *parent = top(s)->treenode;
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

void synRecovery()
{
	// TODO make void and change name to error recovery
	// Reads next token after seicolon
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

void printParseTree(ParseTNode *node, FILE *outFile)
{
	if (node == NULL)
		return;
	printParseTree(node->child, outFile);
	if (node->type == 'N') {
		fprintf(outFile, "%-24s\t", "---");
		fprintf(outFile, "%-10s\t", "---");	 // TODO rule number
		fprintf(outFile, "%-13s\t", "---");
		fprintf(outFile, "%-16s\t", "---");
		if (!node->parent)
			fprintf(outFile, "%-30s\t", "ROOT");  // Parent node symbol
		else
			fprintf(outFile, "%-30s\t", nonTerminalMap[node->parent->data.nt]);	 // Parent node symbol
		fprintf(outFile, "%-6s\t", "no");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->data.nt]);
	} else if (node->type == 'T') {
		Token t = node->data.t;
		if (t == NUM || t == RNUM || t == ID)
			fprintf(outFile, "%-24s\t", node->info.tokIn->data.lexeme);	 // lexeme
		else
			fprintf(outFile, "%-24s\t", lexemeMap[t]);				// lexeme
		fprintf(outFile, "%-10d\t", node->info.tokIn->lineNumber);	// TODO rule number
		fprintf(outFile, "%-13s\t", terminalMap[node->data.t]);
		if (t == NUM)
			fprintf(outFile, "%-16d\t", atoi(node->info.tokIn->data.lexeme));
		else if (t == RNUM)
			fprintf(outFile, "%-16f\t", atof(node->info.tokIn->data.lexeme));
		else
			fprintf(outFile, "%-16s\t", "---");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->parent->data.nt]);	 // Parent node symbol
		fprintf(outFile, "%-6s\t", "yes");
		fprintf(outFile, "%-30s\t", "---");
	} else {
		fprintf(outFile, "%-24s\t", "---");
		fprintf(outFile, "%-10s\t", "---");	 // TODO rule number
		fprintf(outFile, "%-13s\t", terminalMap[node->data.t]);
		fprintf(outFile, "%-16s\t", "---");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->parent->data.nt]);	 // Parent node symbol
		fprintf(outFile, "%-6s\t", "yes");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->data.nt]);
	}
	fprintf(outFile, "\n");
	fflush(outFile);

	if (node->child) {
		ParseTNode *sibling = node->child->sibling;
		while (sibling != NULL) {
			printParseTree(sibling, outFile);
			sibling = sibling->sibling;
		}
	}
	return;
}
