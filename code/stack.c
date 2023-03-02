#include <stdlib.h>

#include "stackDef.h"
#include "treeDef.h"


Stack *createStack();
int isEmpty(Stack *s);
SNode *pushTok(Stack *s, Symbol T, char type);
int pop(Stack *s);
SNode *top(Stack *s);
void deleteStack(Stack *s);


Stack *createStack()
{
	Stack *st = (Stack *)malloc(sizeof(Stack));
	st->size = 0;
	st->top = NULL;
	return st;
}

SNode *pushTok(Stack *s, Symbol T, char type)
{
	SNode *temp = (SNode *)malloc(sizeof(SNode));

	temp->type = type;
	temp->data = T;
	temp->next = s->top;
	temp->treenode = NULL;
	s->top = temp;
	s->size++;

	return temp;
}


int pop(Stack *s)
{
	SNode *temp;

	if (isEmpty(s))
		return 1;
	else {
		temp = s->top;
		s->top = temp->next;
		free(temp);
	}
	s->size--;
	return 0;
}

SNode *top(Stack *s)
{
	return s->top;
}


int isEmpty(Stack *s)
{
	return !s->size;
}

void deleteStack(Stack *s)
{
	while (!isEmpty(s))
		pop(s);
	free(s);
}
