/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
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

extern Stack* s;

#endif
