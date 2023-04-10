#ifndef SEM_ANALYSER_DEF
#define SEM_ANALYSER_DEF

#include "symbolTableDef.h"

typedef struct WhileLLStruct WhileLL;
struct WhileLLStruct {
	SymbolTableEntry* stEntry;
	WhileLL* next;
};

#endif
