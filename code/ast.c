/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>

#include "astDef.h"
#include "parserDef.h"
#include "tree.h"
#define createChildrenArray(size) size != 0 ? (ASTNode**)malloc(size * sizeof(ASTNode)) : NULL

ASTNode* astRoot;

char* astNodeMap[NODE_TYPE_COUNT] = {
	"AST_Program",
	"AST_ModuleDeclarations",
	"AST_ModuleDefinitions",
	"AST_Driver",
	"AST_Module",
	"AST_ParametersList",
	"AST_InputVarType",
	"AST_OutputVarType",
	"AST_Array",
	"AST_ArrayRange",
	"AST_SignedIndex",
	"AST_Statements",
	"AST_GetValue",
	"AST_Print",
	"AST_ArrayAccess",
	"AST_Assign",
	"AST_ArrayAssign",
	"AST_IndexWithExpressions",
	"AST_Declare",
	"AST_Switch",
	"AST_Case",
	"AST_DefaultCase",
	"AST_While",
	"AST_For",
	"AST_ForRange",
	"AST_SignedForIndex",
	"AST_FunctionCall",
	"AST_VariableList",
	"AST_ActualParametersList",
	"AST_SignedParam",
	"AST_UnaryOpExpr",
	"AST_Integer",
	"AST_Real",
	"AST_Boolean",
	"AST_Num",
	"AST_RNum",
	"AST_True",
	"AST_False",
	"AST_ID",
	"AST_Plus",
	"AST_Minus",
	"AST_Mul",
	"AST_Div",
	"AST_LT",
	"AST_LE",
	"AST_GT",
	"AST_GE",
	"AST_EQ",
	"AST_NE",
	"AST_AND",
	"AST_OR"};

ASTNode* createASTNode(ASTNodeType type, int childCount);
void createAST();
void insertChild(ASTNode* parent, ASTNode* child);
void handleIDNumRNum(ParseTNode* idNode, ASTNode** newNode);
void removeAndAdvance(ParseTNode** treeNodePtr);
ASTNode* buildAST(ParseTNode* currNode);

ASTNode* createASTNode(ASTNodeType type, int childCount)
{
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->nodeType = type;
	// node->parent = parent;
	node->childCount = 0;
	node->children = createChildrenArray(childCount);
	// node->dataType = NULL;
	node->listNext = NULL;
	node->nodeSyn = NULL;
	node->value = NULL;

	return node;
}

void createAST()
{
	astRoot = buildAST(parseTreeRoot);
}

void insertChild(ASTNode* parent, ASTNode* child)
{
	// if (child) {
	// 	printf("Inserting %s into %s\n", astNodeMap[child->nodeType], astNodeMap[parent->nodeType]);
	// 	fflush(stdout);
	// }
	parent->children[parent->childCount] = child;
	parent->childCount++;
}

void handleIDNumRNum(ParseTNode* treeNode, ASTNode** newNode)
{
	Token token = treeNode->info.tokIn->token;
	*newNode = createASTNode(token == ID ? AST_ID : (token == NUM ? AST_Num : AST_RNum), 0);
	(*newNode)->value = treeNode->info.tokIn;
	treeNode->info.tokIn = NULL;
}

void removeAndAdvance(ParseTNode** treeNodePtr)
{
	ParseTNode* next = (*treeNodePtr)->nextSibling;
	removeNode(*treeNodePtr);
	*treeNodePtr = next;
}

// TypeInfo* createTypeInfo(ASTNode* node)
// {
// 	TypeInfo* typeInfo = (TypeInfo*)malloc(sizeof(TypeInfo));
// 	switch (node->nodeType) {
// 	case AST_Array:
// 		ASTNode *arr, arrRange, index1, index2;
// 		typeInfo->type = DT_Array;
// 		arr = node->children[0];
// 		arrRange =
// 		switch (arr->nodeType) {
// 		case AST_Integer:
// 			typeInfo->arrayType = DT_Integer;
// 			break;
// 		case AST_Real:
// 			typeInfo->arrayType = DT_Integer;
// 			break;
// 		case AST_Boolean:
// 			typeInfo->arrayType = DT_Boolean;
// 		}


// 		break;
// 	case AST_Integer:
// 		typeInfo->type = DT_Integer;
// 		break;
// 	case AST_Real:
// 		typeInfo->type = DT_Integer;
// 		break;
// 	case AST_Boolean:
// 		typeInfo->type = DT_Boolean;
// 	}
// }

