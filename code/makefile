compileAll: lookupTable.o lexer.o lexer.h parser.o parser.h ast.o ast.h symbolTable.o symbolTable.h semanticAnalyser.o semanticAnalyser.h
	gcc -g driver.c lookupTable.o lexer.o parser.o stack.o tree.o ast.o symbolTable.o semanticAnalyser.o -o stage1exe

lookupTable.o: lookupTable.c lookupTable.h
	gcc -g -c lookupTable.c

lexer.o: lexer.c lookupTable.h lexerDef.h
	gcc -g -c lexer.c

parser.o: parser.c lexer.h parserDef.h stack.o stack.h tree.o tree.h
	gcc -g -c parser.c

stack.o: stack.c stackDef.h treeDef.h
	gcc -g -c stack.c

tree.o: tree.c treeDef.h
	gcc -g -c tree.c

ast.o: ast.c astDef.h
	gcc -g -c ast.c

symbolTable.o: symbolTable.c symbolTableDef.h
	gcc -g -c symbolTable.c

semanticAnalyser.o: semanticAnalyser.c
	gcc -g -c semanticAnalyser.c
