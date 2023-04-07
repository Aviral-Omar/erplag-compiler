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
void insertIntoSymbolTable(SymbolTableEntry* stEntry, SymbolTable* st);

int calcSize(TypeInfo* info);
void insertDeclaration(ASTNode* node);
TypeInfo* createTypeInfo(ASTNode* node);
void insertParams(ASTNode* moduleNode, SymbolTable* st);
void assignSymbolTables(ASTNode* node, SymbolTable* st);
void insertStatements(ASTNode* node, SymbolTable* parentST);
void insertDefinition(ASTNode* node);
void insertDriver(ASTNode* node);
void createSymbolTables();


int stHash(char* key)
{
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

void insertIntoSymbolTable(SymbolTableEntry* stEntry, SymbolTable* st)
{
	// TODO ask if variable overriding supported
	int hashCode = stHash(stEntry->idInfo->name);
	if (!st->stArray[hashCode])
		st->stArray[hashCode] = stEntry;
	else {
		SymbolTableEntry* entry = st->stArray[hashCode];
		while (entry->next)
			entry = entry->next;
		entry->next = stEntry;
	}
}

int calcSize(TypeInfo* info)
{
	// TODO clear sizes from maam
	if (!info) {
		printf("null");
		fflush(stdout);
	}
	printf("Type = %d\n", info->type);
	fflush(stdout);

	if (info->type == DT_Integer) {
		printf("Return int\n");
		fflush(stdout);
		return INT_SIZE;
	}
	if (info->type == DT_Real) {
		printf("Return real\n");
		fflush(stdout);
		return REAL_SIZE;
	}
	if (info->type == DT_Boolean) {
		printf("Return bool\n");
		fflush(stdout);
		return BOOL_SIZE;
	}
	// Else it is array
	if (!info->isStatic)
		return POINTER_SIZE;
	ArrayInfo* arrInfo = info->arrInfo;
	int arrayTypeSize, arrayLen;
	if (arrInfo->arrayType == DT_Integer)
		arrayTypeSize = INT_SIZE;
	else if (arrInfo->arrayType == DT_Real)
		arrayTypeSize = REAL_SIZE;
	else if (arrInfo->arrayType == DT_Boolean)
		arrayTypeSize = BOOL_SIZE;

	arrayLen = arrInfo->uBoundSign * arrInfo->upperBound.numBound - arrInfo->lBoundSign * arrInfo->lowerBound.numBound + 1;
	printf("Array Type = %d, Size = %d, uBound = %d %d, lBound = %d %d, Len = %d\n", arrInfo->arrayType, arrayTypeSize, arrInfo->uBoundSign, arrInfo->upperBound.numBound, arrInfo->lBoundSign, arrInfo->lowerBound.numBound, arrayLen);
	fflush(stdout);
	return arrayTypeSize * arrayLen;
}

void insertDeclaration(ASTNode* node)
{
	FunctionTableEntry* fnEntry = findFunctionEntry(node->value->data.lexeme);

	if (fnEntry) {
		printf("Line %d: Semantic Error: Redeclared function %s\n", node->value->lineNumber, node->value->data.lexeme);
		return;
	}

	fnEntry = (FunctionTableEntry*)malloc(sizeof(FunctionTableEntry));
	fnEntry->declOrder = functionOrder++;
	fnEntry->isCalled = 0;
	fnEntry->isDriver = 0;
	fnEntry->width = 0;
	fnEntry->offset = -1;
	fnEntry->isDefined = 0;
	fnEntry->name = node->value->data.lexeme;
	fnEntry->next = NULL;
	fnEntry->paramCount = 0;
	fnEntry->paramList = NULL;
	fnEntry->returnCount = 0;
	fnEntry->retList = NULL;

	insertIntoFunctionTable(fnEntry);
}

TypeInfo* createTypeInfo(ASTNode* node)
{
	TypeInfo* typeInfo = (TypeInfo*)malloc(sizeof(TypeInfo));
	typeInfo->arrInfo = NULL;
	typeInfo->isStatic = 1;

	switch (node->nodeType) {
	case AST_Integer:
		typeInfo->type = DT_Integer;
		break;
	case AST_Real:
		typeInfo->type = DT_Real;
		break;
	case AST_Boolean:
		typeInfo->type = DT_Boolean;
		break;
	case AST_Array:
		typeInfo->type = DT_Array;
		typeInfo->arrInfo = (ArrayInfo*)malloc(sizeof(ArrayInfo));
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
			typeInfo->isStatic = 0;
		} else
			typeInfo->arrInfo->lowerBound.numBound = lBound->value->data.intValue;
		if (uBound->nodeType == AST_ID) {
			typeInfo->arrInfo->upperBound.idBound = uBound->value->data.lexeme;
			typeInfo->isStatic = 0;
		} else
			typeInfo->arrInfo->upperBound.numBound = uBound->value->data.intValue;
	}

	return typeInfo;
}

