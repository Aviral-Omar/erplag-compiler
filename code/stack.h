/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include "stackDef.h"

Stack *createStack();
int isEmpty(Stack *s);
SNode *pushTok(Stack *s, Symbol T, char type);
int pop(Stack *s);
SNode *top(Stack *s);
void deleteStack(Stack *s);
