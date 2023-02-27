#ifndef STACK_DEF
#define STACK_DEF
#include "parserDef.h"

typedef struct SNodeType SNode;
struct SNodeType {
	union nodeData data;
	struct SNodeType* next;
	ParseTNode* Treenode;
};

typedef struct StackType {
	SNode* top;
} Stack;

#endif
