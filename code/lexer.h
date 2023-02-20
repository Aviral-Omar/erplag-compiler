#include "lexerDef.h"

void clearHeap();

void getStream();

void incrementForward();

void handleWhitespaces();

TokenInfo *getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);
