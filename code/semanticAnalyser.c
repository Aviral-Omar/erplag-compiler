/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>
#include <string.h>

#include "astDef.h"
#include "semanticAnalyserDef.h"
#include "symbolTable.h"
#include "symbolTableDef.h"

void checkForSemanticsHelper(SymbolTableEntry* stEntry, ASTNode* stmtNode);
WhileLL* extractIDs(ASTNode* exprNode);
int checkWhileSemanticsHelper(ASTNode* stmtNode, WhileLL* wll);
void checkSemantics(ASTNode* stmtNode, char* name);
void handleModuleNode(ASTNode* stmtsNode);
void runSemanticAnalyser();


void checkForSemanticsHelper(SymbolTableEntry* stEntry, ASTNode* stmtNode)
{
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_Assign:
			if (!strcmp(stEntry->idInfo->name, stmtNode->children[0]->value->data.lexeme) && stEntry == findSymbolEntry(stmtNode->children[0]->value->data.lexeme, stmtNode->st))
				printf("Line %d: Semantic Error: For loop iterator variable %s reassigned\n", stmtNode->children[0]->value->lineNumber, stEntry->idInfo->name);
			break;
		case AST_FunctionCall:
			for (ASTNode* varNode = stmtNode->children[0]->children[0]; varNode; varNode = varNode->listNext)
				if (!strcmp(stEntry->idInfo->name, varNode->value->data.lexeme) && stEntry == findSymbolEntry(varNode->value->data.lexeme, varNode->st))
					printf("Line %d: Semantic Error: For loop iterator variable %s reassigned\n", varNode->value->lineNumber, stEntry->idInfo->name);

			break;
		case AST_Switch:
			for (ASTNode* tmp = stmtNode->children[1]; tmp; tmp = tmp->listNext)
				checkForSemanticsHelper(stEntry, tmp->children[1]->children[0]);
			if (stmtNode->children[2])
				checkForSemanticsHelper(stEntry, stmtNode->children[2]->children[0]->children[0]);
			break;
		case AST_While:
			checkForSemanticsHelper(stEntry, stmtNode->children[1]->children[0]);
			break;
		case AST_For:
			checkForSemanticsHelper(stEntry, stmtNode->children[2]->children[0]);
		}
		stmtNode = stmtNode->listNext;
	}
}


WhileLL* extractIDs(ASTNode* exprNode)
{
	WhileLL *wll = NULL, *tmp = NULL;
	if (exprNode->nodeType == AST_ID) {
		wll = (WhileLL*)malloc(sizeof(WhileLL));
		wll->stEntry = findSymbolEntry(exprNode->value->data.lexeme, exprNode->st);
		wll->next = NULL;
		return wll;
	}
	if (exprNode->nodeType == AST_ArrayAccess) {
		wll = (WhileLL*)malloc(sizeof(WhileLL));
		wll->stEntry = findSymbolEntry(exprNode->children[0]->value->data.lexeme, exprNode->st);
		wll->next = NULL;
		return wll;
	}
	if (exprNode->childCount) {
		wll = extractIDs(exprNode->children[0]);
		tmp = wll;
		if (tmp) {
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = extractIDs(exprNode->children[1]);
		} else
			wll = extractIDs(exprNode->children[1]);
	}
	return wll;
}

