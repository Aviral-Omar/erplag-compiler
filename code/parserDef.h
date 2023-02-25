#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"

typedef enum {
	// TODO populate using grammar
	N_PROGRAM
} NonTerminal;

// aliasing Token enum
typedef Token Terminal;

typedef struct {
	union symbolData {
		NonTerminal nt;
		Terminal t;
	} data;
	LexicalSymbol* next;
	char type;
} LexicalSymbol;

typedef struct {
	Terminal tr;
	int rule;
	TerminalInfo* next;
} TerminalInfo;

typedef struct {
	NonTerminal nt;
	TerminalInfo* first;
	TerminalInfo* follow;
} FirstFollowEntry;

extern LexicalSymbol* grammar[];
extern char* nonTerminalMap[];
extern char* terminalMap[];

#endif
