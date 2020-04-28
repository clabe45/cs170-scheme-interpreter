CFLAGS = -ggdb

scheme: shell.o evaluator.o environment.o parser.o lexer.o
	gcc $(CFLAGS) -o scheme shell.o evaluator.o environment.o parser.o lexer.o

shell.o: shell.c
	gcc $(CFLAGS) -c shell.c

evaluator.o: evaluator.c
	gcc $(CFLAGS) -c evaluator.c

environment.o: environment.c
	gcc $(CFLAGS) -c environment.c

parser.o: parser.c
	gcc $(CFLAGS) -c parser.c

lexer.o: lexer.c
	gcc $(CFLAGS) -c lexer.c

clean:
	rm -f *~ *.o *.a
