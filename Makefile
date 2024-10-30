all:
	gcc -o program lexer.c token.c main.c stack.c parser.c ast.c -g
