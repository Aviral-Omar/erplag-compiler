#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"
#include "stackDef.h"
#include "symbolDef.h"

#define GRAMMAR_FILE "grammar.txt"
#define RULE_COUNT 139
#define NON_TERMINAL_COUNT 71
#define TERMINAL_COUNT 58
#define GRAMMAR_BUFFER_SIZE 4500
#define SYN -2

// TODO insert comment against each data struct, and in which function it is used.

typedef struct LexicalSymbol LexicalSymbol;
struct LexicalSymbol {
	Symbol data;
	LexicalSymbol* next;
	char type;				// may be 'T', 'N', 'e'
	char followCalculated;	// used during follow computation
};

typedef struct TerminalInfo TerminalInfo;
struct TerminalInfo {
	Terminal tr;
	TerminalInfo* next;
};

typedef struct {
	TerminalInfo* first;
	TerminalInfo* follow;
} FirstFollowEntry;

typedef int ParseTableEntry;

typedef struct StackType Stack;

// TODO make sure all are cleaned
extern LexicalSymbol* grammar[RULE_COUNT];
extern char* nonTerminalMap[NON_TERMINAL_COUNT];
extern char* terminalMap[TERMINAL_COUNT];
extern ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];
extern FirstFollowEntry ffTable[NON_TERMINAL_COUNT];
extern Stack* s;
#endif
