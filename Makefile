all:
	gcc -g syntax/syntax.c lexer/lexer.c queue/queue.c symtable/symtable.c queue/queue_fill.c precedence/precedence.c tree/tree.c precedence/stack.c errors/error.c code_gen/code_gen.c -o compiler
