#ifndef TREE_DEF
#define TREE_DEF

#include "parserDef.h"

typedef struct ParseTNodeType ParseTNode;
struct ParseTNodeType {
	union symbol data;
    char type; // 'T', 'N' or 'e'
	ParseTNode* child;
	ParseTNode* parent;
	ParseTNode* sibling;
};

extern ParseTNode ParseTreeParent;


// typedef struct ParseTreeType {
// 	ParseTNode* node;
// } ParseTree;

#endif