/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "parserDef.h"
#include "treeDef.h"

ParseTNode *parseTreeRoot;

ParseTNode *createParseTree(Symbol d, char type);
ParseTNode *addNode(ParseTNode *node, Symbol d, char type, int ruleNum);
void removeNode(ParseTNode *node);
// int updateData(ParseTNode *node, Symbol d, char type);
void deleteParseTree(ParseTNode *node);

ParseTNode *createParseTree(Symbol d, char type)
{
	ParseTNode *node = (ParseTNode *)malloc(sizeof(ParseTNode));
	node->child = NULL;
	node->parent = NULL;
	node->nextSibling = NULL;
	node->type = type;
	node->data = d;
	node->info.tokIn = NULL;
	// printf("Parse tree created, root: %d\n",d);
	return node;
}

ParseTNode *addNode(ParseTNode *parent, Symbol d, char type, int ruleNum)
{
	ParseTNode *temp, *trav;  // creating new node and one for traversal(to find the terminal node)

	temp = (ParseTNode *)malloc(sizeof(struct ParseTNodeType));
	temp->data = d;
	temp->parent = parent;
	temp->type = type;
	temp->child = NULL;
	temp->prevSibling = NULL;
	temp->nextSibling = NULL;
	temp->info.tokIn = NULL;
	if (type == 'N' || type == 'e')
		temp->info.ruleNum = ruleNum;

	if (!parent->child) {
		parent->child = temp;
	} else {
		trav = parent->child;
		while (trav->nextSibling) {
			trav = trav->nextSibling;
		}
		trav->nextSibling = temp;
		temp->prevSibling = trav;
	}
	return temp;
}

void removeNode(ParseTNode *node)
{
	if (!node->prevSibling)
		node->parent->child = node->nextSibling;
	else
		node->prevSibling->nextSibling = node->nextSibling;
	if (node->nextSibling)
		node->nextSibling->prevSibling = node->prevSibling;
	if (node->type == 'T' && node->info.tokIn)
		free(node->info.tokIn);
	node = node->nextSibling;
	free(node);
}

void deleteParseTree(ParseTNode *node)
{
	if (node->child)
		deleteParseTree(node->child);
	node->child = NULL;
	if (node->nextSibling)
		deleteParseTree(node->nextSibling);
	node->nextSibling = NULL;

	if (node->type == 'T' && node->info.tokIn)
		free(node->info.tokIn);

	free(node);
}

void printParseTree(ParseTNode *node, FILE *outFile)
{
	if (node == NULL)
		return;
	printParseTree(node->child, outFile);
	if (node->type == 'N') {
		fprintf(outFile, "%-24s\t", "---");
		fprintf(outFile, "%-10s\t", "---");
		fprintf(outFile, "%-13s\t", "---");
		fprintf(outFile, "%-16s\t", "---");
		if (!node->parent)
			fprintf(outFile, "%-30s\t", "ROOT");  // Parent node symbol
		else
			fprintf(outFile, "%-30s\t", nonTerminalMap[node->parent->data.nt]);	 // Parent node symbol
		fprintf(outFile, "%-6s\t", "no");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->data.nt]);
	} else if (node->type == 'T') {
		Token t = node->data.t;
		if (t == NUM || t == RNUM || t == ID)
			fprintf(outFile, "%-24s\t", node->info.tokIn->data.lexeme);	 // lexeme
		else
			fprintf(outFile, "%-24s\t", lexemeMap[t]);	// lexeme
		fprintf(outFile, "%-10d\t", node->info.tokIn->lineNumber);
		fprintf(outFile, "%-13s\t", tokenMap[node->data.t]);
		if (t == NUM)
			fprintf(outFile, "%-16d\t", atoi(node->info.tokIn->data.lexeme));
		else if (t == RNUM)
			fprintf(outFile, "%-16f\t", atof(node->info.tokIn->data.lexeme));
		else
			fprintf(outFile, "%-16s\t", "---");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->parent->data.nt]);	 // Parent node symbol
		fprintf(outFile, "%-6s\t", "yes");
		fprintf(outFile, "%-30s\t", "---");
	} else {
		fprintf(outFile, "%-24s\t", "---");
		fprintf(outFile, "%-10s\t", "---");
		fprintf(outFile, "%-13s\t", tokenMap[node->data.t]);
		fprintf(outFile, "%-16s\t", "---");
		fprintf(outFile, "%-30s\t", nonTerminalMap[node->parent->data.nt]);	 // Parent node symbol
		fprintf(outFile, "%-6s\t", "yes");
		fprintf(outFile, "%-30s\t", "---");
	}
	fprintf(outFile, "\n");
	fflush(outFile);

	if (node->child) {
		ParseTNode *nextSibling = node->child->nextSibling;
		while (nextSibling != NULL) {
			printParseTree(nextSibling, outFile);
			nextSibling = nextSibling->nextSibling;
		}
	}
	return;
}
