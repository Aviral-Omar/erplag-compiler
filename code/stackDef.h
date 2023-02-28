#ifndef STACK_DEF
#define STACK_DEF

#include "parserDef.h"

typedef struct SNodeType SNode;
struct SNodeType {
	union symbol data;
	char type; // 'T', 'N' or 'e'
	struct SNodeType* next;
	ParseTNode* treenode;
};

typedef struct StackType {
	SNode* top;
	int size;
} Stack;

#endif
