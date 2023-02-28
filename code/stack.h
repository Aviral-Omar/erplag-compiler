#include "stackDef.h"

Stack* createStack(void);
int isEmpty(Stack *s);
int isFull(Stack *s);
SNode *pushTok(Stack *s, union symbol T, char type);
int pop(Stack *s);
SNode* top(Stack *s);