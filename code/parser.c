#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parserDef.h"
#include "stack.h"
#include "treeDef.h"
#include "tree.h"

/* TODO clean up grammar later*/
LexicalSymbol *grammar[RULE_COUNT];
ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];	 // will be filled by CreateParseTable()
FirstFollowEntry ffTable[NON_TERMINAL_COUNT] = {{NULL, NULL}};	 // will be populated by function calculating first and follow sets
Stack *s;

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

void initParser();
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
void runOnlyParser();
void parseCurrToken();
void printParseTree(ParseTNode *node);

void printParseTree(ParseTNode *node)
{
	if (node == NULL)
		return;
	printParseTree(node->child);

	//printf("%s\n", node->data);
	if (node->type == 'N')
		printf("%s \n", nonTerminalMap[node->data.nt]);
	else
		printf("%s \n", terminalMap[node->data.t]);

	ParseTNode *sibling = node->child->sibling;
	while (sibling != NULL) {
		printParseTree(sibling);
		sibling = sibling->sibling;
	}
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
	// printf("     %d     %d    \n",s->top->data.nt,s->size);

	root->treenode = createParseTree(temp1, 'N');
	ParseTreeParent = root->treenode;

	populateSyn(); //TODO semicolon case
	// printParseTable(ParseTreeParent);
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
		printf("grammar.txt does not exist.\n");
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
	// printFirstSets();
	computeFollowSets();
	// printFollowSets();
}

void printParseTable()
{
	int row = NON_TERMINAL_COUNT;
	int col = TERMINAL_COUNT;
	FILE *outFile = fopen("parseTable", "w");
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
	// TODO check LL1 grammar
	for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
		for (int j = 0; j < TERMINAL_COUNT; j++) {
			parseTable[i][j] = -1;	// Initialize everything to error
		}
	}

	for (int rule = 0; rule < RULE_COUNT; rule++) {
		LexicalSymbol *LHS = grammar[rule];
		LexicalSymbol *RHS = LHS->next;
		int wasEpsilon = 1;
		int needFollow = 0;

		while (wasEpsilon) {
			if (RHS == NULL || RHS->type == 'e') {
				// NULL means we have reached end of RHS
				// go to follow of LHS
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

void pushRuleTokens(Stack *s, LexicalSymbol *RHS, ParseTNode *parent)
{
	if (RHS == NULL) {
		return;
	}

	// TODO iterative conversion
	pushRuleTokens(s, RHS->next, parent);
	SNode *tempS = pushTok(s, RHS->data, RHS->type);
	ParseTNode *tempT = addNode(parent, RHS->data, RHS->type);
	tempS->treenode = tempT;
}

void parseCurrToken(){
	Token inputSymbol = currToken->token;
	SNode *stackTop = top(s);  // get this from top of stack;

	if (stackTop->type == 'T') {
		// pop and input++;
		if (stackTop->data.t == inputSymbol) {
			printf("Accepted: %s\n",terminalMap[s->top->data.t]);
			pop(s);
		} else {
			// Error and recovery
			pop(s);
			printf("Syntax Error: Terminal present at inappropriate position\n\n");
		}
	} else {  // Assuming 'e' is not in stack
		// Case of non-terminal
		int ruleNumber = parseTable[stackTop->data.nt][inputSymbol];
		if (ruleNumber == -1) {
			// Error and Recovery
			printf("%s, %s, rule: %d\n",nonTerminalMap[stackTop->data.nt], terminalMap[inputSymbol], ruleNumber);
			printf("Syntax Error: input symbol can't be derived from top of stack Non Terminal\n\n");
			synRecovery();
		} else {
			LexicalSymbol *LHS = grammar[ruleNumber];
			LexicalSymbol *RHS = LHS->next;

			// Now pop stack top and put RHS in reverse order
			ParseTNode *parent = s->top->treenode;
			printf("Accepted: %s\n\n",nonTerminalMap[s->top->data.nt]);
			pop(s);
			pushRuleTokens(s, RHS, parent);
		}
	}
}

void runOnlyParser()
{
	// Assuming Stack and Tree are already initialized
	initLexer();
	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		getNextToken();
		handleWhitespaces();
		if (!currToken)
			continue;

		parseCurrToken();

		if(currToken){
			free(currToken);
			currToken = NULL;
		}
			
	}
	clearHeap();
	// printParseTree(ParseTreeParent);
}

// Populates Syn to the respective fields where the cell is blank after filling parse table
// -2 will indicate presence of Syn in a particular cell
// TODO insert SEMICOLON in appropriate syn set
void populateSyn()
{
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
	// We are here because Non termi at stack top can't derive input symbol

	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		// TODO free space used by token structs
		getNextToken();
		handleWhitespaces();  // TODO doubt about their order

		if (!currToken)
			continue;

		// printf("\t%-12s\n", tokenMap[currToken->token]);

		Token inputSymbol = currToken->token;
		SNode *stackTop = top(s);

		// Case of non-terminal
		int ruleNumber = parseTable[stackTop->data.nt][inputSymbol];
		if (ruleNumber == -1) {
			fflush(stdout);
			if(currToken){
				free(currToken);
				currToken = NULL;
			}
			continue;					// go to next token
		} else if (ruleNumber == -2) {	// it means input symbol is in sync set of top of stack.
			pop(s);
		} else {  // This means we found input symbol in first(top of stack)
			LexicalSymbol *LHS = grammar[ruleNumber];
			LexicalSymbol *RHS = LHS->next;

			// Now pop stack top and put RHS in reverse order
			ParseTNode *parent = s->top->treenode;
			pop(s);
			pushRuleTokens(s, RHS, parent);
		}

		fflush(stdout);

		/*TODO remove this*/
		if(currToken){
			free(currToken);
			currToken = NULL;
		}
		return;
	}
}
