/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"
#include "lookupTable.h"

#define isAlpha(x) ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
#define isDigit(x) (x >= '0' && x <= '9')
#define isAlnum(x) (((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z')) || (x >= '0' && x <= '9'))

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
char *lexemeMap[59] = {"integer",
					   "real",
					   "boolean",
					   "of",
					   "array",
					   "start",
					   "end",
					   "declare",
					   "module",
					   "driver",
					   "program",
					   "get_value",
					   "print",
					   "use",
					   "with",
					   "parameters",
					   "takes",
					   "input",
					   "returns",
					   "for",
					   "in",
					   "switch",
					   "case",
					   "break",
					   "default",
					   "while",
					   "",
					   "",
					   "",
					   "AND",
					   "OR",
					   "true",
					   "false",
					   "+",
					   "-",
					   "*",
					   "/",
					   "<",
					   "<=",
					   ">=",
					   ">",
					   "==",
					   "!=",
					   "<<",
					   ">>",
					   "<<<",
					   ">>>",
					   ":",
					   "..",
					   ";",
					   ",",
					   ":=",
					   "[",
					   "]",
					   "(",
					   ")",
					   "epsilon",
					   "$",
					   "**"};
FILE *src;
int bufferSize;
char *buf1;
char *buf2;
char *lexemeBegin;
char *forward;
CurrentBuffer currBuffer;
int currLine;
int charsRead;
int lexerCorrect;
int lexerPrint = 0;
TokenInfo *currToken;

void runOnlyLexer(char *filename);
void initLexer();
void clearLexerData();
void getStream();
int incrementForward();
void retractForward();
void handleWhitespaces();
int skipComment();
char *BUFEND();
void handleLexicalError(char *errorMsg, int lineNumber);
void handleNumberLengthWarning(char *errorMsg);
void getNextToken();
void removeComments(char *testcaseFile, char *cleanFile);

void runOnlyLexer(char *filename)
{
	// Checks if file path is valid and opens file
	src = fopen(filename, "r");
	if (!src) {
		printf("File does not exist or unable to access file.\n");
		exit(EXIT_FAILURE);
	}
	initLexer();
	lexerPrint = 1;
	printf("Line Number\tLexeme\t\t\tToken Name\n");
	while (charsRead == bufferSize || lexemeBegin < BUFEND()) {
		getNextToken();
		handleWhitespaces();
		if (!currToken)
			continue;

		free(currToken);
	}
	if (lexerCorrect)
		printf("\nInput source code is lexically correct.\n\n");
	clearLexerData();
}

