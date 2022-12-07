flags=-Wall -O0 -g -std=gnu11

all: concumer.out producer.out

concumer.out: concumer.c ipc_utils.o settings.h
	gcc -c concumer.c $(flags) -o concumer.o
	gcc concumer.o ipc_utils.o -o concumer.out
	rm -f concumer.o

producer.out: producer.c ipc_utils.o settings.h
	gcc -c producer.c $(flags) -o producer.o
	gcc producer.o ipc_utils.o -o producer.out
	rm -f producer.o

ipc_utils.o: ipc_utils.c settings.h
	gcc -c ipc_utils.c $(flags) -o ipc_utils.o

clear:
	rm -f concumer.out producer.out ipc_utils.o
