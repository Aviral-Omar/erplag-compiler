/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>
#include <string.h>

#include "astDef.h"
#include "symbolTableDef.h"

FunctionTableEntry* functionTable[SYMBOL_TABLE_SIZE];

int functionOrder = 1;
int fnTableOffset = 0;

int stHash(char* key);
FunctionTableEntry* findFunctionEntry(char* name);
SymbolTableEntry* findSymbolEntry(char* name, SymbolTable* st);
void insertIntoFunctionTable(FunctionTableEntry* fnEntry);
int insertIntoSymbolTable(SymbolTableEntry* stEntry, SymbolTable* st);
int calcSize(TypeInfo* info);
void checkBounds(ArrayInfo* arrInfo, ASTNode* indexNode);
int areTypesEqual(TypeInfo* t1, TypeInfo* t2);
TypeInfo* findExpressionType(ASTNode* node, SymbolTable* st);
TypeInfo* createTypeInfo(ASTNode* node);
void insertDeclaration(ASTNode* node);
void insertParams(ASTNode* moduleNode, SymbolTable* st);
void handleNodes(ASTNode* node, SymbolTable* st);
void insertStatements(ASTNode* node, SymbolTable* parentST);
void insertDefinition(ASTNode* node);
void insertDriver(ASTNode* node);
void createSymbolTables();


int stHash(char* key)
{
	// TODO change hash function
	// printf("Hashing %s\n", key);
	// fflush(stdout);
	int hashCode = -96 + key[0];
	int c;
	for (c = 0; key[c] != '\0'; c++)
		;
	hashCode += key[c - 1];
	return hashCode % SYMBOL_TABLE_SIZE;
}

FunctionTableEntry* findFunctionEntry(char* name)
{
	int hashCode = stHash(name);
	FunctionTableEntry* fnEntry = functionTable[hashCode];

	while (fnEntry && strcmp(name, fnEntry->name))
		fnEntry = fnEntry->next;

	return fnEntry;
}

SymbolTableEntry* findSymbolEntry(char* name, SymbolTable* st)
{
	int hashCode = stHash(name);

	SymbolTableEntry* stEntry = st->stArray[hashCode];

	while (stEntry && strcmp(name, stEntry->idInfo->name))
		stEntry = stEntry->next;

	if (stEntry || st->isRoot)
		return stEntry;

	return findSymbolEntry(name, st->parentST);
}

void insertIntoFunctionTable(FunctionTableEntry* fnEntry)
{
	int hashCode = stHash(fnEntry->name);
	if (!functionTable[hashCode])
		functionTable[hashCode] = fnEntry;
	else {
		FunctionTableEntry* entry = functionTable[hashCode];
		while (entry->next)
			entry = entry->next;
		entry->next = fnEntry;
	}
}

int insertIntoSymbolTable(SymbolTableEntry* stEntry, SymbolTable* st)
{
	// TODO ask if variable overriding supported
	int hashCode = stHash(stEntry->idInfo->name);
	if (!st->stArray[hashCode])
		st->stArray[hashCode] = stEntry;
	else {
		SymbolTableEntry* entry = st->stArray[hashCode];
		while (entry->next) {
			if (!strcmp(entry->idInfo->name, stEntry->idInfo->name))
				return 1;
			entry = entry->next;
		}
		if (!strcmp(entry->idInfo->name, stEntry->idInfo->name))
			return 1;
		entry->next = stEntry;
	}
	return 0;
}

int calcSize(TypeInfo* info)
{
	// TODO clear sizes from maam
	// printf("Type = %d\n", info->type);
	// fflush(stdout);

	if (info->type == DT_Integer)
		return INT_SIZE;
	if (info->type == DT_Real)
		return REAL_SIZE;
	if (info->type == DT_Boolean)
		return BOOL_SIZE;
	// Else it is array
	ArrayInfo* arrInfo = info->arrInfo;
	if (!arrInfo->isStatic)
		return POINTER_SIZE;
	int arrayTypeSize, arrayLen;
	if (arrInfo->arrayType == DT_Integer)
		arrayTypeSize = INT_SIZE;
	else if (arrInfo->arrayType == DT_Real)
		arrayTypeSize = REAL_SIZE;
	else if (arrInfo->arrayType == DT_Boolean)
		arrayTypeSize = BOOL_SIZE;

	arrayLen = arrInfo->uBoundSign * arrInfo->upperBound.numBound - arrInfo->lBoundSign * arrInfo->lowerBound.numBound + 1;
	// printf("Array Type = %d, Size = %d, uBound = %d %d, lBound = %d %d, Len = %d\n", arrInfo->arrayType, arrayTypeSize, arrInfo->uBoundSign, arrInfo->upperBound.numBound, arrInfo->lBoundSign, arrInfo->lowerBound.numBound, arrayLen);
	// fflush(stdout);
	return arrayTypeSize * arrayLen;
}

