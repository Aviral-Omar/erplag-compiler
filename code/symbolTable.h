/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#include "symbolTableDef.h"

TypeInfo* createTypeInfo(ASTNode* node);

FunctionTableEntry* findFunctionEntry(char* name);

SymbolTableEntry* findSymbolEntry(char* name, SymbolTable* st);

void createSymbolTables();

void printSymbolTable();

void printActivationRecords();

void printArrays();
