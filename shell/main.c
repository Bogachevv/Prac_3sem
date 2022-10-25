#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "lex.h"


int main(int argc, char** argv){
	//char* str = "ABCDEF ABCDEF ABCDEF||ABCDEF \"Hello world\" mir";
	//printf("%s\n", str);
	while (!feof(stdin)){
		char *str = NULL; size_t len;
		len = getline(&str, &len, stdin);
		if (len == -1) break;
		printf("Line [%ld]: %s\n", len, str);
		char **parsed = parse_input(str);
		for (int i = 0; parsed[i]; ++i){
			printf("%s\n", parsed[i]);
		}
	}
}
