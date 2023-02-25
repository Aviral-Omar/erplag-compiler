#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "lookupTable.h"

void initLexer();
void clearHeap();
void getStream();
int incrementForward();
void retractForward();
void handleWhitespaces();
int skipComment();
char *BUFEND();
void handleLexicalError(char *errorMsg, int lineNumber);
void getNextToken();
void removeComments(char *testcaseFile, char *cleanFile);

FILE *fp;
int bufferSize;
char *buf1;
char *buf2;
CurrentBuffer currBuffer = FIRST;
char *lexemeBegin;
char *forward;
int currLine = 1;
int charsRead = 0;
TokenInfo *currToken;
char *tokenMap[58] = {"INTEGER",
					  "REAL",
					  "BOOLEAN",
					  "OF",
					  "ARRAY",
					  "START",
					  "END",
					  "DECLARE",
					  "MODULE",
					  "DRIVER",
					  "PROGRAM",
					  "GET_VALUE",
					  "PRINT",
					  "USE",
					  "WITH",
					  "PARAMETERS",
					  "TAKES",
					  "INPUT",
					  "RETURNS",
					  "FOR",
					  "IN",
					  "SWITCH",
					  "CASE",
					  "BREAK",
					  "DEFAULT",
					  "WHILE",
					  "ID",
					  "NUM",
					  "RNUM",
					  "AND",
					  "OR",
					  "TRUE",
					  "FALSE",
					  "PLUS",
					  "MINUS",
					  "MUL",
					  "DIV",
					  "LT",
					  "LE",
					  "GE",
					  "GT",
					  "EQ",
					  "NE",
					  "DEF",
					  "ENDDEF",
					  "DRIVERDEF",
					  "DRIVERENDDEF",
					  "COLON",
					  "RANGEOP",
					  "SEMICOL",
					  "COMMA",
					  "ASSIGNOP",
					  "SQBO",
					  "SQBC",
					  "BO",
					  "BC",
					  "COMMENTMARK",
					  "EPSILON"};

void initLexer()
{
	buf1 = (char *)malloc(bufferSize * sizeof(char));
	buf2 = (char *)malloc(bufferSize * sizeof(char));
	lexemeBegin = (currBuffer == FIRST ? buf1 : buf2);
	forward = lexemeBegin;

	populateLookupTable();
	// First read is unconditional
	getStream();
	// To remove initial whitespaces
	handleWhitespaces();
}

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
		fprintf(stderr, "Error reading from file.\n");
		exit(EXIT_FAILURE);
	}
	if (feof(fp))
		// fclose returns EOF when there is error else returns 0
		if (fclose(fp) == EOF)
			fprintf(stderr, "Error closing file.\n");
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

void handleLexicalError(char *errorMsg, int lineNumber)
{
	fprintf(stderr, errorMsg, lineNumber);
	free(currToken);
	currToken = NULL;
}

