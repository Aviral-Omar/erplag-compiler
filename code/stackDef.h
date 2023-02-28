#ifndef STACK_DEF
#define STACK_DEF

#include "symbolDef.h"
#include "treeDef.h"

typedef struct ParseTNodeType ParseTNode;
typedef struct SNodeType SNode;
struct SNodeType {
	Symbol data;
	char type;	// 'T', 'N' or 'e'
	struct SNodeType* next;
	ParseTNode* treenode;
};

typedef struct StackType {
	SNode* top;
	int size;
} Stack;

#endif
