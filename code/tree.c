/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "treeDef.h"

ParseTNode *createParseTree(Symbol d, char type);
ParseTNode *addNode(ParseTNode *node, Symbol d, char type, int ruleNum);
// int updateData(ParseTNode *node, Symbol d, char type);
void deleteParseTree(ParseTNode *node);
ParseTNode *createParseTree(Symbol d, char type)
{
	ParseTNode *node = (ParseTNode *)malloc(sizeof(ParseTNode));
	node->child = NULL;
	node->parent = NULL;
	node->sibling = NULL;
	node->type = type;
	node->data = d;
	node->info.ruleNum = -1;
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
	temp->sibling = NULL;
	if (type == 'N' || type == 'e')
		temp->info.ruleNum = ruleNum;
	else if (type == 'T')
		temp->info.tokIn = currToken;

	if (!parent->child) {
		parent->child = temp;
	} else {
		trav = parent->child;
		while (trav->sibling) {
			trav = trav->sibling;
		}
		trav->sibling = temp;
	}
	return temp;
}

// int updateData(ParseTNode *node, Symbol d, char type)
// {
// 	node->data = d;
// 	node->type = type;
// }

void deleteParseTree(ParseTNode *node)
{
	// TODO free tokeninfo here
	if (node->sibling)
		deleteParseTree(node->sibling);
	if (node->child)
		deleteParseTree(node->child);

	free(node);
}
