flags=-std=gnu11 -g -Wall


all: shell.out async_controller.out

async_controller.out: async_controller.o cmdrun.o utils.o
	gcc utils.o cmdrun.o async_controller.o -o async_controller.out

shell.out: main.o lex.o utils.o cmdrun.o
	gcc lex.o utils.o cmdrun.o main.o -o shell.out

main.o: main.c
	gcc -c main.c $(flags) -o main.o

async_controller.o: async_controller.c
	gcc -c async_controller.c $(flags) -o async_controller.o

cmdrun.o: cmdrun.c cmdrun.h
	gcc -c cmdrun.c $(flags) -o cmdrun.o
	
lex.o: lex.c lex.h
	gcc -c lex.c $(flags) -o lex.o

utils.o: utils.c utils.h
	gcc -c utils.c $(flags) -o utils.o

clear:
	rm -f lex.o main.o utils.o cmdrun.o shell.out async_controller.out
