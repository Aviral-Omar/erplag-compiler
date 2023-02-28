#include "treeDef.h"

ParseTNode *createParseTree(union symbol d);
ParseTNode *addNode(ParseTNode *node, union symbol d, char type);
int updateData(ParseTNode *node, union symbol d, char type);