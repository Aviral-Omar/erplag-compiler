/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef SYMBOL_DEF
#define SYMBOL_DEF

typedef enum {
	INTEGER,
	REAL,
	BOOLEAN,
	OF,
	ARRAY,
	START,
	END,
	DECLARE,
	MODULE,
	DRIVER,
	PROGRAM,
	GET_VALUE,
	PRINT,
	USE,
	WITH,
	PARAMETERS,
	TAKES,
	INPUT,
	RETURNS,
	FOR,
	IN,
	SWITCH,
	CASE,
	BREAK,
	DEFAULT,
	WHILE,
	ID,
	NUM,
	RNUM,
	AND,
	OR,
	TRUE,
	FALSE,
	PLUS,
	MINUS,
	MUL,
	DIV,
	LT,
	LE,
	GE,
	GT,
	EQ,
	NE,
	DEF,
	ENDDEF,
	DRIVERDEF,
	DRIVERENDDEF,
	COLON,
	RANGEOP,
	SEMICOL,
	COMMA,
	ASSIGNOP,
	SQBO,
	SQBC,
	BO,
	BC,
	DOLLAR,
	EPSILON,
	COMMENTMARK
} Token;

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
	N_K,
	N_newParaList,
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

typedef union symbol {
	NonTerminal nt;
	Terminal t;
} Symbol;

#endif
