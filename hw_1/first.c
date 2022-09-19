#include <stdio.h>


int main(){
	int ch = 0, c = 0;
	puts("Input text(end with ctrl+D): ");
	while ((ch = getchar()) != EOF){
		c += (ch == '\n');
	}
	printf("Lines count: %d\n", c);

}
