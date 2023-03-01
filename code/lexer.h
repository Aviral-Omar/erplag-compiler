// Contains only functions accessed by files other than lexer.c
#include "lexerDef.h"

void initLexer();

void clearLexerData();

void handleWhitespaces();

void getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

char *BUFEND();

void runOnlyLexer(char *filename);
