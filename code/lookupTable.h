#ifndef LOOKUP_TABLE
#define LOOKUP_TABLE
#include <stdlib.h>

#include "lexerDef.h"
#define TABLE_SIZE 411

typedef struct {
	char keyword[11];
	Token token;
} KeywordPair;

extern KeywordPair* hashTable[TABLE_SIZE];

int hash(char* key);

void populateLookupTable();

KeywordPair* search(char* key);

void deleteTable();

#endif