void getNextToken()
{
	/* TODO handle ID, NUM, RNUM max lengths*/
	/* TODO remove isdigit, isalnum, isalpha calls*/
	// keep getting tokens till EOF
	currToken = (TokenInfo *)malloc(sizeof(TokenInfo));
	currToken->lineNumber = currLine;

	switch (*lexemeBegin) {
	case '+':
		currToken->token = PLUS;
		incrementForward();
		break;
	case '-':
		currToken->token = MINUS;
		incrementForward();
		break;
	case '*':
		if (incrementForward() || *forward != '*')
			currToken->token = MUL;
		else {
			int startLine = currLine;
			if (skipComment()) {
				handleLexicalError("Lexical Error: Comment ending missing for comment starting at line %d.\n", startLine);
			} else {
				free(currToken);
				currToken = NULL;
			}
		}
		break;
	case '/':
		currToken->token = DIV;
		incrementForward();
		break;
	case '<':
		if (incrementForward() || (*forward != '<' && *forward != '='))
			currToken->token = LT;
		else if (*forward == '=') {
			currToken->token = LE;
			incrementForward();
		} else {
			if (incrementForward() || *forward != '<')
				currToken->token = DEF;
			else {
				currToken->token = DRIVERDEF;
				incrementForward();
			}
		}
		break;

	case '>':
		if (incrementForward() || (*forward != '>' && *forward != '='))
			currToken->token = GT;
		else if (*forward == '=') {
			currToken->token = GE;
			incrementForward();
		} else {
			if (incrementForward() || *forward != '>')
				currToken->token = ENDDEF;
			else {
				currToken->token = DRIVERENDDEF;
				incrementForward();
			}
		}
		break;

	case '=':
		if (incrementForward() || *forward != '=') {
			handleLexicalError("Lexical Error: Single '=' at line %d.\n", currLine);
		} else {
			currToken->token = EQ;
			incrementForward();
		}
		break;
	case '!':
		if (incrementForward() || *forward != '=') {
			handleLexicalError("Lexical Error: Single '!' at line %d.\n", currLine);
		} else {
			currToken->token = NE;
			incrementForward();
		}
		break;
	case ':':
		if (incrementForward() || *forward != '=') {
			currToken->token = COLON;
		} else {
			currToken->token = ASSIGNOP;
			incrementForward();
		}
		break;
	case '.':
		if (incrementForward() || *forward != '.') {
			handleLexicalError("Lexical Error: Single '.' at line %d.\n", currLine);
		} else {
			currToken->token = RANGEOP;
			incrementForward();
		}
		break;
	case ';':
		currToken->token = SEMICOL;
		incrementForward();
		break;
	case ',':
		currToken->token = COMMA;
		incrementForward();
		break;
	case '[':
		currToken->token = SQBO;
		incrementForward();
		break;
	case ']':
		currToken->token = SQBC;
		incrementForward();
		break;
	case '(':
		currToken->token = BO;
		incrementForward();
		break;
	case ')':
		currToken->token = BC;
		incrementForward();
		break;

	// Handles case of identifier
	default: {
		int lexLen = 0;
		if (*lexemeBegin == '_' || isalpha(*lexemeBegin)) {
			currToken->data.lexeme[lexLen] = *lexemeBegin;
			lexLen++;
			while (!incrementForward() && (*forward == '_' || isalnum(*forward))) {
				currToken->data.lexeme[lexLen] = *forward;
				lexLen++;
				if (lexLen > 20) {
					/* TODO confirm what action to take*/
					handleLexicalError("Lexical Error: Identifier or keyword longer than 20 chars on line %d.\n", currLine);
					while (!incrementForward() && (*forward == '_' || isalnum(*forward)))
						;
					return;
				}
			}

			currToken->data.lexeme[lexLen] = '\0';
			KeywordPair *p = searchKeyword(currToken->data.lexeme);

			currToken->token = p ? p->token : ID;

		} else if (isdigit(*lexemeBegin)) {
			int lexLen = 0;
			do {
				currToken->data.lexeme[lexLen] = *forward;
				lexLen++;
				if (incrementForward() || (!isdigit(*forward) && *forward != '.')) {
					currToken->token = NUM;
					currToken->data.lexeme[lexLen] = '\0';
					currToken->data.intValue = atoi(currToken->data.lexeme);
					return;
				}
			} while (isdigit(*forward));
			// TODO check length limit of NUM
			if (incrementForward() || (!isdigit(*forward) && *forward != '.')) {
				handleLexicalError("Lexical Error: Incomplete number at line %d.\n", currLine);
			} else if (*forward == '.') {
				retractForward();
				currToken->token = NUM;
				currToken->data.lexeme[lexLen] = '\0';
				currToken->data.intValue = atoi(currToken->data.lexeme);
			} else if (isdigit(*forward)) {
				currToken->data.lexeme[lexLen] = '.';
				lexLen++;
				do {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					if (incrementForward() || (!isdigit(*forward) && *forward != 'e' && *forward != 'E')) {
						currToken->token = RNUM;
						currToken->data.lexeme[lexLen] = '\0';
						currToken->data.floatValue = atof(currToken->data.lexeme);
						return;
					}
				} while (isdigit(*forward));
				currToken->data.lexeme[lexLen] = 'e';
				lexLen++;
				if (incrementForward() || (!isdigit(*forward) && *forward != '+' && *forward != '-')) {
					handleLexicalError("Lexical Error: Incomplete floating point number at line %d.\n", currLine);
					return;
				}
				if (*forward == '+' || *forward == '-') {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					if (incrementForward() || !isdigit(*forward)) {
						handleLexicalError("Lexical Error: Incomplete floating point number at line %d.\n", currLine);
						return;
					}
				}

				do {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					if (incrementForward() || !isdigit(*forward)) {
						currToken->token = RNUM;
						currToken->data.lexeme[lexLen] = '\0';
						currToken->data.floatValue = atof(currToken->data.lexeme);
						// Return is handled automatically here
					}
				} while (isdigit(*forward));
			}
		} else {
			handleLexicalError("Lexical Error: Invalid character at line %d\n", currLine);
			incrementForward();
		}
	}
	}
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
		fprintf(stderr, "Error opening input file\n");
		exit(1);
	}

	// Open output file for writing
	outFile = fopen(cleanFile, "w");
	if (outFile == NULL) {
		fprintf(stderr, "Error opening output file\n");
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
				if (lastAsterisk) {
					fprintf(outFile, "*");
					lastAsterisk = !lastAsterisk;
				}
				fprintf(outFile, "%c", *p);
			}
			p++;
		}
	}

	// Close input and output files
	fclose(inFile);
	fclose(outFile);
}
