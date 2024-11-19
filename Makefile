all:
	gcc -g syntax.c lexer.c queue.c symtable.c queue_fill.c precedence.c tree.c stack.c error.c code_gen.c -o compiler
