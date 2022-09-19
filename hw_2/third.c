#include <stdio.h>

int max(int a, int b){
	return (a > b) ? a : b;
}

int main(){
	int ch, is_sep = 0, max_len = 0, cur_len = 0;
	while ((ch = getchar()) != EOF){
		is_sep = (ch == '\n') || (ch == ' ') || (ch == ',');
		cur_len += !is_sep;
		max_len = max(max_len, cur_len);
		cur_len = is_sep ? 0 : cur_len;
	}
	printf("Max len: %d\n", max_len);
	
	return 0;
}
