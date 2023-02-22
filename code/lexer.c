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

char* BUFEND(){
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

int skipComment(){
	while(forward < BUFEND()){
		if(*forward == '*'){
			incrementForward();
			if((forward < BUFEND()) && (*forward == '*')) return 1;
		}
		incrementForward();
	}
	return 0;
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
		incrementForward();
		if(*forward == '*'){
			incrementForward();
			tk->token = COMMENTMARK;
			if(skipComment()){
				tk->token = COMMENTMARK;
				incrementForward();
			}
			else{
				printf("Lexical error: comment ending missing for comment starting at line %d\n",currLine);
				exit(EXIT_FAILURE);
			}
		}
		else tk->token = MUL;
		break;

	case '/':
		tk->token = DIV;
		incrementForward();
		break;
	case '<':
		incrementForward();
		if(*forward == '<'){
			incrementForward();
			if(*forward == '<'){
				incrementForward();
				tk->token = DRIVERDEF;
			}
			else tk->token = DEF;
		}
		else if(*forward == '='){
			incrementForward();
			tk->token = LE;
		}
		else tk->token = LT;
		break;

	case '>':
		incrementForward();
		if(*forward == '>'){
			incrementForward();
			if(*forward == '>'){
				incrementForward();
				tk->token = DRIVERENDDEF;
			}
			else tk->token = DRIVERDEF;
		}
		else if (*forward == '=') {
			incrementForward();
			tk->token = GE;
		}
		else tk->token = GT;
		break;

	case '=':
		incrementForward();
		if(*forward == '='){
			incrementForward();
			tk->token = EQ;
		}
		else{
			printf("Lexical error in line %d\n",currLine);
			exit(EXIT_FAILURE);
		}
		break;
	case '!':
		incrementForward();
		if(*forward == '='){
			incrementForward();
			tk->token = NE;
		}
		else{
			printf("Lexical error in line %d\n",currLine);
			exit(EXIT_FAILURE);
		}
		break;
	case ':':
		incrementForward();
		if(*forward == '='){
			incrementForward();
			tk->token = ASSIGNOP;
		}
		else{
			tk->token = COLON;
		}
		break;

	case '.':
		incrementForward();
		if(*forward == '.'){
			incrementForward();
			tk->token = RANGEOP;
		}
		else{
			printf("Lexical error in line %d\n",currLine);
			exit(EXIT_FAILURE);
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

			if (p) {
				tk->token = p->token;
				break;
			}
			tk->token = ID;
		} else if (isdigit(*lexemeBegin)) {
			/*TODO handle numbers here*/
			incrementForward();
			while(isdigit(*forward)){
				incrementForward();
			}
			//TODO check length limit of NUM
			tk->token = NUM;
			// tk->intValue = par

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

	/* TODO remove
	// Read input file line by line
	while (fgets(buffer, sizeof(buffer), inFile) != NULL) {
		int i, len = strlen(buffer);
		char newl = '\n';
		// Check for start of comment
		for (i = 0; i < len - 1; i++) {
			if (buffer[i] == '*' && buffer[i + 1] == '*') {
				insideComment = 1;
				fprintf(outFile, "%c", newl);
				break;
			}
		}

		// Write non-comment lines to output file
		if (!insideComment) {
			fprintf(outFile, "%s", buffer);
		}

		// Check for end of comment
		for (i = 0; i < len - 1; i++) {
			if (buffer[i] == '*' && buffer[i + 1] == '*' && insideComment) {
				insideComment = 0;
				break;
			}
		}

		// Increment line number
		lineNum++;
	}
	*/

	// Close input and output files
	fclose(inFile);
	fclose(outFile);
}