void checkBounds(ArrayInfo* arrInfo, ASTNode* indexNode)
{
	if (!arrInfo->isStatic)
		return;

	int sign = (indexNode->children[0] && indexNode->children[0]->nodeType) == AST_Minus ? -1 : 1;
	indexNode = indexNode->children[1];

	if (indexNode->nodeType != AST_Num)
		return;

	int index = sign * indexNode->value->data.intValue;

	if (index < arrInfo->lBoundSign * arrInfo->lowerBound.numBound || index > arrInfo->uBoundSign * arrInfo->upperBound.numBound)
		printf("Line %d: Semantic Error: Index %d used for array %s out of bounds\n", indexNode->value->lineNumber, index, indexNode->parent->parent->children[0]->value->data.lexeme);
}

int areTypesEqual(TypeInfo* t1, TypeInfo* t2)
{
	if (!t1 || !t2)
		return 0;
	if (t1->type != t2->type)
		return 0;
	// else they are of same type
	if (t1->type != DT_Array)
		return 1;
	// else they are arrays
	ArrayInfo *a1 = t1->arrInfo, *a2 = t2->arrInfo;
	if (a1->isStatic && a2->isStatic) {
		return (a1->arrayType == a2->arrayType && a1->lBoundSign == a2->lBoundSign && a1->lowerBound.numBound == a2->lowerBound.numBound && a1->uBoundSign == a2->uBoundSign && a1->upperBound.numBound == a2->upperBound.numBound);
	}
	// dynamic arrays
	return (a1->arrayType == a2->arrayType && a1->lBoundSign == a2->lBoundSign && !strcmp(a1->lowerBound.idBound, a2->lowerBound.idBound) && a1->uBoundSign == a2->uBoundSign && !strcmp(a1->upperBound.idBound, a2->upperBound.idBound));
}

TypeInfo* findExpressionType(ASTNode* node, SymbolTable* st)
{
	TypeInfo *t1, *t2;
	switch (node->nodeType) {
	case AST_Num:
	case AST_RNum:
	case AST_True:
	case AST_False:
		return createTypeInfo(node);
	case AST_ArrayAccess: {
		t1 = findExpressionType(node->children[1]->children[1], st);
		if (t1 && t1->type != DT_Integer) {
			printf("Line %d: Semantic Error: Array Expression not of type integer\n", node->value->lineNumber);
			free(t1);
		}
		SymbolTableEntry* stEntry = findSymbolEntry(node->children[0]->value->data.lexeme, st);
		if (!stEntry)
			return NULL;

		checkBounds(stEntry->idInfo->typeInfo->arrInfo, node->children[1]);

		t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
		t1->type = stEntry->idInfo->typeInfo->arrInfo->arrayType;
		t1->arrInfo = NULL;
		return t1;
	}
	case AST_ID: {
		SymbolTableEntry* stEntry = findSymbolEntry(node->value->data.lexeme, st);
		if (!stEntry)
			return NULL;

		t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
		memcpy(t1, stEntry->idInfo->typeInfo, sizeof(TypeInfo));
		return t1;
	}

	case AST_IndexWithExpressions:
	case AST_SignedIndex:
	case AST_UnaryOpExpr:
	case AST_SignedParam:
		return findExpressionType(node->children[1], st);

	case AST_Plus:
	case AST_Minus:
	case AST_Mul:
		t1 = findExpressionType(node->children[0], st);
		t2 = findExpressionType(node->children[1], st);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		// Types should be real or int and should be equal
		if (areTypesEqual(t1, t2) && t1->type != DT_Array && t1->type != DT_Boolean)
			return t1;
		// else error
		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		return NULL;
	case AST_Div:
		t1 = findExpressionType(node->children[0], st);
		t2 = findExpressionType(node->children[1], st);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		// types should be real or int only and return real
		if (t1->type != DT_Array && t1->type != DT_Boolean && t2->type != DT_Array && t2->type != DT_Boolean) {
			free(t1);
			free(t2);
			t1 = (TypeInfo*)malloc(sizeof(TypeInfo));
			t1->type = DT_Real;
			t1->arrInfo = NULL;
			return t1;
		}
		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		free(t1);
		free(t2);
		return NULL;
	case AST_LE:
	case AST_LT:
	case AST_GE:
	case AST_GT:
	case AST_EQ:
	case AST_NE:
		t1 = findExpressionType(node->children[0], st);
		t2 = findExpressionType(node->children[1], st);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		// types should be int or real only and equal and returns boolean
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
		t1 = findExpressionType(node->children[0], st);
		t2 = findExpressionType(node->children[1], st);
		if (!t1 || !t2) {
			if (t1) free(t1);
			if (t2) free(t2);
			return NULL;
		}
		// types should be boolean only and returns boolean
		if (t1->type == DT_Boolean && t2->type == DT_Boolean) {
			free(t2);
			return t1;
		}

		printf("Line %d: Semantic Error: Mismatched/wrong types in expression\n", node->value->lineNumber);
		return NULL;
	default:
		return NULL;
	}
}


