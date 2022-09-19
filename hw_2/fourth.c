#include <stdio.h>

#define MAXLEN 80

#define SWAP(a, b) typeof(a) t = a; a = b; b = t

int main(){
	char str1[MAXLEN];
	char str2[MAXLEN];
	char *s1 = str1, *s2 = str2;
	int ch, len = 0, max_len = 0, is_sep;

	while ((ch = getchar()) != EOF){
		is_sep = (ch == '\n') || (ch == ' ') || (ch == ',');
		if (is_sep){
			s1[len] = 0;
			if (len > max_len){
				max_len = len;
				SWAP(s1, s2);
			}
			len = 0;
		}
		else{
			s1[len] = ch;
			++len;
		}

	}

	printf("%s\n", (len > max_len) ? s1 : s2);

}
