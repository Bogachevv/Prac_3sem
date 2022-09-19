#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 256
#define STRERROR 111

int str_error = 0;

int read_str(char* str, size_t max_size){
	int ch, i = 0;
	while ((i < max_size) && ((ch = getchar()) != EOF) && (ch != '\n')){
		str[i] = ch;
		++i;
	}
	i = (i < max_size) ? i : (i - 1);
	str[i] = 0; //null terminated string
	return i;
}

void reverse(char* str, size_t len){
	char t;
	for (size_t i = 0; i < len / 2; ++i){
		t = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = t;
	}
}

int str_copy(char* dst, char* src, size_t max_len){
	size_t i = 0;
	for (; (i < max_len) && (src[i] != 0); ++i){
		dst[i] = src[i];
	}
	return i;
}

int str_to_int(char* str){
	int sgn = 1;
	if (*str == '-'){
		sgn = -1;
		++str;
	}
	int err = 0, res = 0;
	for (; *str; ++str){
		err = err || !((*str >= '0') && (*str <= '9'));
		res = res * 10 + (*str - '0');
	}
	str_error = err;
	return res * sgn;
}

int int_to_str(char* str, int val){
	char* s_ptr = str;
	int sgn = 0;
	if (val < 0){
		sgn = 1;
		val = -val;
	}
	else if (0 == val){
		*(s_ptr++) = '0';
		*s_ptr = 0;
		return s_ptr - str;
	}

	while (val){
		*(s_ptr++) = (val % 10) + '0';
		val /= 10;
	}
	if (sgn)
		*(s_ptr++) = '-';
	*s_ptr = 0;
	reverse(str, s_ptr - str);
	return s_ptr - str;
}

int test_convert(int min_int, int max_int){
	char str[MAXLEN];
	int test_res = 1;	
	for (int i = min_int; i <= max_int; ++i){
		int_to_str(str, i);
		if (str_error)
			exit(STRERROR);
		int res = str_to_int(str);
		if (str_error)
			exit(STRERROR);
		if (res != i){
			printf("TEST ERROR:\ti=%d\tres=%d\tstr=%s\n", i, res, str);
			test_res = 0;
		}
	}
	return test_res;
}

int main(){
	if (!test_convert(-10000, 10000)){
		exit(112);
	}
	char str[MAXLEN];
	int str_len = read_str(str, MAXLEN);
	printf("%s\nlen=%d\n", str, str_len);

	int rs = str_to_int(str);
	printf("str_to_int: %d\nstr_error: %d\n", rs, str_error);	
	if (str_error)
		exit(STRERROR);	

	char is[MAXLEN];
	int isl = int_to_str(is, rs);
	printf("int_to_str: %s\nstr_len: %d\n", is, isl);

	return 0;
}