TypeInfo* createTypeInfo(ASTNode* node)
{
	TypeInfo* typeInfo = (TypeInfo*)malloc(sizeof(TypeInfo));
	typeInfo->arrInfo = NULL;

	switch (node->nodeType) {
	case AST_Integer:
	case AST_Num:
		typeInfo->type = DT_Integer;
		break;
	case AST_Real:
	case AST_RNum:
		typeInfo->type = DT_Real;
		break;
	case AST_Boolean:
	case AST_True:
	case AST_False:
		typeInfo->type = DT_Boolean;
		break;
	case AST_Array:
		typeInfo->type = DT_Array;
		typeInfo->arrInfo = (ArrayInfo*)malloc(sizeof(ArrayInfo));
		typeInfo->arrInfo->isStatic = 1;
		ASTNodeType arrTypeNode = node->children[1]->nodeType;
		typeInfo->arrInfo->arrayType = (arrTypeNode == AST_Integer ? DT_Integer : (arrTypeNode == AST_Real ? DT_Real : DT_Boolean));

		ASTNode *lBoundSign, *uBoundSign, *lBound, *uBound;
		lBoundSign = node->children[0]->children[0]->children[0];
		uBoundSign = node->children[0]->children[1]->children[0];
		lBound = node->children[0]->children[0]->children[1];
		uBound = node->children[0]->children[1]->children[1];

		typeInfo->arrInfo->lBoundSign = (lBoundSign && lBoundSign->nodeType == AST_Minus) ? -1 : 1;
		typeInfo->arrInfo->uBoundSign = (uBoundSign && uBoundSign->nodeType == AST_Minus) ? -1 : 1;
		if (lBound->nodeType == AST_ID) {
			typeInfo->arrInfo->lowerBound.idBound = lBound->value->data.lexeme;
			typeInfo->arrInfo->isStatic = 0;
		} else
			typeInfo->arrInfo->lowerBound.numBound = lBound->value->data.intValue;
		if (uBound->nodeType == AST_ID) {
			typeInfo->arrInfo->upperBound.idBound = uBound->value->data.lexeme;
			typeInfo->arrInfo->isStatic = 0;
		} else
			typeInfo->arrInfo->upperBound.numBound = uBound->value->data.intValue;
	}

	return typeInfo;
}

