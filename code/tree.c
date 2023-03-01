#include <stdio.h>
#include <stdlib.h>

#include "treeDef.h"

ParseTNode *parseTreeParent;

ParseTNode *createParseTree(Symbol d, char type);
ParseTNode *addNode(ParseTNode *node, Symbol d, char type);
int updateData(ParseTNode *node, Symbol d, char type);


ParseTNode *createParseTree(Symbol d, char type)
{
	ParseTNode *node = (ParseTNode *)malloc(sizeof(ParseTNode));
	node->child = NULL;
	node->parent = NULL;
	node->sibling = NULL;
	node->type = type;
	node->data = d;
	// printf("Parse tree created, root: %d\n",d);
	return node;
}

ParseTNode *addNode(ParseTNode *parent, Symbol d, char type)
{
	ParseTNode *temp, *trav;  // creating new node and one for traversal(to find the terminal node)

	temp = (ParseTNode *)malloc(sizeof(struct ParseTNodeType));
	temp->data = d;
	temp->parent = parent;
	temp->type = type;
	temp->child = NULL;
	temp->sibling = NULL;
	// printf("1: %d\n", d.nt);

	if (parent->child == NULL) {
		parent->child = temp;
	} else {
		trav = parent->child;
		while (trav->sibling != NULL) {
			trav = trav->sibling;
		}
		trav->sibling = temp;
	}
	// printf("2\n");
	return temp;
}

int updateData(ParseTNode *node, Symbol d, char type)
{
	node->data = d;
	node->type = type;
}