void insertParams(ASTNode* node, SymbolTable* symbolTable)
{
	FunctionTableEntry* ftEntry = findFunctionEntry(node->children[0]->value->data.lexeme);

	for (IDInfo* inputParam = ftEntry->paramList; inputParam != NULL; inputParam = inputParam->next) {
		SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		stEntry->idInfo = inputParam;
		stEntry->offset = symbolTable->size;
		// TODO ask if array sizes are to be calculated by preprocessing
		stEntry->width = calcSize(inputParam->typeInfo);
		stEntry->isParam = 1;
		stEntry->isReturnVar = 0;
		stEntry->next = NULL;
		symbolTable->size += stEntry->width;

		insertIntoSymbolTable(stEntry, symbolTable);
	}

	for (IDInfo* retVar = ftEntry->retList; retVar != NULL; retVar = retVar->next) {
		SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		stEntry->idInfo = retVar;
		stEntry->offset = symbolTable->size;
		// TODO ask if array sizes are to be calculated by preprocessing
		stEntry->width = calcSize(retVar->typeInfo);
		stEntry->isParam = 0;
		stEntry->isReturnVar = 1;
		stEntry->next = NULL;
		symbolTable->size += stEntry->width;

		insertIntoSymbolTable(stEntry, symbolTable);
	}
	printf("Params inserted\n");
	fflush(stdout);
}

void assignSymbolTables(ASTNode* node, SymbolTable* st)
{
	printf("Assigning %s\n", astNodeMap[node->nodeType]);
	fflush(stdout);
	if (node->nodeType == AST_Statements) {
		insertStatements(node, st);
		return;
	}

	node->st = st;

	if (node->nodeType == AST_Declare) {
		TypeInfo* type = createTypeInfo(node->children[1]);

		for (ASTNode* idNode = node->children[0]; idNode != NULL; idNode = idNode->listNext) {
			SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			stEntry->idInfo = (IDInfo*)malloc(sizeof(IDInfo));
			stEntry->idInfo->typeInfo = type;
			stEntry->offset = st->size;
			stEntry->width = calcSize(type);
			stEntry->isParam = 0;
			stEntry->isReturnVar = 0;
			st->size += stEntry->width;

			insertIntoSymbolTable(stEntry, st);
		}
	}

	for (int i = 0; i < node->childCount; i++)
		if (node->children[i])
			assignSymbolTables(node->children[i], st);

	if (node->listNext)
		assignSymbolTables(node->listNext, st);
}

void insertStatements(ASTNode* node, SymbolTable* parentST)
{
	SymbolTable* symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
	symbolTable->parentST = parentST;
	symbolTable->isRoot = 0;

	for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
		symbolTable->stArray[i] = NULL;

	if (node->parent->nodeType == AST_Module) {
		symbolTable->isRoot = 1;
		insertParams(node->parent, symbolTable);
	}

	node->st = symbolTable;

	assignSymbolTables(node->children[0], symbolTable);
}

void insertDefinition(ASTNode* node)
{
	FunctionTableEntry* fnEntry = findFunctionEntry(node->children[0]->value->data.lexeme);

	if (!fnEntry) {
		fnEntry = (FunctionTableEntry*)malloc(sizeof(FunctionTableEntry));
		fnEntry->declOrder = functionOrder++;
		fnEntry->isCalled = 0;
		fnEntry->isDriver = 0;
		fnEntry->width = 0;
		fnEntry->offset = -1;
		fnEntry->isDefined = 0;
		fnEntry->name = node->children[0]->value->data.lexeme;
		fnEntry->next = NULL;
		fnEntry->paramCount = 0;
		fnEntry->paramList = NULL;
		fnEntry->returnCount = 0;
		fnEntry->retList = NULL;
		// fnEntry->returnInfo = NULL;
		insertIntoFunctionTable(fnEntry);
	} else if (fnEntry->isDefined) {
		printf("Line %d: Semantic Error: Redefined function %s\n", node->children[0]->value->lineNumber, node->children[0]->value->data.lexeme);
		return;
	}

	fnEntry->offset = fnTableOffset;
	fnEntry->isDefined = 1;

	ASTNode* iPList = node->children[1];
	ASTNode* oPList = node->children[2];

	// TODO handle array reference in Input parameters
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

	insertStatements(node->children[3], NULL);
}

void insertDriver(ASTNode* node)
{
	FunctionTableEntry* fnEntry = findFunctionEntry("driver");

	if (!fnEntry) {
		fnEntry = (FunctionTableEntry*)malloc(sizeof(FunctionTableEntry));
		fnEntry->declOrder = functionOrder++;
		fnEntry->isCalled = 0;
		fnEntry->isDriver = 1;
		fnEntry->width = 0;
		fnEntry->offset = fnTableOffset;
		fnEntry->isDefined = 1;
		fnEntry->name = "driver";
		fnEntry->next = NULL;
		fnEntry->paramCount = 0;
		fnEntry->paramList = NULL;
		fnEntry->returnCount = 0;
		fnEntry->retList = NULL;
		insertIntoFunctionTable(fnEntry);
	} else if (fnEntry->isDefined) {
		printf("Semantic Error: Driver function already defined\n");
		return;
	}

	insertStatements(node->children[0], NULL);
}

void createSymbolTables()
{
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
			insertDefinition(node);
			node = node->listNext;
		}
	}

	node = astRoot->children[2];
	insertDriver(node);


	node = astRoot->children[3];

	if (node->childCount) {
		node = node->children[0];
		while (node) {
			insertDefinition(node);
			node = node->listNext;
		}
	}
}
