compileAll: lookupTable.o lexer.o lexer.h
	gcc driver.c lookupTable.o lexer.o

lookupTable.o: lookupTable.c lookupTable.h
	gcc -c lookupTable.c

lexer.o: lexer.c lookupTable.h lexerDef.h
	gcc -c lexer.c
