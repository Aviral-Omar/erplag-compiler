/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdlib.h>
#include <string.h>

#include "lookupTableDef.h"

int hash(char* key);
void populateLookupTable();
KeywordPair* searchKeyword(char* key);
void deleteLookupTable();

KeywordPair* hashTable[TABLE_SIZE] = {NULL};

int hash(char* key)
{
	int hashCode = -16;
	for (int c = 0; key[c] != '\0'; c++)
		hashCode += (key[c] - 'A');
	return hashCode;
}

void populateLookupTable()
{
	char* keywords[30] = {"integer", "real", "boolean", "of", "array", "start", "end", "declare", "module", "driver", "program", "get_value", "print", "use", "with", "parameters", "takes", "input", "returns", "for", "in", "switch", "case", "break", "default", "while", "AND", "OR", "true", "false"};

	Token tokens[30] = {INTEGER, REAL, BOOLEAN, OF, ARRAY, START, END, DECLARE, MODULE, DRIVER, PROGRAM, GET_VALUE, PRINT, USE, WITH, PARAMETERS, TAKES, INPUT, RETURNS, FOR, IN, SWITCH, CASE, BREAK, DEFAULT, WHILE, AND, OR, TRUE, FALSE};

	for (int i = 0; i < 30; i++) {
		int hashIndex = hash(keywords[i]);

		// Used linear probing
		while (hashTable[hashIndex])
			hashIndex++;

		KeywordPair* p = (KeywordPair*)malloc(sizeof(KeywordPair));
		strcpy(p->keyword, keywords[i]);
		p->token = tokens[i];
		hashTable[hashIndex] = p;
	}
}

KeywordPair* searchKeyword(char* key)
{
	int hashIndex = hash(key);
	if (hashIndex < 0 || hashIndex >= TABLE_SIZE || !hashTable[hashIndex])
		return NULL;

	// Linear Probing
	while (hashTable[hashIndex]) {
		if (!strcmp(hashTable[hashIndex]->keyword, key))
			return hashTable[hashIndex];
		hashIndex++;
	}
	return NULL;
}

void deleteLookupTable()
{
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (hashTable[i]) {
			free(hashTable[i]);
			hashTable[i] = NULL;
		}
	}
}