ASTNode* buildAST(ParseTNode* currNode)
{
	if (!currNode)
		return NULL;

	ASTNode* currASTNode = NULL;
	// removeNode automatically changes currChild to next sibling
	// Checking if node is ID, Num or RNum
	if (currNode->type == 'T') {
		Token tk = currNode->info.tokIn->token;
		if (tk == ID || tk == NUM || tk == RNUM) {
			handleIDNumRNum(currNode, &currASTNode);
			return currASTNode;
		}
	}

	ParseTNode* currChild = currNode->child;
	// To align with semantic rules

	int rule = currNode->info.ruleNum + 1;

	// printf("%-30s\t", nonTerminalMap[currNode->data.nt]);
	// fflush(stdout);
	// printf("Rule %d\n", rule);
	// fflush(stdout);

	switch (rule) {
	case 1:
		currASTNode = createASTNode(AST_Program, 4);

		insertChild(currASTNode, createASTNode(AST_ModuleDeclarations, 1));
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_ModuleDefinitions, 1));
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_ModuleDefinitions, 1));
		insertChild(currASTNode->children[3], buildAST(currChild));
		removeAndAdvance(&currChild);
		break;
	case 2:
	case 5:
	case 26:
		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 3:
	case 6:
	case 13:
	case 16:
	case 27:
	case 55:
	case 59:
	case 65:
	case 68:
	case 125:
	case 129:
		removeAndAdvance(&currChild);
		break;
	case 4:
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		handleIDNumRNum(currChild, &currASTNode);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 7:
		currASTNode = createASTNode(AST_Driver, 1);

		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		break;
	case 8:
		currASTNode = createASTNode(AST_Module, 4);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_ParametersList, 1));
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		break;
	case 9:
	case 34:
		currASTNode = createASTNode(rule == 9 ? AST_ParametersList : AST_Print, 1);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 10:
		currASTNode = createASTNode(AST_ParametersList, 0);
		removeAndAdvance(&currChild);
		break;
	case 11:
	case 14:
		currASTNode = createASTNode(rule == 11 ? AST_InputVarType : AST_OutputVarType, 1);
		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);

		break;
	case 12:
	case 15:
		currASTNode = createASTNode(rule == 12 ? AST_InputVarType : AST_OutputVarType, 1);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 17:
	case 22:
		currASTNode = createASTNode(AST_Integer, 0);
		removeAndAdvance(&currChild);
		break;
	case 18:
	case 23:
		currASTNode = createASTNode(AST_Real, 0);
		removeAndAdvance(&currChild);
		break;
	case 19:
	case 24:
		currASTNode = createASTNode(AST_Boolean, 0);
		removeAndAdvance(&currChild);
		break;
	case 20:
		currASTNode = createASTNode(AST_Array, 2);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		break;
	case 21:
	case 132:
		currASTNode = createASTNode(rule == 21 ? AST_ArrayRange : AST_ForRange, 2);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		break;
	case 25:
		currASTNode = createASTNode(AST_Statements, 1);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 38:
	case 43:
	case 44:
	case 62:
	case 69:
	case 70:
	case 73:
	case 94:
	case 107:
	case 127:
		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 33:
		currASTNode = createASTNode(AST_GetValue, 1);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 35:
	case 45:
		currChild = currChild->nextSibling;
		currChild->inh = currChild->prevSibling;

		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);

		break;
	case 36:
	case 37:
	case 51:
	case 52:
	case 60:
	case 61:
	case 76:
	case 77:
	case 78:
	case 92:
	case 93:
	case 105:
	case 106:
	case 126:
		handleIDNumRNum(currChild, &currASTNode);
		removeAndAdvance(&currChild);
		break;
	case 39:
	case 40:
		currASTNode = createASTNode(rule == 39 ? AST_True : AST_False, 0);
		removeAndAdvance(&currChild);
		break;
	case 41:
	case 48:
		currASTNode = createASTNode(rule == 41 ? AST_ArrayAccess : AST_Assign, 2);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currNode->inh, &currASTNode->children[currASTNode->childCount++]);
		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeNode(currNode->inh);

		break;
	case 42:
		handleIDNumRNum(currNode->inh, &currASTNode);
		removeAndAdvance(&currChild);
		removeNode(currNode->inh);

		break;
	case 46:
	case 47:
		currChild->inh = currNode->inh;
		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);

		break;
	case 49:
		currASTNode = createASTNode(AST_ArrayAssign, 2);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_ArrayAccess, 2));

		handleIDNumRNum(currNode->inh, &currASTNode->children[0]->children[currASTNode->children[0]->childCount++]);
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 50:
	case 71:
		currASTNode = createASTNode(rule == 50 ? AST_SignedIndex : AST_UnaryOpExpr, 2);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		break;
	case 53:
	case 74:
		currASTNode = createASTNode(AST_Plus, 0);
		removeAndAdvance(&currChild);
		break;
	case 54:
	case 75:
		currASTNode = createASTNode(AST_Minus, 0);
		removeAndAdvance(&currChild);
		break;
	case 56:
		currASTNode = createASTNode(AST_FunctionCall, 3);

		insertChild(currASTNode, createASTNode(AST_VariableList, 1));
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_ActualParametersList, 1));
		insertChild(currASTNode->children[2], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 57:
		currASTNode = createASTNode(AST_SignedParam, 1);
		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 58:
		currASTNode = createASTNode(AST_SignedParam, 1);
		removeAndAdvance(&currChild);
		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 63:
		currASTNode = createASTNode(AST_ArrayAccess, 2);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		break;
	case 64:
		removeAndAdvance(&currChild);
		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 66:
		handleIDNumRNum(currChild, &currASTNode);
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);

		break;
	case 67:
		removeAndAdvance(&currChild);
		handleIDNumRNum(currChild, &currASTNode);
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 72:
	case 91:
	case 108:
		removeAndAdvance(&currChild);

		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 79:
	case 82:
	case 85:
	case 88:
	case 99:
	case 102:
		currChild = currChild->nextSibling;
		currChild->inh = currChild->prevSibling;

		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 80:
	case 86:
	case 89:
	case 100:
	case 103:
		currChild->nextSibling->nextSibling->inh = currChild->nextSibling;

		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currNode->inh));
		currChild = currChild->nextSibling;

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeNode(currNode->inh);

		break;
	case 81:
	case 84:
	case 87:
	case 90:
	case 97:
	case 101:
	case 104:
		removeAndAdvance(&currChild);
		if (currNode->inh) {
			currASTNode = buildAST(currNode->inh);
			removeNode(currNode->inh);
		}
		break;
	case 83:
		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currNode->inh));

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeNode(currNode->inh);
		break;
	case 95:
		currChild = currChild->nextSibling;
		currChild->inh = currChild->prevSibling;

		currASTNode = buildAST(currChild);
		removeAndAdvance(&currChild);

		break;
	case 96:
		currASTNode = createASTNode(AST_ArrayAccess, 2);
		handleIDNumRNum(currNode->inh, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 98:
		currASTNode = createASTNode(AST_IndexWithExpressions, 2);
		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		break;
	case 109:
		currASTNode = createASTNode(AST_Plus, 2);
		break;
	case 110:
		currASTNode = createASTNode(AST_Minus, 2);
		break;
	case 111:
		currASTNode = createASTNode(AST_Mul, 2);
		break;
	case 112:
		currASTNode = createASTNode(AST_Div, 2);
		break;
	case 113:
		currASTNode = createASTNode(AST_AND, 2);
		break;
	case 114:
		currASTNode = createASTNode(AST_OR, 2);
		break;
	case 115:
		currASTNode = createASTNode(AST_LT, 2);
		break;
	case 116:
		currASTNode = createASTNode(AST_LE, 2);
		break;
	case 117:
		currASTNode = createASTNode(AST_GT, 2);
		break;
	case 118:
		currASTNode = createASTNode(AST_GE, 2);
		break;
	case 119:
		currASTNode = createASTNode(AST_EQ, 2);
		break;
	case 120:
		currASTNode = createASTNode(AST_NE, 2);
		break;
	case 121:
		currASTNode = createASTNode(AST_Declare, 2);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 122:
		currASTNode = createASTNode(AST_Switch, 3);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 123:
	case 124:
		currASTNode = createASTNode(AST_Case, 2);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_Statements, 1));
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		currASTNode->listNext = buildAST(currChild);
		removeAndAdvance(&currChild);
		break;
	case 128:
		currASTNode = createASTNode(AST_DefaultCase, 1);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_Statements, 1));
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 130:
		currASTNode = createASTNode(AST_For, 3);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_Statements, 1));
		insertChild(currASTNode->children[2], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		break;
	case 131:
		currASTNode = createASTNode(AST_While, 2);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);

		insertChild(currASTNode, createASTNode(AST_Statements, 1));
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeAndAdvance(&currChild);
		removeAndAdvance(&currChild);
		break;
	case 133:
		currASTNode = createASTNode(AST_SignedForIndex, 2);
		insertChild(currASTNode, buildAST(currChild));
		removeAndAdvance(&currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeAndAdvance(&currChild);
		break;
	}

	// printf("Rule %d done\n", rule);
	// fflush(stdout);

	return currASTNode;
}