void insertDeclaration(ASTNode* node)
{
	FunctionTableEntry* fnEntry = findFunctionEntry(node->value->data.lexeme);

	if (fnEntry) {
		printf("Line %d: Semantic Error: Redeclared module %s\n", node->value->lineNumber, node->value->data.lexeme);
		return;
	}

	fnEntry = (FunctionTableEntry*)malloc(sizeof(FunctionTableEntry));
	fnEntry->defnFound = 0;
	fnEntry->isCalled = 0;
	fnEntry->isUsed = 0;
	fnEntry->width = 0;
	fnEntry->offset = -1;
	fnEntry->isDeclared = 1;
	fnEntry->isDefined = 0;
	fnEntry->name = node->value->data.lexeme;
	fnEntry->next = NULL;
	fnEntry->paramCount = 0;
	fnEntry->paramList = NULL;
	fnEntry->returnCount = 0;
	fnEntry->retList = NULL;
	fnEntry->st = NULL;
	fnEntry->moduleNode = NULL;

	insertIntoFunctionTable(fnEntry);
}

void insertParams(ASTNode* node, SymbolTable* symbolTable)
{
	// TODO Top symbol table has input params and return variables only
	FunctionTableEntry* ftEntry = findFunctionEntry(node->children[0]->value->data.lexeme);
	ftEntry->st = symbolTable;

	for (IDInfo* inputParam = ftEntry->paramList; inputParam != NULL; inputParam = inputParam->next) {
		SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		stEntry->idInfo = inputParam;
		stEntry->offset = symbolTable->size;
		stEntry->width = calcSize(inputParam->typeInfo);
		stEntry->isParam = 1;
		stEntry->isReturnVar = 0;
		stEntry->valueAssigned = 0;
		stEntry->next = NULL;
		symbolTable->size += stEntry->width;

		TypeInfo* typeInfo = stEntry->idInfo->typeInfo;
		if (typeInfo->type == DT_Array && !typeInfo->arrInfo->isStatic && (!findSymbolEntry(typeInfo->arrInfo->lowerBound.idBound, symbolTable) || !findSymbolEntry(typeInfo->arrInfo->upperBound.idBound, symbolTable))) {
			printf("Line %d: Semantic Error: Identifier in array range not present in input parameters\n", node->children[0]->value->lineNumber);
		}

		if (insertIntoSymbolTable(stEntry, symbolTable))
			printf("Line %d: Semantic Error: Reused %s in module parameters\n", node->children[0]->value->lineNumber, stEntry->idInfo->name);
	}

	for (IDInfo* retVar = ftEntry->retList; retVar != NULL; retVar = retVar->next) {
		SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		stEntry->idInfo = retVar;
		stEntry->offset = symbolTable->size;
		stEntry->width = calcSize(retVar->typeInfo);
		stEntry->isParam = 0;
		stEntry->isReturnVar = 1;
		stEntry->valueAssigned = 0;
		stEntry->next = NULL;
		symbolTable->size += stEntry->width;

		if (insertIntoSymbolTable(stEntry, symbolTable))
			printf("Line %d: Semantic Error: Reused %s in module parameters\n", node->children[0]->value->lineNumber, stEntry->idInfo->name);
	}
	// printf("Params inserted\n");
	// fflush(stdout);
}

