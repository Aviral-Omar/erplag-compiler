compileAll: lookupTable.o lookupTable.h lexer.o lexer.h parser.o parser.h
	gcc driver.c lookupTable.o lexer.o parser.o

lookupTable.o: lookupTable.c lookupTable.h
	gcc -c lookupTable.c

lexer.o: lexer.c lookupTable.h lexerDef.h
	gcc -c lexer.c

parser.o: parser.c parserDef.h
	gcc -c parser.c
