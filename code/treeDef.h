/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
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
