#include "lexerDef.h"

void clearHeap();

void getStream();

int incrementForward();

void retractForward();

void handleWhitespaces();

TokenInfo *getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

char *BUFEND();
