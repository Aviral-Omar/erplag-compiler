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

char *BUFEND()
{
	return (currBuffer == FIRST ? buf1 : buf2) + charsRead;
}

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

// Returns 1 if EOF is reached, else returns 0
int incrementForward()
{
	forward++;
	if ((charsRead < bufferSize) && (forward == BUFEND())) {
		return 1;  // error, EOF
	}
	if (forward - (currBuffer == FIRST ? buf1 : buf2) == bufferSize) {
		// This toggles currBuffer between FIRST and SECOND
		currBuffer = (currBuffer + 1) & 1;
		getStream();
		forward = currBuffer == FIRST ? buf1 : buf2;
	} else if (forward - (currBuffer == FIRST ? buf2 : buf1) == bufferSize) {
		// Here, we check if forward was retracted without changing current buffer
		forward = currBuffer == FIRST ? buf1 : buf2;
	}
	return 0;  // NO error
}

void retractForward()
{
	forward--;
	if (forward < (currBuffer == FIRST ? buf1 : buf2))
		forward = (currBuffer == FIRST ? buf2 : buf1) + bufferSize - 1;
}

void handleWhitespaces()
{
	while (isblank(*forward) || *forward == '\n') {
		if (*forward == '\n')
			currLine++;
		if (incrementForward())
			break;
	}
	lexemeBegin = forward;
}

int skipComment()
{
	while (!incrementForward()) {
		if (*forward == '\n')
			currLine++;
		else if (*forward == '*') {
			if (incrementForward())
				return 1;
			if (*forward == '*') {
				incrementForward();
				return 0;
			}
		}
	}
	return 1;
}

