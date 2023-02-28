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

extern char* tokenMap[];
extern FILE* src;
extern int bufferSize;
extern char* buf1;
extern char* buf2;
extern char* lexemeBegin;
extern char* forward;
extern int currLine;
extern int charsRead;
extern CurrentBuffer currBuffer;
extern TokenInfo* currToken;
extern int syntaxCorrect;
extern int lexerPrint;

#endif
