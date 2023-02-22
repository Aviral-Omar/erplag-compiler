#include "lexerDef.h"
#include "lookupTableDef.h"

int hash(char* key);

void populateLookupTable();

KeywordPair* searchKeyword(char* key);

void deleteLookupTable();
