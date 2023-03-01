#include "stackDef.h"

Stack *createStack();
int isEmpty(Stack *s);
SNode *pushTok(Stack *s, Symbol T, char type);
int pop(Stack *s);
SNode *top(Stack *s);
void deleteStack(Stack *s);
