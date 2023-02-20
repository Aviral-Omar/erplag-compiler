#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "lookupTable.h"

void getStream()
{
	// Reads stream into buf1 or buf2
	fread(currBuffer == FIRST ? buf1 : buf2, sizeof(char), bufferSize, fp);
	if (ferror(fp)) {
		printf("Error reading from file.\n");
		exit(EXIT_FAILURE);
	}
	if (feof(fp))
		// fclose returns EOF when there is error else returns 0
		if (fclose(fp) == EOF)
			printf("Error closing file.\n");
}

void checkOverflow()
{
	if (forward - (currBuffer == FIRST ? buf1 : buf2) == bufferSize) {
		// This toggles currBuffer between FIRST and SECOND
		currBuffer = (currBuffer + 1) & 1;
		getStream();
		forward = currBuffer == FIRST ? buf1 : buf2;
	}
}

TokenInfo *getNextToken()
{
	// keep getting tokens till EOF
	TokenInfo *tk = (TokenInfo *)malloc(sizeof(TokenInfo));
	int charsRead = 0;

	switch (*lexemeBegin) {
	// Handles case of identifier
	default: {
		if (*lexemeBegin == '_' || isalpha(*lexemeBegin)) {
			tk->data.lexeme[charsRead] = *lexemeBegin;
			charsRead++;
			forward++;
			checkOverflow();
			while (*forward == '_' || isalnum(*forward)) {
				tk->data.lexeme[charsRead] = *forward;
				charsRead++;
				forward++;
				checkOverflow();
				if (charsRead > 20) {
				}
			}
		}
	}
	}

	forward++;
	checkOverflow();
	lexemeBegin = forward;
	return tk;
}

void removeComments(char *testcaseFile, char *cleanFile)
{
}
