#include <stdlib.h>

#include "tree.h"


ParseTNode *createParseTree(union symbol d, char type)
{
	ParseTNode *node;

	ParseTNode *p = (ParseTNode *)malloc(sizeof(struct ParseTNodeType));
    node->child = NULL;
	node->parent = NULL;
	node->sibling = NULL;
    node->type = type;
	node->data = d;
	return p;
}

ParseTNode *addNode(ParseTNode *parent, union symbol d, char type)
{
	ParseTNode *temp, *trav;  // creating new node and one for traversal(to find the terminal node)

    ParseTNode *temp = (ParseTNode *)malloc(sizeof(struct ParseTNodeType));
	temp->data = d;
	temp->parent = parent;
	temp->child = NULL;
	temp->sibling = NULL;

	if (parent->child == NULL) {
		parent->child = temp;
	} else {
		trav = parent->child;
		while (trav->sibling != NULL) {
			trav = trav->sibling;
		}
		trav->sibling = temp;
	}
	return temp;
}

int updateData(ParseTNode *node,union symbol d, char type)
{
    node->data = d;
    node->type = type;
}