/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef TREE_DEF
#define TREE_DEF

#include "lexerDef.h"
#include "symbolDef.h"

typedef struct ParseTNodeType ParseTNode;
struct ParseTNodeType {
	Symbol data;
	char type;	// 'T', 'N' or 'e'
	union TNodeInfo {
		TokenInfo* tokIn;
		int ruleNum;
	} info;
	ParseTNode* child;
	ParseTNode* parent;
	ParseTNode* prevSibling;
	ParseTNode* nextSibling;
};

extern ParseTNode* parseTreeRoot;
#endif
