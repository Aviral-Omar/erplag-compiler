/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include "treeDef.h"

ParseTNode *createParseTree(Symbol d, char type);
ParseTNode *addNode(ParseTNode *node, Symbol d, char type, int ruleNum);
void removeNode(ParseTNode *node);
// int updateData(ParseTNode *node, Symbol d, char type);
void printParseTree(ParseTNode *node, FILE *outFile);
void deleteParseTree(ParseTNode *node);
