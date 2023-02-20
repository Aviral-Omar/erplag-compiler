#include "lexerDef.h"

void getStream();

void incrementForward();

void handleWhitespaces();

TokenInfo *getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);
