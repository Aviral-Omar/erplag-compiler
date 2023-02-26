#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"

typedef enum {
	// TODO populate using grammar
	E,
	Edash,
	T,
	Tdash,
	F
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
	NonTerminal nt; //TODO No need for this nt here imo
	//TODO initialize both First,Follow below to NULL
	TerminalInfo* first;
	TerminalInfo* follow;
} FirstFollowEntry;

typedef struct{
	int rule; // -1 if its error
	// created a struct in case we want to add some other info later.
} ParseTableEntry;

extern LexicalSymbol* grammar[];
extern char* nonTerminalMap[];
extern char* terminalMap[];

#endif
