all:
	gcc -g syntax/syntax.c lexer/lexer.c syntax/syntax_types.c queue/queue.c symtable/symtable.c queue/queue_fill.c -o compiler
