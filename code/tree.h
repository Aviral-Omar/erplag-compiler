#include "treeDef.h"

ParseTNode *createParseTree(Symbol d, char type);
ParseTNode *addNode(ParseTNode *node, Symbol d, char type);
int updateData(ParseTNode *node, Symbol d, char type);
