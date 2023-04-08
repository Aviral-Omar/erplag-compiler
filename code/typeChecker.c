/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>
#include <string.h>

#include "astDef.h"
#include "symbolTable.h"

void checkBounds(ArrayInfo* arrInfo, ASTNode* indexNode);
int areTypesEqual(TypeInfo* t1, TypeInfo* t2);
TypeInfo* findExpressionType(ASTNode* node);
void checkStatements(ASTNode* stNode);
void checkTypes();

void checkBounds(ArrayInfo* arrInfo, ASTNode* indexNode)
{
	if (!arrInfo->isStatic)
		return;
	if (indexNode->nodeType != AST_Num && indexNode->nodeType != AST_SignedIndex)
		return;

	int sign = 1;
	if (indexNode->nodeType == AST_SignedIndex) {
		if (indexNode->children[1]->nodeType == AST_ID)
			return;
		indexNode = indexNode->children[1];
		sign = indexNode->children[0]->nodeType == AST_Minus ? -1 : 1;
	}

	int index = sign * indexNode->value->data.intValue;
	if (index >= arrInfo->lBoundSign * arrInfo->lowerBound.numBound && index <= arrInfo->uBoundSign * arrInfo->upperBound.numBound)
		return;
	printf("Line %d: Semantic Error: Array index out of bounds\n", indexNode->value->lineNumber);
}

int areTypesEqual(TypeInfo* t1, TypeInfo* t2)
{
	if (!t1 || !t2)
		return 0;
	if (t1->type != DT_Array)
		return t1->type == t2->type;
	// else they are arrays
	ArrayInfo *a1 = t1->arrInfo, *a2 = t2->arrInfo;
	if (a1->isStatic && a2->isStatic) {
		return (a1->arrayType == a2->arrayType && a1->lBoundSign == a2->lBoundSign && a1->lowerBound.numBound == a2->lowerBound.numBound && a1->uBoundSign == a2->uBoundSign && a1->upperBound.numBound == a2->upperBound.numBound);
	}
	// dynamic arrays
	return (a1->arrayType == a2->arrayType && a1->lBoundSign == a2->lBoundSign && !strcmp(a1->lowerBound.idBound, a2->lowerBound.idBound) && a1->uBoundSign == a2->uBoundSign && !strcmp(a1->upperBound.idBound, a2->upperBound.idBound));
}

TypeInfo* findExpressionType(ASTNode* node)
{
	TypeInfo *t1, *t2;
	switch (node->nodeType) {
	case AST_Num:
	case AST_RNum:
	case AST_True:
	case AST_False:
		return createTypeInfo(node);
	case AST_ArrayAccess: {
		t1 = findExpressionType(node->children[1]->children[1]);
		if (t1 && t1->type != DT_Integer) {
			printf("Line %d: Semantic Error: Array Expression not of type integer\n", node->value->lineNumber);
			free(t1);
		}
		SymbolTableEntry* stEntry = findSymbolEntry(node->children[0]->value->data.lexeme, node->children[0]->st);
		if (!stEntry)
			return NULL;
		else
			checkBounds(stEntry->idInfo->typeInfo->arrInfo, node->children[1]);

		t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
		t1->type = stEntry->idInfo->typeInfo->arrInfo->arrayType;
		t1->arrInfo = NULL;
		return t1;
	}
	case AST_ID: {
		SymbolTableEntry* stEntry = findSymbolEntry(node->value->data.lexeme, node->st);
		if (!stEntry)
			return NULL;

		t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
		memcpy(t1, stEntry->idInfo->typeInfo, sizeof(TypeInfo));
		return t1;
	}

	case AST_IndexWithExpressions:
	case AST_SignedIndex:
	case AST_UnaryOpExpr:
		return findExpressionType(node->children[1]);

	case AST_Plus:
	case AST_Minus:
	case AST_Mul:
		t1 = findExpressionType(node->children[0]), t2 = findExpressionType(node->children[1]);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		if (areTypesEqual(t1, t2) && t1->type != DT_Array && t1->type != DT_Boolean)
			return t1;
		// else error
		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		return NULL;
	case AST_Div:
		t1 = findExpressionType(node->children[0]), t2 = findExpressionType(node->children[1]);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		if (t1->type != DT_Array && t1->type != DT_Boolean && t2->type != DT_Array && t2->type != DT_Boolean) {
			t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
			t1->type = DT_Real;
			t1->arrInfo = NULL;
			return t1;
		}
		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		return NULL;
	case AST_LE:
	case AST_LT:
	case AST_GE:
	case AST_GT:
	case AST_EQ:
	case AST_NE:
		t1 = findExpressionType(node->children[0]), t2 = findExpressionType(node->children[1]);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		if (areTypesEqual(t1, t2) && t1->type != DT_Array && t1->type != DT_Boolean) {
			t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
			t1->type = DT_Boolean;
			t1->arrInfo = NULL;
			return t1;
		}
		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		return NULL;
	case AST_AND:
	case AST_OR:
		t1 = findExpressionType(node->children[0]), t2 = findExpressionType(node->children[1]);
		if (t1->type == DT_Boolean && t2->type == DT_Boolean)
			return t1;

		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		return NULL;
	default:
		return NULL;
	}
}

