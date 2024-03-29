/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef LEXER_DEF
#define LEXER_DEF

#include <stdio.h>

#include "symbolDef.h"

typedef struct {
	Token token;
	unsigned int lineNumber;
	union lexemeData {
		char lexeme[21];
		int intValue;
		float floatValue;
	} data;
} TokenInfo;

typedef enum {
	FIRST,
	SECOND
} CurrentBuffer;

extern char* tokenMap[59];
extern char* lexemeMap[];
extern FILE* src;
extern int bufferSize;
extern char* lexemeBegin;
extern int charsRead;
extern TokenInfo* currToken;
extern int lexerCorrect;
extern int lexerPrint;

#endif
