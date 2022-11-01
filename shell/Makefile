flags=-std=gnu11 -g -Wall

all: shell.out

shell.out: main.o lex.o utils.o
	gcc lex.o utils.o main.o -o shell.out

main.o: main.c
	gcc -c main.c $(flags) -o main.o

lex.o: lex.c lex.h
	gcc -c lex.c $(flags) -o lex.o

utils.o: utils.c utils.h
	gcc -c utils.c $(flags) -o utils.o

clear:
	rm -f lex.o main.o shell.out
