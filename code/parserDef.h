#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"

/* TODO change later*/
#define GRAMMAR_FILE "grammar.txt"
#define RULE_COUNT 139
#define NON_TERMINAL_COUNT 71
#define TERMINAL_COUNT 58
#define GRAMMAR_BUFFER_SIZE 4500

// TODO rename Non Terminals in grammar
typedef enum {
	N_program,
	N_moduleDeclarations,
	N_otherModules,
	N_driverModule,
	N_moduleDeclaration,
	N_module,
	N_moduleDef,
	N_inputPlist,
	N_ret,
	N_outputPlist,
	N_dataType,
	N_N1,
	N_type,
	N_N2,
	N_rangeArrays,
	N_indexArr,
	N_statements,
	N_statement,
	N_ioStmt,
	N_simpleStmt,
	N_declareStmt,
	N_conditionalStmt,
	N_iterativeStmt,
	N_varPrint,
	N_P1,
	N_boolConst,
	N_moduleReuseStmt,
	N_assignmentStmt,
	N_whichStmt,
	N_lvalueIDStmt,
	N_lvalueARRStmt,
	N_expression,
	N_elementIndexWithExpressions,
	N_sign,
	N_newIndex,
	N_optional,
	N_actualParaList,
	N_N11,
	N_idList,
	N_N3,
	N_arithmeticOrBooleanExpr,
	N_U,
	N_unaryOp,
	N_newNT,
	N_arithmeticExpr,
	N_varIDNum,
	N_anyTerm,
	N_N7,
	N_logicalOp,
	N_N8,
	N_relationalOp,
	N_term,
	N_N4,
	N_op1,
	N_factor,
	N_N5,
	N_op2,
	N_N10,
	N_arrExpr,
	N_arrN4,
	N_arrTerm,
	N_arrFactor,
	N_arrN5,
	N_caseStmts,
	N_default,
	N_value,
	N_N9,
	N_rangeForLoop,
	N_indexForLoop,
	N_signForLoop,
	N_newIndexForLoop
} NonTerminal;

// aliasing Token enum
typedef Token Terminal;

typedef struct LexicalSymbol LexicalSymbol;
struct LexicalSymbol {
	union symbolData {
		NonTerminal nt;
		Terminal t;
	} data;
	LexicalSymbol* next;
	char type;				// may be 'T', 'N', 'e'
	char followCalculated;	// used during follow computation
};

typedef struct TerminalInfo TerminalInfo;
struct TerminalInfo {
	Terminal tr;
	TerminalInfo* next;
};

typedef struct {
	// TODO initialize both First,Follow below to NULL
	TerminalInfo* first;
	TerminalInfo* follow;
} FirstFollowEntry;

typedef struct {
	int rule;  // -1 if its error
			   // created a struct in case we want to add some other info later.
} ParseTableEntry;

// TODO make sure all are cleaned
extern LexicalSymbol* grammar[RULE_COUNT];
extern char* nonTerminalMap[NON_TERMINAL_COUNT];
extern char* terminalMap[TERMINAL_COUNT];
extern ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];
extern FirstFollowEntry ffTable[NON_TERMINAL_COUNT];


typedef struct SNodeType
{
    union nodeData {
		LexicalSymbol *S;
		TokenInfo *T;
	}data;

    struct SNodeType* next;
    struct ParseTNode* Treenode;

}SNode;

typedef struct StackType
{
    SNode* top;
}Stack;

Stack create(void);
int isEmpty(Stack* s);
int isFull(Stack* s);
int pushLex(Stack* s, LexicalSymbol *nextSymbol);
int pushTok(Stack* s, TokenInfo *T);
int pop(Stack* s);
TokenInfo *top(Stack* s);


typedef struct ParseTNodeType
{
	union nodeData {
		LexicalSymbol *S;
		TokenInfo *T;
	}data;

	ParseTNodeType* child, parent, sibling;

}ParseTNode;

typedef struct ParseTreeType
{
	ParseTNode* node;
}ParseTree;


ParseTree *CreateParseTree(union nodeData *d);
int addNode(ParseTNode *node,union nodeData *d);
ParseTNode *child(ParseTNode *node);
ParseTNode *sibling(ParseTNode *node);
ParseTNode *parent(ParseTNode *node);

#endif