int checkWhileSemanticsHelper(ASTNode* stmtNode, WhileLL* wll)
{
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_Assign:
			for (WhileLL* tmp = wll; tmp; tmp = tmp->next)
				if (!strcmp(tmp->stEntry->idInfo->name, stmtNode->children[0]->value->data.lexeme) && tmp->stEntry == findSymbolEntry(stmtNode->children[0]->value->data.lexeme, stmtNode->st))
					return 1;

			break;

		case AST_FunctionCall:
			for (WhileLL* tmp = wll; tmp; tmp = tmp->next)
				for (ASTNode* varNode = stmtNode->children[0]->children[0]; varNode; varNode = varNode->listNext)
					if (!strcmp(tmp->stEntry->idInfo->name, varNode->value->data.lexeme) && tmp->stEntry == findSymbolEntry(varNode->value->data.lexeme, varNode->st))
						return 1;

			break;

		case AST_Switch:
			for (ASTNode* caseNode = stmtNode->children[1]; caseNode; caseNode = caseNode->listNext)
				if (checkWhileSemanticsHelper(caseNode->children[1]->children[0], wll))
					return 1;
			if (stmtNode->children[2])
				if (checkWhileSemanticsHelper(stmtNode->children[2]->children[0]->children[0], wll))
					return 1;
			break;
		case AST_While:
			if (checkWhileSemanticsHelper(stmtNode->children[1]->children[0], wll))
				return 1;
			break;
		case AST_For:
			if (checkWhileSemanticsHelper(stmtNode->children[2]->children[0], wll))
				return 1;
		}
		stmtNode = stmtNode->listNext;
	}
	return 0;
}


void checkSemantics(ASTNode* stmtNode, char* name)
{
	while (stmtNode) {
		switch (stmtNode->nodeType) {
		case AST_FunctionCall: {
			// Checks recursion
			if (!strcmp(name, stmtNode->children[1]->value->data.lexeme))
				printf("Line %d: Semantic Error: Module %s called recursively\n", stmtNode->children[1]->value->lineNumber, stmtNode->children[1]->value->data.lexeme);
			break;
		}

		case AST_For: {
			SymbolTableEntry* stEntry = findSymbolEntry(stmtNode->children[0]->value->data.lexeme, stmtNode->st);
			checkForSemanticsHelper(stEntry, stmtNode->children[2]->children[0]);
			checkSemantics(stmtNode->children[2]->children[0], name);
			break;
		}

		case AST_Switch: {
			for (ASTNode* tmp = stmtNode->children[1]; tmp; tmp = tmp->listNext)
				checkSemantics(tmp->children[1]->children[0], name);
			if (stmtNode->children[2])
				checkSemantics(stmtNode->children[2]->children[0]->children[0], name);
			break;
		}

		case AST_While: {
			WhileLL* wll = extractIDs(stmtNode->children[0]);
			if (!checkWhileSemanticsHelper(stmtNode->children[1]->children[0], wll))
				printf("Line %d: Semantic Error: None of the conditional variables in while condition is assigned any value\n", stmtNode->children[0]->value->lineNumber);
			for (WhileLL* tmp = wll->next; wll; wll = tmp, tmp = tmp ? tmp->next : NULL)
				free(wll);
			checkSemantics(stmtNode->children[1]->children[0], name);
		}
		}
		stmtNode = stmtNode->listNext;
	}
}

void handleModuleNode(ASTNode* stmtsNode)
{
	if (stmtsNode->parent->nodeType == AST_Module) {
		// Checking if all return values have been assigned values
		// valueAssigned set during type checking
		FunctionTableEntry* ftEntry = findFunctionEntry(stmtsNode->parent->children[0]->value->data.lexeme);
		for (IDInfo* retVal = ftEntry->retList; retVal; retVal = retVal->next) {
			SymbolTableEntry* stEntry = findSymbolEntry(retVal->name, ftEntry->st);
			if (!stEntry->valueAssigned)
				printf("Line %d: Semantic Error: No value assigned to return variable %s\n", stmtsNode->parent->children[0]->value->lineNumber, retVal->name);
		}

		checkSemantics(stmtsNode->children[0], stmtsNode->parent->children[0]->value->data.lexeme);
	} else if (stmtsNode->parent->nodeType == AST_Driver)
		checkSemantics(stmtsNode->children[0], "driver");
}

void runSemanticAnalyser()
{
	ASTNode* module = astRoot->children[1]->children[0];
	while (module) {
		handleModuleNode(module->children[3]);
		module = module->listNext;
	}

	module = astRoot->children[2];
	handleModuleNode(module->children[0]);

	module = astRoot->children[3]->children[0];
	while (module) {
		handleModuleNode(module->children[3]);
		module = module->listNext;
	}
}
