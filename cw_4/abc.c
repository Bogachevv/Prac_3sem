#include <stdio.h>

int main(){
	int i, j = 1, k = 9;
	do while (i = k--);
	printf("i = %d, j = %d, k = %d\n", i, j, k);
	while (k=++i, j--);
	printf("i = %d, j = %d, k = %d\n", i, j, k);
}
