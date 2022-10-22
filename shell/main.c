#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

int readline(char *buf, int *buf_cap, FILE *fd){
	int ch;
	while ((ch = getchar()) != EOF){
		if (ch == '\n') break;
		
	}

}

int main(int argc, char** argv){
	//char* str = "ABCDEF ABCDEF ABCDEF||ABCDEF \"Hello world\" mir";
	//printf("%s\n", str);
	while (!feof(stdin)){
		char *str = NULL; size_t len;
		len = getline(&str, &len, stdin);
		printf("Line [%d]: %s\n", len, str);
		char **parsed = parse_input(str);
		for (int i = 0; parsed[i]; ++i){
			printf("%s\n", parsed[i]);
		}
	}
}