void initLexer()
{
	currBuffer = FIRST;
	currLine = 1;
	charsRead = 0;
	lexerCorrect = 1;
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

void clearLexerData()
{
	free(buf1);
	free(buf2);
	deleteLookupTable();
}

void getStream()
{
	// Reads stream into buf1 or buf2
	charsRead = fread(currBuffer == FIRST ? buf1 : buf2, sizeof(char), bufferSize, src);

	if (ferror(src)) {
		printf("Error reading from file.\n");
		exit(EXIT_FAILURE);
	}
	if (feof(src))
		if (fclose(src) == EOF)
			printf("Error closing file.\n");

	// fclose returns EOF when there is error else returns 0
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
	lexerCorrect = 0;
	printf(errorMsg, lineNumber);
	free(currToken);
	currToken = NULL;
}

void handleNumberLengthWarning(char *errorMsg)
{
	printf(errorMsg, currLine);
}


void getNextToken()
{
	// Keep getting tokens till EOF
	currToken = (TokenInfo *)malloc(sizeof(TokenInfo));
	currToken->lineNumber = currLine;

	switch (*lexemeBegin) {
	case '+':
		currToken->token = PLUS;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case '-':
		currToken->token = MINUS;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case '*':
		if (incrementForward() || *forward != '*') {
			currToken->token = MUL;
			if (lexerPrint) printf("%d\t*\t\t%-12s\n", currLine, tokenMap[currToken->token]);
		} else {
			int startLine = currLine;
			if (lexerPrint) printf("%d\t**\t\tCOMMENT\n", currLine);
			if (skipComment()) {
				handleLexicalError("Lexical Error: Comment ending missing for comment starting: Line %d\n", startLine);
			} else {
				free(currToken);
				currToken = NULL;
			}
		}
		break;
	case '/':
		currToken->token = DIV;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case '<':
		if (lexerPrint) printf("%d\t%c", currLine, *forward);
		if (incrementForward() || (*forward != '<' && *forward != '='))
			currToken->token = LT;
		else if (*forward == '=') {
			currToken->token = LE;
			if (lexerPrint) printf("%c", *forward);
			incrementForward();
		} else {
			if (lexerPrint) printf("%c", *forward);
			if (incrementForward() || *forward != '<')
				currToken->token = DEF;
			else {
				currToken->token = DRIVERDEF;
				if (lexerPrint) printf("%c", *forward);
				incrementForward();
			}
		}
		if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
		break;

	case '>':
		if (lexerPrint) printf("%d\t%c", currLine, *forward);
		if (incrementForward() || (*forward != '>' && *forward != '='))
			currToken->token = GT;
		else if (*forward == '=') {
			currToken->token = GE;
			if (lexerPrint) printf("%c", *forward);
			incrementForward();
		} else {
			if (lexerPrint) printf("%c", *forward);
			if (incrementForward() || *forward != '>')
				currToken->token = ENDDEF;
			else {
				currToken->token = DRIVERENDDEF;
				if (lexerPrint) printf("%c", *forward);
				incrementForward();
			}
		}
		if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
		break;

	case '=':
		if (lexerPrint) printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '=') {
			if (lexerPrint) printf("\n");
			handleLexicalError("Lexical Error: Single '=': Line %d\n", currLine);
		} else {
			currToken->token = EQ;
			if (lexerPrint) printf("%c\t\t%-12s\n", *forward, tokenMap[currToken->token]);
			incrementForward();
		}
		break;
	case '!':
		if (lexerPrint) printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '=') {
			if (lexerPrint) printf("\n");
			handleLexicalError("Lexical Error: Single '!': Line %d\n", currLine);
		} else {
			currToken->token = NE;
			if (lexerPrint) printf("%c\t\t%-12s\n", *forward, tokenMap[currToken->token]);
			incrementForward();
		}
		break;
	case ':':
		if (lexerPrint) printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '=') {
			currToken->token = COLON;
		} else {
			currToken->token = ASSIGNOP;
			if (lexerPrint) printf("%c", *forward);
			incrementForward();
		}
		if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
		break;
	case '.':
		if (lexerPrint) printf("%d\t%c", currLine, *forward);
		if (incrementForward() || *forward != '.') {
			if (lexerPrint) printf("\n");
			handleLexicalError("Lexical Error: Single '.': Line %d\n", currLine);
		} else {
			currToken->token = RANGEOP;
			if (lexerPrint) printf("%c\t\t%-12s\n", *forward, tokenMap[currToken->token]);
			incrementForward();
		}
		break;
	case ';':
		currToken->token = SEMICOL;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case ',':
		currToken->token = COMMA;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case '[':
		currToken->token = SQBO;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case ']':
		currToken->token = SQBC;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case '(':
		currToken->token = BO;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;
	case ')':
		currToken->token = BC;
		if (lexerPrint) printf("%d\t%c\t\t%-12s\n", currLine, *forward, tokenMap[currToken->token]);
		incrementForward();
		break;

	default: {
		int lexLen = 0;
		if (*lexemeBegin == '_' || isAlpha(*lexemeBegin)) {
			if (lexerPrint) printf("%d\t%c", currLine, *forward);
			currToken->data.lexeme[lexLen] = *lexemeBegin;
			lexLen++;
			while (!incrementForward() && (*forward == '_' || isAlnum(*forward))) {
				if (lexerPrint) printf("%c", *forward);
				currToken->data.lexeme[lexLen] = *forward;
				lexLen++;
				if (lexLen > 20) {
					while (!incrementForward() && (*forward == '_' || isAlnum(*forward)))
						if (lexerPrint) printf("%c", *forward);
					if (lexerPrint) printf("\n");
					handleLexicalError("Lexical Error: Identifier or keyword longer than 20 chars: Line %d\n", currLine);
					return;
				}
			}

			currToken->data.lexeme[lexLen] = '\0';
			KeywordPair *p = searchKeyword(currToken->data.lexeme);

			currToken->token = p ? p->token : ID;
			if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);

		} else if (isDigit(*lexemeBegin)) {
			int lexLen = 0;
			if (lexerPrint) printf("%d\t", currLine);
			do {
				currToken->data.lexeme[lexLen] = *forward;
				lexLen++;
				if (lexerPrint) printf("%c", *forward);
				if (incrementForward() || (!isDigit(*forward) && *forward != '.')) {
					currToken->token = NUM;
					currToken->data.lexeme[lexLen] = '\0';
					// currToken->data.intValue = atoi(currToken->data.lexeme);

					if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
					if (lexLen > 9) {
						handleNumberLengthWarning("Warning: Integer is too big, there is a possibility that it may be misrepresented: Line %d\n");
					}
					return;
				}
			} while (isDigit(*forward));
			// Checking length of Integer, throw error if length >10


			if (incrementForward() || (!isDigit(*forward) && *forward != '.')) {
				if (lexerPrint) printf(".\n");
				handleLexicalError("Lexical Error: Incomplete number: Line %d\n", currLine);
			} else if (*forward == '.') {
				retractForward();
				currToken->token = NUM;
				currToken->data.lexeme[lexLen] = '\0';
				// currToken->data.intValue = atoi(currToken->data.lexeme);
				if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
				if (lexLen > 9) {
					handleNumberLengthWarning("Warning: Integer is too big, there is a possibility that it may be misrepresented: Line %d\n");
				}
			} else if (isDigit(*forward)) {
				if (lexerPrint) printf(".");
				currToken->data.lexeme[lexLen] = '.';
				lexLen++;
				int decimalpartLen = 0;
				int exponentpartLen = 0;
				do {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					decimalpartLen++;
					if (lexerPrint) printf("%c", *forward);
					if (incrementForward() || (!isDigit(*forward) && *forward != 'e' && *forward != 'E')) {
						currToken->token = RNUM;
						currToken->data.lexeme[lexLen] = '\0';
						// currToken->data.floatValue = atof(currToken->data.lexeme);
						if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
						if (decimalpartLen > 11 && lexLen > 21) {
							handleNumberLengthWarning("Warning: Decimal part of float is too big, there is a possibility that it may be misrepresented: Line %d\n");
						}
						return;
					}
				} while (isDigit(*forward));
				currToken->data.lexeme[lexLen] = 'e';
				lexLen++;
				if (lexerPrint) printf("%c", *forward);
				if (incrementForward() || (!isDigit(*forward) && *forward != '+' && *forward != '-')) {
					if (lexerPrint) printf("\n");
					handleLexicalError("Lexical Error: Incomplete floating point number: Line %d\n", currLine);
					return;
				}
				if (*forward == '+' || *forward == '-') {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					if (lexerPrint) printf("%c", *forward);
					if (incrementForward() || !isDigit(*forward)) {
						if (lexerPrint) printf("\n");
						handleLexicalError("Lexical Error: Incomplete floating point number: Line %d\n", currLine);
						return;
					}
				}

				do {
					currToken->data.lexeme[lexLen] = *forward;
					lexLen++;
					exponentpartLen++;
					if (lexerPrint) printf("%c", *forward);
					if (incrementForward() || !isDigit(*forward)) {
						currToken->token = RNUM;
						currToken->data.lexeme[lexLen] = '\0';
						currToken->data.floatValue = atof(currToken->data.lexeme);
						if (lexerPrint) printf("\t\t%-12s\n", tokenMap[currToken->token]);
						if (exponentpartLen > 11 && lexLen > 21) {
							handleNumberLengthWarning("Warning: Exponent is too big, there is a possibility that it may be misrepresented: Line %d\n");
						}
						return;
					}
				} while (isDigit(*forward));
			}
		} else {
			if (lexerPrint) printf("%d\t%c\n", currLine, *forward);
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
		printf("Error opening output file.\n");
		exit(EXIT_FAILURE);
	}
	while (!feof(inFile)) {
		int len = fread(buffer, sizeof(char), bufferSize, inFile);
		char *p = buffer;
		while (p - buffer < len) {
			if (*p == '\n') {
				fprintf(outFile, "%c", *p);
				printf("%c", *p);
			} else if (*p == '*') {
				if (lastAsterisk)
					insideComment = !insideComment;
				lastAsterisk = !lastAsterisk;
			} else if (!insideComment) {
				if (lastAsterisk) {
					fprintf(outFile, "*");
					printf("*");
					lastAsterisk = !lastAsterisk;
				}
				fprintf(outFile, "%c", *p);
				printf("%c", *p);
			}
			p++;
		}
	}

	// Close input and output files
	fclose(inFile);
	fclose(outFile);
}
