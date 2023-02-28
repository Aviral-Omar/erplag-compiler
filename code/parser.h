#include "parserDef.h"

int findSymbol(char *symbol);
void readGrammar();
void printGrammar();
void insertIntoFirst(NonTerminal nt, Terminal t);
void computeFirstSets();
void printFirstSets();
void insertIntoFollow(NonTerminal nt, Terminal t);
int insertFirstIntoFollow(NonTerminal firstNT, NonTerminal followNT);
void insertFollowIntoFollow(NonTerminal nt1, NonTerminal nt2);
void computeFollowSets();
void printFollowSets();
void computeFirstAndFollowSets();
void populateSyn();
void synRecovery();
ParseTNode *createParseTree(union symbol d, char type);
ParseTNode *addNode(ParseTNode *node, union symbol d, char type);
int updateData(ParseTNode *node, union symbol d, char type);

// ParseTNode *addNode(ParseTNode *node,union symbol *d);
