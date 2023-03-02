/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
// Contains only functions accessed by files other than lexer.c
#include "lexerDef.h"

void initLexer();

void clearLexerData();

void handleWhitespaces();

void getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

char *BUFEND();

void runOnlyLexer(char *filename);
