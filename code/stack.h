#include "stackDef.h"

Stack create(void);
int isEmpty(Stack* s);
int isFull(Stack* s);
int pushLex(Stack* s, LexicalSymbol* nextSymbol);
int pushTok(Stack* s, TokenInfo* T);
int pop(Stack* s);
TokenInfo* top(Stack* s);
