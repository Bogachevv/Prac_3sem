#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

void free_parsed(char **parsed){
	for (char **str_ptr = parsed; *str_ptr; ++str_ptr){ 
		free(*str_ptr);
	}
	free(parsed);
}


int main(int argc, char** argv){
	char *str = NULL; size_t str_cap = 0, len = 0;
	while (!feof(stdin)){
		printf(">>");
		len = getline(&str, &str_cap, stdin);
		if (len == -1) break;
		printf("Line [%ld]: %s\n", len, str);
		char **parsed = parse_input(str);
		for (int i = 0; parsed[i]; ++i){
			printf("%s\n", parsed[i]);
		}
		free_parsed(parsed);
	}
	free(str);
}
