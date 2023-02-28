#include <stdlib.h>

#include "stack.h"
#include "parser.h"
#include "tree.h"

// Stack *createStack(void);
// int isEmpty(Stack *s);
// int isFull(Stack *s);
// int pushTok(Stack *s, union symbol T, char type);
// int pop(Stack *s);
// union symbol top(Stack *s);


Stack *createStack()
{
	Stack *q = (Stack *)malloc(sizeof(Stack));
	q->size=0;
	q->top = NULL;
	return q;
}

SNode *pushTok(Stack *s, union symbol T, char type)
{
	SNode *temp;
	ParseTNode *treenode = NULL;
	// ParseTNode *treenode = addNode(s,T);


	if (isFull(s)) return FALSE;

	temp = (SNode *)malloc(sizeof(SNode));
	temp->data = T;
	temp->next = s->top;
	temp->treenode = treenode;
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

SNode* top(Stack *s)
{
	return s->top;
}


int isEmpty(Stack *s)
{
	return (s->size == 0);
}

int isFull(Stack *q)
{
	// TODO waiting for Stack size limit
	return FALSE;
}
