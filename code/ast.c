/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>

#include "astDef.h"
#include "tree.h"
#define createChildrenArray(size) size != 0 ? (ASTNode**)malloc(size * sizeof(ASTNode)) : NULL

ASTNode* astRoot;

ASTNode* createASTNode(ASTNodeType type, int childCount);
void createAST();
void insertChild(ASTNode* parent, ASTNode* child);
void handleIDNumRNum(ParseTNode* idNode, ASTNode* newNode);
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
	parent->children[parent->childCount] = child;
	parent->childCount++;
}

void handleIDNumRNum(ParseTNode* idNode, ASTNode* newNode)
{
	Token token = idNode->info.tokIn->token;
	newNode = createASTNode(token == ID ? AST_ID : (token == NUM ? AST_Num : AST_RNum), 0);
	newNode->value = idNode->info.tokIn;
	idNode->info.tokIn = NULL;
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
	ASTNode* currASTNode = NULL;
	// removeNode automatically changes currChild to next sibling
	ParseTNode* currChild = currNode->child;
	int rule = currNode->info.ruleNum + 1;
	// To align with semantic rules
	switch (rule) {
	case 1:
		currASTNode = createASTNode(AST_Program, 4);

		insertChild(currASTNode, createASTNode(AST_ModuleDeclarations, 1));
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, createASTNode(AST_ModuleDefinitions, 1));
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, createASTNode(AST_ModuleDeclarations, 1));
		insertChild(currASTNode->children[3], buildAST(currChild));
		removeNode(currChild);
		break;
	case 2:
	case 5:
	case 26:
		currASTNode = buildAST(currChild);
		removeNode(currChild);
		currASTNode->listNext = buildAST(currChild);
		removeNode(currChild);
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
		removeNode(currChild);
		break;
	case 4:
		removeNode(currChild);
		removeNode(currChild);
		handleIDNumRNum(currNode, currASTNode);
		removeNode(currChild);
		removeNode(currChild);

		break;
	case 7:
		currASTNode = createASTNode(AST_Driver, 1);

		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);
		insertChild(currNode, buildAST(currChild));
		removeNode(currChild);

		break;
	case 8:
		currASTNode = createASTNode(AST_Module, 4);
		removeNode(currChild);
		removeNode(currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, createASTNode(AST_ParametersList, 1));
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		break;
	case 9:
	case 34:
		currASTNode = createASTNode(rule == 9 ? AST_ParametersList : AST_Print, 1);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		break;
	case 10:
		currASTNode = createASTNode(AST_ParametersList, 0);
		removeNode(currChild);
		break;
	case 11:
	case 14:
		currASTNode = createASTNode(rule == 11 ? AST_InputVarType : AST_OutputVarType, 1);
		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		currASTNode->listNext = buildAST(currChild);
		removeNode(currChild);

		break;
	case 12:
	case 15:
		currASTNode = createASTNode(rule == 12 ? AST_InputVarType : AST_OutputVarType, 1);
		removeNode(currChild);

		handleIDNumRNum(currChild, &currASTNode->children[currASTNode->childCount++]);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		currASTNode->listNext = buildAST(currChild);
		removeNode(currChild);
		break;
	case 17:
	case 22:
		currASTNode = createASTNode(AST_Integer, 0);
		removeNode(currChild);
		break;
	case 18:
	case 23:
		currASTNode = createASTNode(AST_Real, 0);
		removeNode(currChild);
		break;
	case 19:
	case 24:
		currASTNode = createASTNode(AST_Boolean, 0);
		removeNode(currChild);
		break;
	case 20:
		currASTNode = createASTNode(AST_Array, 2);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		break;
	case 21:
		currASTNode = createASTNode(AST_ArrayRange, 2);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		break;
	case 25:
		currASTNode = createASTNode(AST_Statements, 1);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
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
		removeNode(currChild);
		break;
	case 33:
		currASTNode = createASTNode(AST_GetValue, 1);
		removeNode(currChild);
		removeNode(currChild);

		handleIDNumRNum(currNode, &currASTNode->children[currASTNode->childCount++]);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);
		break;
	case 35:
	case 45:
		currChild = currChild->nextSibling;
		currChild->inh = currChild->prevSibling;

		currASTNode = buildAST(currChild->nextSibling);
		removeNode(currChild);

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
		handleIDNumRNum(currChild, currASTNode);
		removeNode(currChild);
		break;
	case 39:
	case 40:
		currASTNode = createASTNode(rule == 39 ? AST_True : AST_False, 0);
		removeNode(currChild);
		break;
	case 41:
	case 48:
		currASTNode = createASTNode(rule == 41 ? AST_ArrayAccess : AST_Assign, 2);
		removeNode(currChild);

		handleIDNumRNum(currNode->inh, &currASTNode->children[currASTNode->childCount++]);
		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currNode->inh);

		break;
	case 42:
		handleIDNumRNum(currNode->inh, currASTNode);
		removeNode(currChild);
		removeNode(currNode->inh);

		break;
	case 46:
	case 47:
		currChild->inh = currNode->inh;
		currASTNode = buildAST(currChild);
		removeNode(currChild);

		break;
	case 49:
		currASTNode = createASTNode(AST_ArrayAssign, 2);
		removeNode(currChild);

		insertChild(currASTNode, createASTNode(AST_ArrayAccess, 2));

		handleIDNumRNum(currNode->inh, &currASTNode->children[0]->children[currASTNode->children[0]->childCount++]);
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);

		break;
	case 50:
	case 71:
		currASTNode = createASTNode(rule == 50 ? AST_SignedIndex : AST_UnaryOpExpr, 2);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		break;
	case 53:
	case 74:
	case 109:
		currASTNode = createASTNode(AST_Plus, 0);
		removeNode(currChild);
		break;
	case 54:
	case 75:
	case 110:
		currASTNode = createASTNode(AST_Minus, 0);
		removeNode(currChild);
		break;
	case 56:
		currASTNode = createASTNode(AST_FunctionCall, 3);

		insertChild(currASTNode, createASTNode(AST_VariableList, 1));
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		handleIDNumRNum(currChild, currASTNode);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		insertChild(currASTNode, createASTNode(AST_ActualParametersList, 1));
		insertChild(currASTNode->children[2], buildAST(currChild));
		removeNode(currChild);
		removeNode(currChild);

		break;
	case 57:
		currASTNode = createASTNode(AST_SignedParam, 1);
		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		currASTNode->listNext = buildAST(currChild);
		removeNode(currChild);
		break;
	case 58:
		currASTNode = createASTNode(AST_SignedParam, 1);
		removeNode(currChild);
		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		currASTNode->listNext = buildAST(currChild);
		removeNode(currChild);
		break;
	case 63:
		currASTNode = createASTNode(AST_ArrayAccess, 2);

		handleIDNumRNum(currNode, &currASTNode->children[currASTNode->childCount++]);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		break;
	case 64:
		removeNode(currChild);
		currASTNode = buildAST(currChild);
		removeNode(currChild);
		removeNode(currChild);
		removeNode(currChild);

		break;
	case 66:
		handleIDNumRNum(currChild, currASTNode);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		break;
	case 67:
		removeNode(currChild);
		handleIDNumRNum(currChild, currASTNode);
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);
		break;
	case 72:
		removeNode(currChild);

		currASTNode = buildAST(currChild);
		removeNode(currChild);
		removeNode(currChild);
		break;
	case 79:
		currChild->nextSibling->inh = buildAST(currChild);
		removeNode(currChild);

		currASTNode = buildAST(currChild);
		removeNode(currChild);
		break;
	case 80:
		break;
	}

	return currASTNode;
}
