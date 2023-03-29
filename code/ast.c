/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>

#include "astDef.h"
#include "tree.h"
#define createChildrenArray(size) size != 0 ? (ASTNode**)malloc(size * sizeof(ASTNode)) : NULL

ASTNode* astRoot;

ASTNode* createASTNode(ASTNodeType type, int childCount);
void createAST();
void insertChild(ASTNode* parent, ASTNode* child);
ASTNode* buildAST(ParseTNode* currNode);

ASTNode* createASTNode(ASTNodeType type, int childCount)
{
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->nodeType = type;
	// node->parent = parent;
	node->childCount = 0;
	node->children = createChildrenArray(childCount);
	node->dataType = NULL;
	node->listNext = NULL;
	node->nodeSyn = NULL;

	return node;
}

void createAST()
{
	astRoot = buildAST(parseTreeRoot);
}

void insertChild(ASTNode* parent, ASTNode* child)
{
	parent->children[parent->childCount] = child;
	parent->childCount++;
}

void handleID(ParseTNode* idNode, ASTNode* newNode)
{
	newNode = createASTNode(AST_ID, 0);
	newNode->value = idNode->info.tokIn;
	idNode->info.tokIn = NULL;
}

ASTNode* buildAST(ParseTNode* currNode)
{
	ASTNode* currASTNode = NULL;
	// removeNode automatically changes currChild to next sibling
	ParseTNode* currChild = currNode->child;
	// To align with semantic rules
	switch (currNode->info.ruleNum + 1) {
	case 1:
		currASTNode = createASTNode(AST_Program, 4);

		currASTNode->children[currASTNode->childCount++] = createASTNode(AST_ModuleDeclarations, 1);
		insertChild(currASTNode->children[0], buildAST(currChild));
		removeNode(currChild);

		currASTNode->children[currASTNode->childCount++] = createASTNode(AST_ModuleDefinitions, 1);
		insertChild(currASTNode->children[1], buildAST(currChild));
		removeNode(currChild);

		insertChild(currASTNode, buildAST(currChild));
		removeNode(currChild);

		currASTNode->children[currASTNode->childCount++] = createASTNode(AST_ModuleDeclarations, 1);
		insertChild(currASTNode->children[3], buildAST(currChild));
		removeNode(currChild);
		break;
	case 2:
		currASTNode = buildAST(currChild);
		removeNode(currChild);
		currASTNode->listNext = buildAST(currChild);
		removeNode(currChild);
		break;
	case 3:
		removeNode(currNode->child);
		break;
	case 4:
		removeNode(currChild);
		removeNode(currChild);
		handleID(currNode, currASTNode);
		removeNode(currChild);
		removeNode(currChild);

		break;
	}
	return currASTNode;
}