void handleNodes(ASTNode* node, SymbolTable* st)
{
	// printf("Assigning %s\n", astNodeMap[node->nodeType]);
	// fflush(stdout);
	node->st = st;

	TypeInfo *t1, *t2;

	switch (node->nodeType) {
	case AST_ID: {
		if (node->parent->nodeType != AST_FunctionCall && !findSymbolEntry(node->value->data.lexeme, st))
			printf("Line %d: Semantic Error: Variable %s not declared\n", node->value->lineNumber, node->value->data.lexeme);
		else if (node->parent->nodeType == AST_FunctionCall && !findFunctionEntry(node->value->data.lexeme))
			printf("Line %d: Semantic Error: Module %s not declared\n", node->value->lineNumber, node->value->data.lexeme);
		break;
	}

	case AST_Declare: {
		TypeInfo *t1 = createTypeInfo(node->children[1]), *t2;

		for (ASTNode* idNode = node->children[0]; idNode != NULL; idNode = idNode->listNext) {
			t2 = (TypeInfo*)malloc(sizeof(TypeInfo));
			memcpy(t2, t1, sizeof(TypeInfo));
			SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			stEntry->idInfo = (IDInfo*)malloc(sizeof(IDInfo));
			stEntry->idInfo->name = idNode->value->data.lexeme;
			stEntry->idInfo->typeInfo = t2;
			stEntry->idInfo->next = NULL;
			stEntry->offset = st->size;
			stEntry->width = calcSize(t2);
			stEntry->isParam = 0;
			stEntry->isReturnVar = 0;
			stEntry->valueAssigned = 0;
			stEntry->next = NULL;
			st->size += stEntry->width;

			if (insertIntoSymbolTable(stEntry, st))
				printf("Line %d: Semantic Error: Redeclared variable %s\n", node->children[0]->value->lineNumber, stEntry->idInfo->name);
		}
		free(t1);
		break;
	}

	case AST_Assign: {
		SymbolTableEntry* stEntry = findSymbolEntry(node->children[0]->value->data.lexeme, node->st);
		if (stEntry) stEntry->valueAssigned = 1;

		t1 = findExpressionType(node->children[0], st);
		t2 = findExpressionType(node->children[1], st);
		if (!t1 || !t2 || !areTypesEqual(t1, t2))
			printf("Line %d: Semantic Error: Mismatch between type of LHS and RHS of assignment\n", node->children[0]->value->lineNumber);
		if (t1) free(t1);
		if (t2) free(t2);
		break;
	}

	case AST_ArrayAssign:
		t1 = findExpressionType(node->children[0], st);
		t2 = findExpressionType(node->children[1], st);
		if (!t1 || !t2 || !areTypesEqual(t1, t2))
			printf("Line %d: Semantic Error: Mismatch between type of LHS and RHS of assignment\n", node->children[0]->children[0]->value->lineNumber);
		if (t1) free(t1);
		if (t2) free(t2);
		break;

	case AST_Print:
		// Done for bound checking
		t1 = findExpressionType(node->children[0], st);
		if (t1) free(t1);
		break;

	case AST_While:
		t1 = findExpressionType(node->children[0], st);
		if (!t1 || t1->type != DT_Boolean)
			printf("Line %d: Semantic Error: While expression is not of boolean type\n", node->children[0]->value->lineNumber);
		if (t1) free(t1);
		break;

	case AST_Switch: {
		t1 = findExpressionType(node->children[0], st);

		if (!t1 || (t1->type != DT_Boolean && t1->type != DT_Integer)) {
			printf("Line %d: Semantic Error: Invalid type of switch variable\n", node->children[0]->value->lineNumber);
			free(t1);
			break;
		}

		if (t1 && t1->type == DT_Boolean && node->children[2])
			printf("Line %d: Semantic Error: Default case in boolean switch\n", node->children[0]->value->lineNumber);

		if (t1 && t1->type == DT_Integer && !node->children[2])
			printf("Line %d: Semantic Error: No default case in integer switch\n", node->children[0]->value->lineNumber);

		ASTNode* caseNode = node->children[1];
		while (caseNode) {
			t2 = findExpressionType(caseNode->children[0], st);
			if (!t2 || t2->type != t1->type)
				printf("Line %d: Semantic Error: Mismatch type of case value\n", caseNode->children[0]->value->lineNumber);
			if (t2) free(t2);

			caseNode = caseNode->listNext;
		}
		if (t1) free(t1);
		break;
	}

	case AST_For:
		t1 = findExpressionType(node->children[0], st);
		if (t1->type != DT_Integer)
			printf("Line %d: Semantic Error: Variable %s must be of type integer\n", node->children[0]->value->lineNumber, node->children[0]->value->data.lexeme);
		free(t1);
		break;

	case AST_FunctionCall: {
		int varCount = 0, paramCount = 0;
		ASTNode *varList = node->children[0]->children[0], *actualPList = node->children[2]->children[0];
		FunctionTableEntry* ftEntry = findFunctionEntry(node->children[1]->value->data.lexeme);
		if (!ftEntry)
			break;

		ftEntry->isUsed = 1;
		if (!ftEntry->isDeclared && !ftEntry->isDefined) {
			printf("Line %d: Semantic Error: Module %s not defined\n", node->children[1]->value->lineNumber, node->children[1]->value->data.lexeme);
			break;
		}

		IDInfo *retList, *paramList;
		for (retList = ftEntry->retList; varList && retList; retList = retList->next, varList = varList->listNext, varCount++) {
			t1 = findExpressionType(varList, st);
			if (!areTypesEqual(retList->typeInfo, t1))
				printf("Line %d: Semantic Error: Output parameter %s of wrong type\n", node->children[1]->value->lineNumber, varList->value->data.lexeme);
			if (t1) free(t1);
		}
		if (retList || varList)
			printf("Line %d: Semantic Error: Output parameter list size not equal to number of return values\n", node->children[1]->value->lineNumber);

		for (paramList = ftEntry->paramList; actualPList && paramList; paramList = paramList->next, actualPList = actualPList->listNext, paramCount++) {
			t1 = findExpressionType(actualPList, st);
			if (!areTypesEqual(paramList->typeInfo, t1))
				printf("Line %d: Semantic Error: Actual Parameter #%d of wrong type in function call of %s\n", node->children[1]->value->lineNumber, paramCount + 1, ftEntry->name);
			if (t1) free(t1);
		}
		if (paramList || actualPList)
			printf("Line %d: Semantic Error: Actual Parameter list size not equal to number of formal parameters\n", node->children[1]->value->lineNumber);
	}
	}

	for (int i = 0; i < node->childCount; i++) {
		if (node->children[i]) {
			if (node->children[i]->nodeType != AST_Statements)
				handleNodes(node->children[i], st);
			else
				insertStatements(node->children[i], st);
		}
	}

	if (node->listNext)
		handleNodes(node->listNext, st);
}

