#include <stdlib.h>

#include "stackDef.h"
#include "treeDef.h"


Stack *createStack();
int isEmpty(Stack *s);
int isFull(Stack *s);
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
		return FALSE;
	else {
		temp = s->top;
		temp->treenode->data = temp->data;
		s->top = s->top->next;
		free(temp);
	}
	s->size--;
	return TRUE;
}

SNode *top(Stack *s)
{
	return s->top;
}


int isEmpty(Stack *s)
{
	return (s->size == 0);
}

void deleteStack(Stack *s)
{
	SNode *cur = s->top, *next;
	do {
		next = cur->next;
		free(cur);
		cur = next;
	} while (cur);
	free(s);
}

int isFull(Stack *q)
{
	// TODO waiting for Stack size limit
	return FALSE;
}
