#include "parserDef.h"

void initParser();
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
void runOnlyParser();
void parseCurrToken();