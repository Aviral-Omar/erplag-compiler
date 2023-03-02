/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"
#include "stackDef.h"
#include "symbolDef.h"

#define GRAMMAR_FILE "grammar.txt"
#define RULE_COUNT 138
#define NON_TERMINAL_COUNT 72
#define TERMINAL_COUNT 58
#define GRAMMAR_BUFFER_SIZE 4500
#define SYN -2

// TODO insert comment against each data struct, and in which function it is used.
typedef struct LexicalSymbol LexicalSymbol;
// Used to represent grammar
struct LexicalSymbol {
	Symbol data;
	LexicalSymbol* next;
	char type;				// may be 'T', 'N', 'e'
	char followCalculated;	// used during follow computation
};

typedef struct TerminalInfo TerminalInfo;
// Used to store terminals in first and follow sets
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

// TODO declare only those used outside
extern LexicalSymbol* grammar[RULE_COUNT];
extern char* nonTerminalMap[NON_TERMINAL_COUNT];
extern char* terminalMap[TERMINAL_COUNT];
extern ParseTableEntry parseTable[NON_TERMINAL_COUNT][TERMINAL_COUNT];
extern FirstFollowEntry ffTable[NON_TERMINAL_COUNT];
extern Stack* s;
extern ParseTNode* parseTreeParent;
extern int parserCorrect;
#endif
