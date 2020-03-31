scheme: shell.o parser.o lexer.o
	gcc -o scheme shell.o parser.o lexer.o

shell.o: shell.c
	gcc -c shell.c

parser.o: parser.c
	gcc -c parser.c

lexer.o: lexer.c
	gcc -c lexer.c

clean:
	rm -f *~ *.o *.a