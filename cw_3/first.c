#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define T char
#define BLOCK_SIZE 16

T* resize(T* arr, size_t old_size, size_t new_size){
	T* new_ptr = (T*)malloc(new_size * sizeof(T));
	for (size_t i = 0; i < old_size; ++i){
		new_ptr[i] = arr[i];
	}
	if (arr != NULL)
		free(arr);
	return new_ptr;
}

void push_back(T** arr, T elm, size_t* len, size_t* max_len){
    if (*len >= *max_len){
        *max_len += BLOCK_SIZE;
        *arr = resize(*arr, *len, *max_len);
    }
    *(*arr + *len) = elm;
    ++(*len);
}

void reverse(char* str, size_t len){
	char t;
	for (size_t i = 0; i < len / 2; ++i){
		t = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = t;
	}
}

size_t str_len(const char* str){
	const char* beg = str;
	while (*str++);
	return str - beg - 2;
}

int my_strcmp(const char* str1, const char* str2){
	const char* s1 = str1, *s2 = str2;
	while (!(*s1 - *s2) && *s1){
		++s1;
		++s2;
	}
	return (*s1 - *s2);
}

int main(){
	int ch;
    size_t len = 0, max_len = 0;
	char* str = NULL;
	while ((ch = getchar()) != '\n'){
	    push_back(&str, ch, &len, &max_len);
    }
	push_back(&str, ch, &len, &max_len);
	printf("%s\n%ld\n", str, len);
	reverse(str, len);
	printf("%s\n%ld\n", str, len);
    free(str);
}	
