#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "lookupTable.h"

#define isAlpha(x) ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
#define isDigit(x) (x >= '0' && x <= '9')
#define isAlnum(x) (((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z')) || (x >= '0' && x <= '9'))

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
int syntaxCorrect = 1;
TokenInfo *currToken;
char *tokenMap[59] = {"INTEGER",
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
					  "EPSILON",
					  "DOLLAR",
					  "COMMENTMARK"};

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
	while (*forward == ' ' || *forward == '\t' || *forward == '\n') {
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
	syntaxCorrect = 0;
	printf(errorMsg, lineNumber);
	free(currToken);
	currToken = NULL;
}

void handleNumberLengthWarning(char *errorMsg)
{
	printf(errorMsg);
}

void getNextToken()
{
	/* TODO handle ID, NUM, RNUM max lengths*/
	// keep getting tokens till EOF
	currToken = (TokenInfo *)malloc(sizeof(TokenInfo));
	currToken->lineNumber = currLine;

	switch (*lexemeBegin) {
	case '+':
		currToken->token = PLUS;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case '-':
		currToken->token = MINUS;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case '*':
		if (incrementForward() || *forward != '*') {
			printf("%d\t*\t", currLine);
			currToken->token = MUL;
		} else {
			int startLine = currLine;
			printf("%d\t**\t\tCOMMENT\n", currLine);
			if (skipComment()) {
				handleLexicalError("\tLexical Error: Comment ending missing for comment starting at line %d.\n", startLine);
			} else {
				free(currToken);
				currToken = NULL;
			}
		}
		break;
	case '/':
		currToken->token = DIV;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case '<':
		printf("%d\t%c", currLine, *forward);
		if (incrementForward() || (*forward != '<' && *forward != '='))
			currToken->token = LT;
		else if (*forward == '=') {
			currToken->token = LE;
			printf("%c", *forward);
			incrementForward();
		} else {
			printf("%c", *forward);
			if (incrementForward() || *forward != '<')
				currToken->token = DEF;
			else {
				currToken->token = DRIVERDEF;
				printf("%c", *forward);
				incrementForward();
			}
		}
		printf("\t");
		break;

	case '>':
		printf("%d\t%c", currLine, *forward);
		if (incrementForward() || (*forward != '>' && *forward != '='))
			currToken->token = GT;
		else if (*forward == '=') {
			currToken->token = GE;
			printf("%c", *forward);
			incrementForward();
		} else {
			printf("%c", *forward);
			if (incrementForward() || *forward != '>')
				currToken->token = ENDDEF;
			else {
				currToken->token = DRIVERENDDEF;
				printf("%c", *forward);
				incrementForward();
			}
		}
		printf("\t");
		break;

	case '=':
		printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '=') {
			printf("\t");
			handleLexicalError("\tLexical Error: Single '=' at line %d.\n", currLine);
		} else {
			currToken->token = EQ;
			printf("%c\t", *forward);
			incrementForward();
		}
		break;
	case '!':
		printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '=') {
			printf("\t");
			handleLexicalError("\tLexical Error: Single '!' at line %d.\n", currLine);
		} else {
			currToken->token = NE;
			printf("%c\t", *forward);
			incrementForward();
		}
		break;
	case ':':
		printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '=') {
			currToken->token = COLON;
		} else {
			currToken->token = ASSIGNOP;
			printf("%c", *forward);
			incrementForward();
		}
		printf("\t");
		break;
	case '.':
		printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '.') {
			printf("\t");
			handleLexicalError("\tLexical Error: Single '.' at line %d.\n", currLine);
		} else {
			currToken->token = RANGEOP;
			printf("%c\t", *forward);
			incrementForward();
		}
		break;
	case ';':
		currToken->token = SEMICOL;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case ',':
		currToken->token = COMMA;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case '[':
		currToken->token = SQBO;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case ']':
		currToken->token = SQBC;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case '(':
		currToken->token = BO;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;
	case ')':
		currToken->token = BC;
		printf("%d\t%c\t", currLine, *forward);
		incrementForward();
		break;

	default: {
		int lexLen = 0;
		if (*lexemeBegin == '_' || isAlpha(*lexemeBegin)) {
			printf("%d\t%c", currLine, *forward);
			currToken->data.lexeme[lexLen] = *lexemeBegin;
			lexLen++;
			while (!incrementForward() && (*forward == '_' || isAlnum(*forward))) {
				printf("%c", *forward);
				currToken->data.lexeme[lexLen] = *forward;
				lexLen++;
				if (lexLen > 20) {
					while (!incrementForward() && (*forward == '_' || isAlnum(*forward)))
						printf("%c", *forward);
					printf("\t");
					handleLexicalError("Lexical Error: Identifier or keyword longer than 20 chars on line %d.\n", currLine);
					return;
				}
			}

			printf("\t");
			currToken->data.lexeme[lexLen] = '\0';
			KeywordPair *p = searchKeyword(currToken->data.lexeme);

			currToken->token = p ? p->token : ID;

		} else if (isDigit(*lexemeBegin)) {
			int lexLen = 0;
			printf("%d\t", currLine);
			do {
				currToken->data.lexeme[lexLen] = *forward;
				lexLen++;
				printf("%c", *forward);
				if (incrementForward() || (!isDigit(*forward) && *forward != '.')) {
					currToken->token = NUM;
					currToken->data.lexeme[lexLen] = '\0';
					currToken->data.intValue = atoi(currToken->data.lexeme);

					printf("\t");
					if (lexLen > 9) {
						handleNumberLengthWarning("Warning: Integer is too big, there is a possibility that it may be misrepresented.");
					}
					return;
				}
			} while (isDigit(*forward));
			// Checking length of Integer, throw error if length >10


			if (incrementForward() || (!isDigit(*forward) && *forward != '.')) {
				printf(".\t\t");
				handleLexicalError("Lexical Error: Incomplete number at line %d.\n", currLine);
			} else if (*forward == '.') {
				printf("\t");
				if (lexLen > 9) {
					handleNumberLengthWarning("Warning: Integer is too big, there is a possibility that it may be misrepresented.");
				}
				retractForward();
				currToken->token = NUM;
				currToken->data.lexeme[lexLen] = '\0';
				currToken->data.intValue = atoi(currToken->data.lexeme);
			} else if (isDigit(*forward)) {
				printf(".");
				currToken->data.lexeme[lexLen] = '.';
				lexLen++;
				int decimalpartLen = 0;
				int exponentpartLen = 0;
				do {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					decimalpartLen++;
					printf("%c", *forward);
					if (incrementForward() || (!isDigit(*forward) && *forward != 'e' && *forward != 'E')) {
						currToken->token = RNUM;
						currToken->data.lexeme[lexLen] = '\0';
						currToken->data.floatValue = atof(currToken->data.lexeme);
						printf("\t");
						if (decimalpartLen > 11 && lexLen > 21) {
							handleNumberLengthWarning("Warning: Decimal part of float is too big, there is a possibility that it may be misrepresented.");
						}
						return;
					}
				} while (isDigit(*forward));
				currToken->data.lexeme[lexLen] = 'e';
				lexLen++;
				printf("%c", *forward);
				if (incrementForward() || (!isDigit(*forward) && *forward != '+' && *forward != '-')) {
					printf("\t\t");
					handleLexicalError("Lexical Error: Incomplete floating point number at line %d.\n", currLine);
					return;
				}
				if (*forward == '+' || *forward == '-') {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					printf("%c", *forward);
					if (incrementForward() || !isDigit(*forward)) {
						printf("\t\t");
						handleLexicalError("Lexical Error: Incomplete floating point number at line %d.\n", currLine);
						return;
					}
				}

				do {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					exponentpartLen++;
					printf("%c", *forward);
					if (incrementForward() || !isDigit(*forward)) {
						currToken->token = RNUM;
						currToken->data.lexeme[lexLen] = '\0';
						currToken->data.floatValue = atof(currToken->data.lexeme);
						if (exponentpartLen > 11 && lexLen > 21) {
							printf("\t");
							handleNumberLengthWarning("Warning: Exponent is too big, there is a possibility that it may be misrepresented.");
						}
						// Return is handled automatically here
					}
				} while (isDigit(*forward));
				printf("\t");
			}
		} else {
			printf("%c\t", *forward);
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
