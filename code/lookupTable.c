#include "lookupTable.h"

KeywordPair* hashTable[TABLE_SIZE] = {NULL};

int hash(char* key)
{
	int hashCode = -16;
	for (int c = 0; key[c] != '\0'; c++)
		hashCode += (key[c] - 'a');
	return hashCode;
}

void populateLookupTable()
{
	char* keywords[30] = {"integer", "real", "boolean", "of", "array", "start", "end", "declare", "module", "driver", "program", "get_value", "print", "use", "with", "parameters", "takes", "input", "returns", "for", "in", "switch", "case", "break", "default", "while", "AND", "OR", "true", "false"};

	Token tokens[30] = {INTEGER, REAL, BOOLEAN, OF, ARRAY, START, END, DECLARE, MODULE, DRIVER, PROGRAM, GET_VALUE, PRINT, USE, WITH, PARAMETERS, TAKES, INPUT, RETURNS, FOR, IN, SWITCH, CASE, BREAK, DEFAULT, WHILE, AND, OR, TRUE, FALSE};

	for (int i = 0; i < 30; i++) {
		int hashIndex = hash(keywords[i]);

		/*TODO Discuss whether to keep linear probing*/
		// Used linear probing
		while (hashTable[hashIndex])
			hashIndex++;
		KeywordPair* p = (KeywordPair*)malloc(sizeof(KeywordPair));
		strcpy(p->keyword, keywords[i]);
		p->token = tokens[i];
		hashTable[hashIndex] = p;
	}
}

KeywordPair* search(char* key)
{
	int hashIndex = hash(key);
	if (hashIndex < 0 || !hashTable[hashIndex])
		return NULL;

	// Linear Probing
	while (hashTable[hashIndex]) {
		if (strcmp(hashTable[hashIndex]->keyword, key))
			return hashTable[hashIndex];
		hashIndex++;
	}
	return NULL;
}

/* TODO call at end of lexer */
void deleteTable()
{
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (hashTable[i])
			free(hashTable[i]);
	}
}