TokenInfo *getNextToken()
{
	// keep getting tokens till EOF
	TokenInfo *tk = (TokenInfo *)malloc(sizeof(TokenInfo));
	tk->lineNumber = currLine;

	switch (*lexemeBegin) {
	case '+':
		tk->token = PLUS;
		incrementForward();
		break;
	case '-':
		tk->token = MINUS;
		incrementForward();
		break;
	case '*':
		if (incrementForward() || *forward != '*')
			tk->token = MUL;
		else {
			free(tk);
			tk = NULL;
			if (skipComment()) {
				printf("Lexical error: Comment ending missing for comment starting at line %d\n", currLine);
				exit(EXIT_FAILURE);
			}
		}
		break;
	case '/':
		tk->token = DIV;
		incrementForward();
		break;
	case '<':
		if (incrementForward() || (*forward != '<' && *forward != '='))
			tk->token = LT;
		else if (*forward == '=') {
			tk->token = LE;
			incrementForward();
		} else {
			if (incrementForward() || *forward != '<')
				tk->token = DEF;
			else {
				tk->token = DRIVERDEF;
				incrementForward();
			}
		}
		break;

	case '>':
		if (incrementForward() || (*forward != '>' && *forward != '='))
			tk->token = GT;
		else if (*forward == '=') {
			tk->token = GE;
			incrementForward();
		} else {
			if (incrementForward() || *forward != '>')
				tk->token = ENDDEF;
			else {
				tk->token = DRIVERENDDEF;
				incrementForward();
			}
		}
		break;

	case '=':
		if (incrementForward() || *forward != '=') {
			printf("Lexical error in line %d\n", currLine);
			exit(EXIT_FAILURE);
		} else {
			tk->token = EQ;
			incrementForward();
		}
		break;
	case '!':
		if (incrementForward() || *forward != '=') {
			printf("Lexical error in line %d\n", currLine);
			exit(EXIT_FAILURE);
		} else {
			tk->token = NE;
			incrementForward();
		}
		break;
	case ':':
		if (incrementForward() || *forward != '=') {
			tk->token = COLON;
		} else {
			tk->token = ASSIGNOP;
			incrementForward();
		}
		break;
	case '.':
		if (incrementForward() || *forward != '.') {
			printf("Lexical error in line %d\n", currLine);
			exit(EXIT_FAILURE);
		} else {
			tk->token = RANGEOP;
			incrementForward();
		}
		break;
	case ';':
		tk->token = SEMICOL;
		incrementForward();
		break;
	case ',':
		tk->token = COMMA;
		incrementForward();
		break;
	case '[':
		tk->token = SQBO;
		incrementForward();
		break;
	case ']':
		tk->token = SQBC;
		incrementForward();
		break;
	case '(':
		tk->token = BO;
		incrementForward();
		break;
	case ')':
		tk->token = BC;
		incrementForward();
		break;

	// Handles case of identifier
	default: {
		// TODO handle error in incrementForward
		int lexLen = 0;
		if (*lexemeBegin == '_' || isalpha(*lexemeBegin)) {
			tk->data.lexeme[lexLen] = *lexemeBegin;
			lexLen++;
			incrementForward();
			while (*forward == '_' || isalnum(*forward)) {
				tk->data.lexeme[lexLen] = *forward;
				lexLen++;
				incrementForward();
				if (lexLen > 20) {
					/* TODO confirm what action to take*/
					printf("Identifier or keyword longer than 20 chars.\n");
					exit(EXIT_FAILURE);
				}
			}

			tk->data.lexeme[lexLen] = '\0';
			KeywordPair *p = searchKeyword(tk->data.lexeme);

			if (p) {
				tk->token = p->token;
				break;
			}
			tk->token = ID;
		} else if (isdigit(*lexemeBegin)) {
			/*TODO handle numbers here*/
			int lexLen = 0;
			do {
				tk->data.lexeme[lexLen] = *forward;
				lexLen++;
				if (incrementForward() == 1 || (!isdigit(*forward) && *forward != '.')) {
					tk->token = NUM;
					tk->data.lexeme[lexLen] = '\0';
					tk->data.intValue = atoi(tk->data.lexeme);
					printf("INTEGER:%d ", tk->data.intValue);
					handleWhitespaces();
					return tk;
				}
			} while (isdigit(*forward));
			// TODO check length limit of NUM
			if (incrementForward() == 1 || (!isdigit(*forward) && *forward != '.')) {
				printf("Incomplete number at line %d.\n", currLine);
				exit(EXIT_FAILURE);
			}
			if (*forward == '.') {
				retractForward();
				tk->token = NUM;
				tk->data.lexeme[lexLen] = '\0';
				tk->data.intValue = atoi(tk->data.lexeme);
				printf("INTEGER:%d ", tk->data.intValue);
			} else if (isdigit(*forward)) {
				tk->data.lexeme[lexLen] = '.';
				lexLen++;
				do {
					tk->data.lexeme[lexLen] = *forward;
					lexLen++;
					if (incrementForward() == 1 || (!isdigit(*forward) && *forward != 'e' && *forward != 'E')) {
						tk->token = RNUM;
						tk->data.lexeme[lexLen] = '\0';
						tk->data.floatValue = atof(tk->data.lexeme);
						printf("FLOAT:%f ", tk->data.floatValue);
						handleWhitespaces();
						return tk;
					}
				} while (isdigit(*forward));
				tk->data.lexeme[lexLen] = 'e';
				lexLen++;
				if (incrementForward() == 1 || (!isdigit(*forward) && *forward != '+' && *forward != '-')) {
					printf("Incomplete floating point number at line %d.\n", currLine);
					exit(EXIT_FAILURE);
				}
				do {
					tk->data.lexeme[lexLen] = *forward;
					lexLen++;
					if (incrementForward() == 1 || !isdigit(*forward)) {
						tk->token = RNUM;
						tk->data.lexeme[lexLen] = '\0';
						tk->data.floatValue = atof(tk->data.lexeme);
						printf("FLOAT:%f ", tk->data.floatValue);
						// Return is handled automatically here
					}
				} while (isdigit(*forward));
			}
		} else {
			printf("Invalid character in line %d\n", currLine);
			clearHeap();
			exit(EXIT_FAILURE);
		}
	}
	}

	handleWhitespaces();
	return tk;
}

void removeComments(char *testcaseFile, char *cleanFile)
{
	FILE *inFile, *outFile;
	char buffer[bufferSize];
	int lastAsterisk = 0;
	int insideComment = 0;

	// Open input file for reading
	inFile = fopen(testcaseFile, "r");
	if (inFile == NULL) {
		printf("Error opening input file\n");
		exit(1);
	}

	// Open output file for writing
	outFile = fopen(cleanFile, "w");
	if (outFile == NULL) {
		printf("Error opening output file\n");
		exit(1);
	}
	while (!feof(inFile)) {
		int len = fread(buffer, sizeof(char), bufferSize, inFile);
		char *p = buffer;
		while (p - buffer < len) {
			if (*p == '\n') {
				fprintf(outFile, "%c", *p);
			} else if (*p == '*') {
				if (lastAsterisk)
					insideComment = !insideComment;
				lastAsterisk = !lastAsterisk;
			} else if (!insideComment) {
				if (lastAsterisk)
					fprintf(outFile, "*");
				fprintf(outFile, "%c", *p);
			}
			p++;
		}
	}

	// Close input and output files
	fclose(inFile);
	fclose(outFile);
}
