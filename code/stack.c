#include <stdlib.h>

#include "stackDef.h"

Stack create(void);
int isEmpty(Stack *s);
int isFull(Stack *s);
int pushLex(Stack *s, LexicalSymbol *nextSymbol);
int pushTok(Stack *s, TokenInfo *T);
int pop(Stack *s);
TokenInfo *top(Stack *s);


Stack *Create()
{
	Stack *q = (Stack *)malloc(sizeof(struct StackType));
	(q)->top = NULL;
	return q;
}



int pushLex(Stack *s, LexicalSymbol *nextSymbol)
{
	SNode *temp;
	ParseTNode *treenode;


	if (isFull(s)) return FALSE;

	temp = (SNode *)malloc(sizeof(SNode));
	temp->data.S = nextSymbol;
	temp->next = s->top;
	s->top = temp;


	return TRUE;
}

int pushTok(Stack *s, TokenInfo *T)
{
	SNode *temp;
	ParseTNode *treenode;


	if (isFull(s)) return FALSE;

	temp = (SNode *)malloc(sizeof(SNode));
	temp->data.T = T;
	temp->next = s->top;
	s->top = temp;


	return TRUE;
}


int pop(Stack *s)
{
	SNode *temp;

	if (isEmpty(s))
		return FALSE;
	else {
		temp = s->top;
		temp->Treenode->data.T = temp->data.T;
		s->top = s->top->next;
		free(temp);
	}
	return TRUE;
}

union nodeData *top(Stack *s)
{
	return s->top->data;
}


int isEmpty(Stack *s)
{
	return ((s)->top == NULL);
}

int isFull(Queue *q)
{
	return FALSE;
}
