#ifndef TREE_DEF
#define TREE_DEF

#include "symbolDef.h"

typedef struct ParseTNodeType ParseTNode;
struct ParseTNodeType {
	Symbol data;
	char type;	// 'T', 'N' or 'e'
	ParseTNode* child;
	ParseTNode* parent;
	ParseTNode* sibling;
};

#endif
