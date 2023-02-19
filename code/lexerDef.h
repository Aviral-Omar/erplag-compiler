typedef enum {
	INTEGER,
	REAL,
	BOOLEAN,
	OF,
	ARRAY,
	START,
	END,
	DECLARE,
	MODULE,
	DRIVER,
	PROGRAM,
	GET_VALUE,
	PRINT,
	USE,
	WITH,
	PARAMETERS,
	TAKES,
	INPUT,
	RETURNS,
	FOR,
	IN,
	SWITCH,
	CASE,
	BREAK,
	DEFAULT,
	WHILE,
	ID,
	NUM,
	RNUM,
	AND,
	OR,
	TRUE,
	FALSE,
	PLUS,
	MINUS,
	MUL,
	DIV,
	LT,
	LE,
	GE,
	GT,
	EQ,
	NE,
	DEF,
	ENDDEF,
	DRIVERDEF,
	DRIVERENDDEF,
	COLON,
	RANGEOP,
	SEMICOL,
	COMMA,
	ASSIGNOP,
	SQBO,
	SQBC,
	BO,
	BC,
	COMMENTMARK
} Token;

typedef struct {
	Token token;
	unsigned int lineNumber;
	union lexemeData {
		char lexeme[20];
		int intValue;
		float floatValue;
	} data;
} TokenInfo;
