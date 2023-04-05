/* GROUP 51
Arnav Agarwal:			2019B2A70966P
Aviral Omar:			2019B3A70411P
Chandra Sekhar Reddy E:	2019B4A70634P
Vatsal Pattani:			2019B5A70697P
*/
#ifndef SYMBOL_TABLE_DEF
#define SYMBOL_TABLE_DEF

#define SYMBOL_TABLE_SIZE 149


typedef enum {
	DT_Integer,
	DT_Real,
	DT_Boolean,
	DT_Array
} DataType;

typedef struct {
	DataType arrayType;
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
	int isStatic;
} TypeInfo;

typedef struct IDInfoStruct IDInfo;
struct IDInfoStruct {
	char* name;
	TypeInfo* typeInfo;
	IDInfo* next;
};

typedef struct FTEntryStruct FunctionTableEntry;
struct FTEntryStruct {
	char* name;
	int declOrder;
	int isCalled;
	int isDriver;
	int offset;
	int isDefined;
	int width;
	int paramCount;
	IDInfo* paramList;
	int returnCount;
	IDInfo* retList;
	// For chaining in hash table
	FunctionTableEntry* next;
};

typedef struct STEntryStruct SymbolTableEntry;
struct STEntryStruct {
	IDInfo* idInfo;
	int offset;
	int width;
	int isParam;
	int isReturnVar;
	// For chaining in hash table
	SymbolTableEntry* next;
};

typedef struct SymbolTableStruct SymbolTable;
struct SymbolTableStruct {
	SymbolTableEntry* stArray[SYMBOL_TABLE_SIZE];
	SymbolTable* parentST;
	int isRoot;
	int size;
};

extern FunctionTableEntry* functionTable[SYMBOL_TABLE_SIZE];
;
#endif
