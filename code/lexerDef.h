#ifndef LEXER_DEF
#define LEXER_DEF

#include <stdio.h>
typedef enum {
	INTEGER,
	REAL,
	BOOLEAN,
	OF,
	ARRAY,
	START,
	END,
	DECLARE,
	MODULE,
	DRIVER,
	PROGRAM,
	GET_VALUE,
	PRINT,
	USE,
	WITH,
	PARAMETERS,
	TAKES,
	INPUT,
	RETURNS,
	FOR,
	IN,
	SWITCH,
	CASE,
	BREAK,
	DEFAULT,
	WHILE,
	ID,
	NUM,
	RNUM,
	AND,
	OR,
	TRUE,
	FALSE,
	PLUS,
	MINUS,
	MUL,
	DIV,
	LT,
	LE,
	GE,
	GT,
	EQ,
	NE,
	DEF,
	ENDDEF,
	DRIVERDEF,
	DRIVERENDDEF,
	COLON,
	RANGEOP,
	SEMICOL,
	COMMA,
	ASSIGNOP,
	SQBO,
	SQBC,
	BO,
	BC,
	COMMENTMARK
} Token;

typedef struct {
	Token token;
	unsigned int lineNumber;
	union lexemeData {
		// TODO Storing as string for ease of conversion
		char lexeme[21];
		int intValue;
		float floatValue;
	} data;
} TokenInfo;

typedef enum {
	FIRST,
	SECOND
} CurrentBuffer;

extern FILE* fp;
extern int bufferSize;
extern char* buf1;
extern char* buf2;
extern char* lexemeBegin;
extern char* forward;
extern int currLine;
extern int charsRead;
extern CurrentBuffer currBuffer;

#endif
