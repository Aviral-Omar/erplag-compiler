compileAll: lookupTable.o lexer.o lexer.h parser.o parser.h ast.o ast.h symbolTable.o symbolTable.h
	gcc driver.c lookupTable.o lexer.o parser.o stack.o tree.o ast.o symbolTable.o -o stage1exe

lookupTable.o: lookupTable.c lookupTable.h
	gcc -c lookupTable.c

lexer.o: lexer.c lookupTable.h lexerDef.h
	gcc -c lexer.c

parser.o: parser.c lexer.h parserDef.h stack.o stack.h tree.o tree.h
	gcc -c parser.c

stack.o: stack.c stackDef.h treeDef.h
	gcc -c stack.c

tree.o: tree.c treeDef.h
	gcc -c tree.c

ast.o: ast.c astDef.h
	gcc -c ast.c

symbolTable.o: symbolTable.c symbolTableDef.h
	gcc -c symbolTable.c
