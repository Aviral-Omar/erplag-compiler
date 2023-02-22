#ifndef LOOKUP_TABLE_DEF
#define LOOKUP_TABLE_DEF

#include "lexerDef.h"
#define TABLE_SIZE 411

typedef struct {
	char keyword[11];
	Token token;
} KeywordPair;

extern KeywordPair* hashTable[TABLE_SIZE];

#endif