void insertStatements(ASTNode* node, SymbolTable* parentST)
{
	SymbolTable* symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
	symbolTable->parentST = parentST;
	symbolTable->isRoot = 0;

	for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
		symbolTable->stArray[i] = NULL;

	node->st = symbolTable;

	if (node->children[0])
		handleNodes(node->children[0], symbolTable);
}

void insertDefinition(ASTNode* node)
{
	FunctionTableEntry* fnEntry = findFunctionEntry(node->children[0]->value->data.lexeme);

	if (!fnEntry) {
		fnEntry = (FunctionTableEntry*)malloc(sizeof(FunctionTableEntry));
		fnEntry->isCalled = 0;
		fnEntry->isDeclared = 0;
		fnEntry->isDefined = 0;
		fnEntry->isUsed = 0;
		fnEntry->width = 0;
		fnEntry->name = node->children[0]->value->data.lexeme;
		fnEntry->next = NULL;
		fnEntry->paramCount = 0;
		fnEntry->paramList = NULL;
		fnEntry->returnCount = 0;
		fnEntry->retList = NULL;
		fnEntry->st = NULL;

		insertIntoFunctionTable(fnEntry);
	}

	fnEntry->defnFound = 1;
	++functionOrder;
	fnEntry->moduleNode = node;
	fnEntry->offset = fnTableOffset;

	ASTNode* iPList = node->children[1];
	ASTNode* oPList = node->children[2];

	if (iPList->childCount) {
		iPList = iPList->children[0];
		fnEntry->paramCount++;

		IDInfo* newID = (IDInfo*)malloc(sizeof(IDInfo));
		newID->next = NULL;
		newID->name = iPList->children[0]->value->data.lexeme;
		newID->typeInfo = createTypeInfo(iPList->children[1]);
		fnEntry->paramList = newID;
		iPList = iPList->listNext;

		while (iPList != NULL) {
			fnEntry->paramCount++;
			newID->next = (IDInfo*)malloc(sizeof(IDInfo));
			newID = newID->next;
			newID->next = NULL;
			newID->name = iPList->children[0]->value->data.lexeme;
			newID->typeInfo = createTypeInfo(iPList->children[1]);
			iPList = iPList->listNext;
		}
	}
	if (oPList->childCount) {
		oPList = oPList->children[0];
		fnEntry->returnCount++;

		IDInfo* newID = (IDInfo*)malloc(sizeof(IDInfo));
		newID->next = NULL;
		newID->name = oPList->children[0]->value->data.lexeme;
		newID->typeInfo = createTypeInfo(oPList->children[1]);
		fnEntry->retList = newID;
		oPList = oPList->listNext;

		while (oPList != NULL) {
			fnEntry->returnCount++;
			newID->next = (IDInfo*)malloc(sizeof(IDInfo));
			newID = newID->next;
			newID->next = NULL;
			newID->name = oPList->children[0]->value->data.lexeme;
			newID->typeInfo = createTypeInfo(oPList->children[1]);
			oPList = oPList->listNext;
		}
	}
}

