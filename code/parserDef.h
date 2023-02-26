#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"

typedef enum {
	// TODO populate using grammar
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
	N_boolConstt,
	N_idNumRnum,
	N_arrayElementForPrint,
	N_newIndex,
	N_moduleReuseStmt,
	N_assignmentStmt,
	N_whichStmt,
	N_lvalueIDStmt,
	N_lvalueARRStmt,
	N_expression,
	N_elementIndexWithExpressions,
	N_sign,
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
	char type;
};

typedef struct TerminalInfo TerminalInfo;
struct TerminalInfo {
	Terminal tr;
	int rule;
	TerminalInfo* next;
};

typedef struct {
	NonTerminal nt;
	TerminalInfo* first;
	TerminalInfo* follow;
} FirstFollowEntry;

extern LexicalSymbol* grammar[];
extern char* nonTerminalMap[];
extern char* terminalMap[];

#endif
