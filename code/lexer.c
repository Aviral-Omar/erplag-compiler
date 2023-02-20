#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "lookupTable.h"

FILE *fp;
int bufferSize;
char *buf1;
char *buf2;
CurrentBuffer currBuffer = FIRST;
char *lexemeBegin;
char *forward;
int currLine = 1;
int charsRead = 0;

void clearHeap()
{
	free(buf1);
	free(buf2);
	deleteLookupTable();
}

void getStream()
{
	// Reads stream into buf1 or buf2
	charsRead = fread(currBuffer == FIRST ? buf1 : buf2, sizeof(char), bufferSize, fp);
	if (ferror(fp)) {
		printf("Error reading from file.\n");
		exit(EXIT_FAILURE);
	}
	if (feof(fp))
		// fclose returns EOF when there is error else returns 0
		if (fclose(fp) == EOF)
			printf("Error closing file.\n");
}

void incrementForward()
{
	forward++;
	if (forward - (currBuffer == FIRST ? buf1 : buf2) == bufferSize) {
		// This toggles currBuffer between FIRST and SECOND
		currBuffer = (currBuffer + 1) & 1;
		getStream();
		forward = (currBuffer == FIRST ? buf1 : buf2);
	}
}

void handleWhitespaces()
{
	while (isblank(*forward) || *forward == '\n') {
		if (*forward == '\n')
			currLine++;
		incrementForward();
	}
	lexemeBegin = forward;
}

TokenInfo *getNextToken()
{
	// keep getting tokens till EOF
	TokenInfo *tk = (TokenInfo *)malloc(sizeof(TokenInfo));

	switch (*lexemeBegin) {
	case '+':
		tk->token = PLUS;
		incrementForward();
		break;
	case '-':
		break;
	case '*':
		break;
	case '/':
		break;
	case '<':
		break;
	case '>':
		break;
	case '=':
		break;
	case '!':
		break;
	case ':':
		break;
	case '.':
		break;
	case ';':
		break;
	case ',':
		break;
	case '[':
		break;
	case ']':
		break;
	case '(':
		break;
	case ')':
		break;
	// Handles case of identifier
	default: {
		int charsRead = 0;
		if (*lexemeBegin == '_' || isalpha(*lexemeBegin)) {
			tk->data.lexeme[charsRead] = *lexemeBegin;
			charsRead++;
			incrementForward();
			while (*forward == '_' || isalnum(*forward)) {
				tk->data.lexeme[charsRead] = *forward;
				charsRead++;
				incrementForward();
				if (charsRead > 20) {
					/* TODO confirm what action to take*/
					printf("Identifier or keyword longer than 20 chars.\n");
					exit(EXIT_FAILURE);
				}
			}

			tk->data.lexeme[charsRead] = '\0';
			KeywordPair *p = searchKeyword(tk->data.lexeme);

			tk->lineNumber = currLine;
			if (p) {
				tk->token = p->token;
				break;
			}
			tk->token = ID;
		} else if (isdigit(*lexemeBegin)) {
			/*TODO handle numbers here*/
		} else {
			printf("Invalid character in line %d\n", currLine);
			clearHeap();
			exit(EXIT_FAILURE);
		}
	}
	}

	lexemeBegin = forward;
	handleWhitespaces();
	return tk;
}

void removeComments(char *testcaseFile, char *cleanFile)
{
}
