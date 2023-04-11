/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef SYMBOL_TABLE_DEF
#define SYMBOL_TABLE_DEF

#define SYMBOL_TABLE_SIZE 149

#define INT_SIZE 2
#define REAL_SIZE 4
#define BOOL_SIZE 1
#define POINTER_SIZE 8

typedef struct ASTNodeStruct ASTNode;

typedef enum {
	DT_Integer,
	DT_Real,
	DT_Boolean,
	DT_Array
} DataType;

typedef struct {
	DataType arrayType;
	int isStatic;
	int lBoundSign;
	int uBoundSign;
	union {
		int numBound;
		char* idBound;
	} lowerBound;
	union {
		int numBound;
		char* idBound;
	} upperBound;
} ArrayInfo;

typedef struct {
	DataType type;
	// Can only be Int, Real or Boolean because multi dimensional arrays not supported
	ArrayInfo* arrInfo;
} TypeInfo;

typedef struct IDInfoStruct IDInfo;
struct IDInfoStruct {
	char* name;
	TypeInfo* typeInfo;
	IDInfo* next;
};

typedef struct FTEntryStruct FunctionTableEntry;
typedef struct SymbolTableStruct SymbolTable;
struct FTEntryStruct {
	char* name;
	int defnFound;
	int isDeclared;
	int isDefined;
	int isCalled;
	int isUsed;
	int size;
	int paramCount;
	IDInfo* paramList;
	int returnCount;
	IDInfo* retList;
	SymbolTable* st;
	ASTNode* moduleNode;
	// For chaining in hash table
	FunctionTableEntry* next;
};

typedef struct STEntryStruct SymbolTableEntry;
struct STEntryStruct {
	IDInfo* idInfo;
	int offset;
	int width;
	// TODO remove these 2
	int isParam;
	int isReturnVar;
	int valueAssigned;
	// For chaining in hash table
	SymbolTableEntry* next;
};

struct SymbolTableStruct {
	SymbolTableEntry* stArray[SYMBOL_TABLE_SIZE];
	SymbolTable* parentST;
	int startLine;
	int endLine;
	int nestingLevel;
};

extern FunctionTableEntry* functionTable[SYMBOL_TABLE_SIZE];
extern int semanticPrint;
#endif
