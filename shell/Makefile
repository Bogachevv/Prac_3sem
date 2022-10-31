flags=-std=gnu11 -g -Wall

all: shell.out

shell.out: main.o lex.o
	gcc lex.o main.o -o shell.out

main.o: main.c
	gcc -c main.c $(flags) -o main.o

lex.o: lex.c
	gcc -c lex.c $(flags) -o lex.o

clear:
	rm -f lex.o main.o shell.out