void insertDriver(ASTNode* node)
{
	FunctionTableEntry* fnEntry = (FunctionTableEntry*)malloc(sizeof(FunctionTableEntry));
	fnEntry->defnFound = 1;
	++functionOrder;
	fnEntry->isCalled = 0;
	fnEntry->isDeclared = 0;
	fnEntry->isUsed = 0;
	fnEntry->width = 0;
	fnEntry->offset = fnTableOffset;
	fnEntry->isDefined = 0;
	fnEntry->name = "driver";
	fnEntry->next = NULL;
	fnEntry->paramCount = 0;
	fnEntry->paramList = NULL;
	fnEntry->returnCount = 0;
	fnEntry->retList = NULL;
	fnEntry->st = NULL;

	insertIntoFunctionTable(fnEntry);
}

void createSymbolTables()
{
	SymbolTable* symbolTable;
	FunctionTableEntry* fnEntry;

	for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
		functionTable[i] = NULL;

	functionOrder = 1;

	ASTNode* node = astRoot->children[0];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			insertDeclaration(node);
			node = node->listNext;
		}
	}

	node = astRoot->children[1];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			fnEntry = findFunctionEntry(node->children[0]->value->data.lexeme);
			if (!fnEntry || !fnEntry->defnFound)
				insertDefinition(node);
			else
				printf("Line %d: Semantic Error: Redefined module %s\n", node->children[0]->value->lineNumber, node->children[0]->value->data.lexeme);
			node = node->listNext;
		}
	}

	node = astRoot->children[2];
	insertDriver(node);


	node = astRoot->children[3];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			fnEntry = findFunctionEntry(node->children[0]->value->data.lexeme);
			if (!fnEntry || !fnEntry->defnFound)
				insertDefinition(node);
			else
				printf("Line %d: Semantic Error: Redefined module %s\n", node->children[0]->value->lineNumber, node->children[0]->value->data.lexeme);
			node = node->listNext;
		}
	}

	node = astRoot->children[1];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			fnEntry = findFunctionEntry(node->children[0]->value->data.lexeme);
			if (!fnEntry->isDefined) {
				symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
				symbolTable->parentST = NULL;
				symbolTable->isRoot = 1;

				for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
					symbolTable->stArray[i] = NULL;

				insertParams(node, symbolTable);

				node->st = symbolTable;

				if (fnEntry->isDeclared && !fnEntry->isUsed)
					printf("Line %d: Semantic Error: Module %s definition and its declaration both appear before its call\n", node->children[0]->value->lineNumber, node->children[0]->value->data.lexeme);

				insertStatements(node->children[3], symbolTable);
				fnEntry->isDefined = 1;
			}

			node = node->listNext;
		}
	}

	node = astRoot->children[2];

	fnEntry = findFunctionEntry("driver");
	if (!fnEntry->isDefined) {
		symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
		symbolTable->parentST = NULL;
		symbolTable->isRoot = 1;

		for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
			symbolTable->stArray[i] = NULL;

		node->st = symbolTable;

		insertStatements(node->children[0], symbolTable);
		fnEntry->isDefined = 1;
	} else
		printf("Semantic Error: Driver module already defined\n");

	node = astRoot->children[3];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			fnEntry = findFunctionEntry(node->children[0]->value->data.lexeme);
			if (!fnEntry->isDefined) {
				symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
				symbolTable->parentST = NULL;
				symbolTable->isRoot = 1;

				for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
					symbolTable->stArray[i] = NULL;

				insertParams(node, symbolTable);

				node->st = symbolTable;

				if (fnEntry->isDeclared && !fnEntry->isUsed)
					printf("Line %d: Semantic Error: Module %s definition and its declaration both appear before its call\n", node->children[0]->value->lineNumber, node->children[0]->value->data.lexeme);

				insertStatements(node->children[3], symbolTable);
				fnEntry->isDefined = 1;
			}

			node = node->listNext;
		}
	}

	node = astRoot->children[0];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			fnEntry = findFunctionEntry(node->value->data.lexeme);
			if (fnEntry->isDeclared && !fnEntry->isDefined)
				printf("Semantic Error: Module %s never defined\n", node->value->data.lexeme);
			node = node->listNext;
		}
	}
}