void checkStatements(ASTNode* stmtNode)
{
	stmtNode = stmtNode->children[0];
	if (!stmtNode)
		return;
	TypeInfo *t1, *t2;
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_Assign:
			t1 = findExpressionType(stmtNode->children[0]), t2 = findExpressionType(stmtNode->children[1]);
			if (!t1 || !t2 || !areTypesEqual(t1, t2))
				printf("Line %d: Semantic Error: Mismatch between type of LHS and RHS of assignment\n", stmtNode->children[0]->value->lineNumber);
			if (t1) free(t1);
			if (t2) free(t2);
			break;
		case AST_ArrayAssign:
			t1 = findExpressionType(stmtNode->children[0]), t2 = findExpressionType(stmtNode->children[1]);
			if (!t1 || !t2 || !areTypesEqual(t1, t2))
				printf("Line %d: Semantic Error: Mismatch between type of LHS and RHS of assignment\n", stmtNode->children[0]->children[0]->value->lineNumber);
			if (t1) free(t1);
			if (t2) free(t2);
			break;
		case AST_Print:
			t1 = findExpressionType(stmtNode->children[0]);
			if (t1) free(t1);
			break;
		case AST_While:
			t1 = findExpressionType(stmtNode->children[0]);
			if (!t1 || t1->type != DT_Boolean)
				printf("Line %d: Semantic Error: While expression is not of boolean type\n", stmtNode->children[0]->value->lineNumber);
			if (t1) free(t1);
			checkStatements(stmtNode->children[1]);
			break;
		case AST_Switch: {
			t1 = findExpressionType(stmtNode->children[0]);
			if (t1 && t1->type == DT_Boolean && stmtNode->children[3])
				printf("Line %d: Semantic Error: Default case in boolean switch\n", stmtNode->children[0]->value->lineNumber);
			if (t1 && (t1->type != DT_Boolean || t1->type != DT_Integer)) {
				printf("Line %d: Semantic Error: Invalid type of switch variable\n", stmtNode->children[0]->value->lineNumber);
				free(t1);
				return;
			}
			ASTNode* caseNode = stmtNode->children[1];
			while (caseNode) {
				t2 = findExpressionType(caseNode->children[0]);
				if (!t2 || t2->type != t1->type)
					printf("Line %d: Semantic Error: Mismatch type of case value\n", caseNode->children[0]->value->lineNumber);
				if (t2) free(t2);

				checkStatements(caseNode->children[1]);
				caseNode = caseNode->listNext;
			}
			if (t1) free(t1);
			caseNode = stmtNode->children[2];
			if (caseNode)
				checkStatements(caseNode->children[0]);
			break;
		}
		case AST_For:
			break;
		case AST_FunctionCall:
			break;
		}
		// TODO Null check
		stmtNode = stmtNode->listNext;
	}
}

void checkTypes()
{
	ASTNode* module = astRoot->children[1]->children[0];
	while (module) {
		checkStatements(module->children[3]);
		module = module->listNext;
	}

	module = astRoot->children[3]->children[0];
	while (module) {
		checkStatements(module->children[3]);
		module = module->listNext;
	}
}
