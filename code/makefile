compileAll: lookupTable.o lookupTable.h lexer.o lexer.h parser.o parser.h
	gcc driver.c lookupTable.o lexer.o parser.o stack.o tree.o

lookupTable.o: lookupTable.c lookupTable.h
	gcc -c lookupTable.c

lexer.o: lexer.c lookupTable.h lexerDef.h
	gcc -c lexer.c

parser.o: parser.c lexer.h parserDef.h stack.h tree.h
	gcc -c parser.c

stack.o: stack.c stackDef.h treeDef.h
	gcc -c stack.c

tree.o: tree.c treeDef.h
	gcc -c tree.c
