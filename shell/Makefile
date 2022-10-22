flags=-std=c11 -g -Wall

all: shell.out

shell.out: main.o lex.o
	gcc lex.o main.o -o shell.out

main.o: main.c
	gcc -C main.c $(flags) -o main.o

lex.o: lex.c
	gcc -C lex.c $(flags) -o lex.o

clear:
	rm -f lex.o main.o shell.out
