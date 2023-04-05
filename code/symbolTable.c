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

int hash(char* key);
FunctionTableEntry* findFunctionEntry(char* name);
void insertIntoFunctionTable(FunctionTableEntry* fnEntry);
int calcSize(TypeInfo* info);
void insertDeclaration(ASTNode* node);
TypeInfo* createTypeInfo(ASTNode* node);
void insertStatements(ASTNode* node, SymbolTable* parentST);
void insertDefinition(ASTNode* node);
void insertDriver(ASTNode* node);
void createSymbolTables();


int hash(char* key)
{
	int hashCode = -96 + key[0];
	int c;
	for (c = 0; key[c] != '\0'; c++)
		;
	hashCode += key[c - 1];
	return hashCode % SYMBOL_TABLE_SIZE;
}

FunctionTableEntry* findFunctionEntry(char* name)
{
	int hashCode = hash(name);
	FunctionTableEntry* fnEntry = functionTable[hashCode];

	while (fnEntry && strcmp(name, fnEntry->name))
		fnEntry = fnEntry->next;

	return fnEntry;
}

void insertIntoFunctionTable(FunctionTableEntry* fnEntry)
{
	int hashCode = hash(fnEntry->name);
	if (!functionTable[hashCode])
		functionTable[hashCode] = fnEntry;
	else {
		FunctionTableEntry* entry = functionTable[hashCode];
		while (entry->next)
			entry = entry->next;
		entry->next = fnEntry;
	}
}

int calcSize(TypeInfo* info)
{
	// TODO clear this
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
	// fnEntry->paramInfo = NULL;
	fnEntry->returnCount = 0;
	// fnEntry->returnInfo = NULL;

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
		typeInfo->arrInfo->arrayType = arrTypeNode == AST_Integer ? DT_Integer : (arrTypeNode == AST_Real ? DT_Real : DT_Boolean);

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
			typeInfo->arrInfo->lowerBound.idBound = lBound->value->data.intValue;
		if (uBound->nodeType == AST_ID) {
			typeInfo->arrInfo->upperBound.idBound = uBound->value->data.lexeme;
			typeInfo->isStatic = 0;
		} else
			typeInfo->arrInfo->upperBound.idBound = uBound->value->data.intValue;

		break;
	}

	return typeInfo;
}

void insertStatements(ASTNode* node, SymbolTable* parentST)
{
	ASTNodeType type = node->nodeType;

	SymbolTable* symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
	symbolTable->parentST = parentST;
	symbolTable->isRoot = 0;
	node->st = symbolTable;

	if (type == AST_Module) {
		symbolTable->isRoot = 1;
		FunctionTableEntry* ftEntry = findFunctionEntry(node->children[0]->value->data.lexeme);

		for (IDInfo* inputParam = ftEntry->paramList; inputParam != NULL; inputParam = inputParam->next) {
			SymbolTableEntry* stEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			stEntry->idInfo = inputParam;
			stEntry->offset = symbolTable->size;
			symbolTable->size += calcSize(inputParam->typeInfo);
		}

		node = node->children[2];
		type = node->nodeType;
	}


	// if (type == AST_Statements) {}
}

void insertDefinition(ASTNode* node)
{
	FunctionTableEntry* fnEntry = findFunctionEntry(node->value->data.lexeme);

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
		// fnEntry->paramInfo = NULL;
		fnEntry->returnCount = 0;
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
		newID->name = iPList->children[0];
		newID->typeInfo = createTypeInfo(iPList->children[1]);
		fnEntry->paramList = newID;
		iPList = iPList->listNext;

		while (iPList != NULL) {
			fnEntry->paramCount++;
			newID->next = (IDInfo*)malloc(sizeof(IDInfo));
			newID->name = iPList->children[0];
			newID->typeInfo = createTypeInfo(iPList->children[1]);
			newID = newID->next;
			iPList = iPList->listNext;
		}
	}
	if (oPList->childCount) {
		oPList = oPList->children[0];
		fnEntry->returnCount++;

		IDInfo* newID = (IDInfo*)malloc(sizeof(IDInfo));
		newID->name = oPList->children[0];
		newID->typeInfo = createTypeInfo(oPList->children[1]);
		fnEntry->retList = newID;
		oPList = oPList->listNext;

		while (oPList != NULL) {
			fnEntry->returnCount++;
			newID->next = (IDInfo*)malloc(sizeof(IDInfo));
			newID->name = oPList->children[0];
			newID->typeInfo = createTypeInfo(oPList->children[1]);
			newID = newID->next;
			oPList = oPList->listNext;
		}
	}

	// TODO insert statements
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
		// fnEntry->paramInfo = NULL;
		fnEntry->returnCount = 0;
		// fnEntry->returnInfo = NULL;
		insertIntoFunctionTable(fnEntry);
	} else if (fnEntry->isDefined) {
		printf("Semantic Error: Driver function already defined\n");
		return;
	}

	// TODO insert statements
}

void createSymbolTables()
{
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
		node = node->children[1];
		while (node) {
			insertDefinition(node);
			node = node->listNext;
		}
	}

	node = astRoot->children[2];
	if (node) {
		// TODO insert driver
	}

	node = astRoot->children[3];

	if (node->childCount) {
		node = node->children[3];
		while (node) {
			insertDefinition(node);
			node = node->listNext;
		}
	}
}
