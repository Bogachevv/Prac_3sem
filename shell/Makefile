flags=-std=gnu11 -g -Wall


all: shell.out 

shell.out: main.o lex.o utils.o cmdrun.o queue.o
	gcc queue.o lex.o utils.o cmdrun.o main.o -o shell.out

main.o: main.c
	gcc -c main.c $(flags) -o main.o

queue.o: queue.c queue.h
	gcc -c queue.c $(flags) -o queue.o

cmdrun.o: cmdrun.c cmdrun.h
	gcc -c cmdrun.c $(flags) -o cmdrun.o
	
lex.o: lex.c lex.h
	gcc -c lex.c $(flags) -o lex.o

utils.o: utils.c utils.h
	gcc -c utils.c $(flags) -o utils.o

clear:
	rm -f lex.o main.o utils.o cmdrun.o shell.out
