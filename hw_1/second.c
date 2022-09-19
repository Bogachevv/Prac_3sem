#include <stdio.h>


int main(){
	int ch = 0;
	int is_nl = 0;
	puts("Input text(end with ctrl+D): ");
	while ((ch = getchar()) != EOF){
		if (!is_nl || (ch != '\n')) putchar(ch);
		is_nl = ch == '\n';
	}
}
