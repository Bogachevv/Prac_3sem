#include <stdio.h>


int main(){
	int ch = 0, c = 0, is_pw = 1;
	puts("Input text(end with ctrl+D): ");
	while ((ch = getchar()) != EOF){
		c += is_pw && ((ch == ' ') || (ch == '\n'));
		is_pw = !((ch == ' ') || (ch == '\n'));
	}
	printf("Words count: %d\n", c);
}
