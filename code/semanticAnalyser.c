/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <string.h>

#include "astDef.h"
#include "symbolTable.h"
#include "symbolTableDef.h"

void checkReturnSemantics(FunctionTableEntry* ftEntry, int lineNumber);
void checkForSemanticsHelper(char* name, ASTNode* stmtNode);
void checkForSemantics(ASTNode* stmtNode);
void checkRecursion(ASTNode* stmtNode);
void handleStatements(ASTNode* stmtsNode);
void checkSemantics();


void checkReturnSemantics(FunctionTableEntry* ftEntry, int lineNumber)
{
	SymbolTableEntry* stEntry;

	// Checking if all return values have been assigned values
	// valueAssigned set during type checking
	for (IDInfo* retVal = ftEntry->retList; retVal; retVal = retVal->next) {
		stEntry = findSymbolEntry(retVal->name, ftEntry->st);
		if (!stEntry->valueAssigned)
			printf("Line %d: Semantic Error: No value assigned to return variable %s\n", lineNumber, retVal->name);
	}
}

void checkForSemanticsHelper(char* name, ASTNode* stmtNode)
{
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_Assign:
			if (!strcmp(name, stmtNode->children[0]->value->data.lexeme))
				printf("Line %d: Semantic Error: For loop iterator variable %s reassigned\n", stmtNode->children[0]->value->lineNumber, name);
			break;
		case AST_FunctionCall:
			for (ASTNode* varNode = stmtNode->children[0]->children[0]; varNode; varNode = varNode->listNext)
				if (!strcmp(name, varNode->value->data.lexeme))
					printf("Line %d: Semantic Error: For loop iterator variable %s reassigned\n", varNode->value->lineNumber, name);

			break;
		case AST_Switch:
			for (ASTNode* tmp = stmtNode->children[1]; tmp; tmp = tmp->listNext)
				checkForSemanticsHelper(name, tmp->children[1]->children[0]);
			if (stmtNode->children[2])
				checkForSemanticsHelper(name, stmtNode->children[2]->children[0]->children[0]);
			break;
		case AST_While:
			checkForSemanticsHelper(name, stmtNode->children[1]->children[0]);
			break;
		case AST_For:
			checkForSemanticsHelper(name, stmtNode->children[2]->children[0]);
		}
		stmtNode = stmtNode->listNext;
	}
}

void checkForSemantics(ASTNode* stmtNode)
{
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_For: {
			char* varName = stmtNode->children[0]->value->data.lexeme;
			checkForSemanticsHelper(varName, stmtNode->children[2]->children[0]);
			checkForSemantics(stmtNode->children[2]->children[0]);
			break;
		case AST_Switch:
			for (ASTNode* tmp = stmtNode->children[1]; tmp; tmp = tmp->listNext)
				checkForSemantics(tmp->children[1]->children[0]);
			if (stmtNode->children[2])
				checkForSemantics(stmtNode->children[2]->children[0]->children[0]);
			break;
		case AST_While:
			checkForSemantics(stmtNode->children[1]->children[0]);
		}
		}
		stmtNode = stmtNode->listNext;
	}
}

void checkRecursion(ASTNode* stmtNode)
{
	FunctionTableEntry* ftEntry;
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_FunctionCall:
			ftEntry = findFunctionEntry(stmtNode->children[1]->value->data.lexeme);
			if (ftEntry) {
				if (ftEntry->isCalled) {
					printf("Line %d: Semantic Error: Module %s called recursively", stmtNode->children[1]->value->lineNumber, stmtNode->children[1]->value->data.lexeme);
					break;
				}
				ftEntry->isCalled = 1;
				checkRecursion(ftEntry->moduleNode->children[3]->children[0]);
				ftEntry->isCalled = 0;
			}
			break;
		case AST_For: {
			checkRecursion(stmtNode->children[2]);
			break;
		case AST_Switch:
			for (ASTNode* tmp = stmtNode->children[1]; tmp; tmp = tmp->listNext)
				checkRecursion(tmp->children[1]);
			if (stmtNode->children[2])
				checkRecursion(stmtNode->children[2]);
			break;
		case AST_While:
			checkRecursion(stmtNode->children[1]);
		}
		}
		stmtNode = stmtNode->listNext;
	}
}

void handleStatements(ASTNode* stmtsNode)
{
	FunctionTableEntry* ftEntry;
	if (stmtsNode->parent->nodeType == AST_Module) {
		ftEntry = findFunctionEntry(stmtsNode->parent->children[0]->value->data.lexeme);
		checkReturnSemantics(ftEntry, stmtsNode->parent->children[0]->value->lineNumber);
	} else if (stmtsNode->parent->nodeType == AST_Driver) {
		ftEntry = findFunctionEntry("driver");
		checkRecursion(stmtsNode->children[0]);
	}


	checkForSemantics(stmtsNode->children[0]);
	// TODO assign while variables some value (TC5)
}

void checkSemantics()
{
	ASTNode* module = astRoot->children[1]->children[0];
	while (module) {
		handleStatements(module->children[3]);
		module = module->listNext;
	}

	module = astRoot->children[2];
	handleStatements(module->children[0]);

	module = astRoot->children[3]->children[0];
	while (module) {
		handleStatements(module->children[3]);
		module = module->listNext;
	}
}
