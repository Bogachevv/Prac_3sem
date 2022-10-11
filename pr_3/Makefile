s=main
flags=-Wall -std=c11 -g

all: utf8.out utf16.out

argparse.o: argparse.c
	gcc -c argparse.c -o argparse.o

utf8.out: argparse.o $(s).c
	cpp $(s).c -D UTF8_IN > $(s).i
	gcc argparse.o $(s).i $(flags) -o utf8.out 
	rm -f $(s).i

utf16.out: argparse.o $(s).c
	cpp $(s).c -D UTF16_IN > $(s).i
	gcc argparse.o $(s).i $(flags) -o utf16.out 
	rm -f $(s).i

clear:
	rm -f argparse.o utf16.out utf8.out
