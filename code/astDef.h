/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef AST_DEF
#define AST_DEF

#include "treeDef.h"

#define NODE_TYPE_COUNT 51

typedef enum {
	AST_Program,
	AST_ModuleDeclarations,
	AST_ModuleDefinitions,
	AST_Driver,
	AST_Module,
	AST_ParametersList,
	AST_InputVarType,
	AST_OutputVarType,
	AST_Array,
	AST_ArrayRange,
	AST_SignedIndex,
	AST_Statements,
	AST_GetValue,
	AST_Print,
	AST_ArrayAccess,
	AST_Assign,
	AST_ArrayAssign,
	AST_IndexWithExpressions,
	AST_Declare,
	AST_Switch,
	AST_Case,
	AST_DefaultCase,
	AST_While,
	AST_For,
	AST_ForRange,
	AST_SignedForIndex,
	AST_FunctionCall,
	AST_VariableList,
	AST_ActualParametersList,
	AST_SignedParam,
	AST_UnaryOpExpr,
	AST_Integer,
	AST_Real,
	AST_Boolean,
	AST_Num,
	AST_RNum,
	AST_True,
	AST_False,
	AST_ID,
	AST_Plus,
	AST_Minus,
	AST_Mul,
	AST_Div,
	AST_LT,
	AST_LE,
	AST_GT,
	AST_GE,
	AST_EQ,
	AST_NE,
	AST_AND,
	AST_OR
} ASTNodeType;

// typedef enum {
// 	DT_Integer,
// 	DT_Real,
// 	DT_Boolean,
// 	DT_Array
// } DataType;

// typedef struct {
// 	DataType type;
// 	// To be used for array data
// 	DataType arrayType;
// 	int lowerBound;
// 	int upperBound;
// } TypeInfo;

typedef struct ASTNodeStruct ASTNode;
struct ASTNodeStruct {
	ASTNodeType nodeType;
	int childCount;
	ASTNode **children;
	// ASTNode *parent;
	ASTNode *listNext;
	ParseTNode *nodeSyn;
	// TODO symbol table
	// TypeInfo *dataType;
	TokenInfo *value;
};

extern ASTNode *astRoot;
extern char *astNodeMap[NODE_TYPE_COUNT];
#endif
