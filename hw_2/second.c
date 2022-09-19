#include <stdio.h>

int main(){
	int ch, is_sep = 0, is_prev_sep = 0;
	while ((ch = getchar()) != EOF){
		is_sep = (ch == ' ' || ch == '\n' || ch == ',');
		putchar(is_sep ? (is_prev_sep ? 0 : '\n') : ch);
		is_prev_sep = is_sep;
	}
	return 0;
}
