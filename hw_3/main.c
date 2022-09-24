#include <stdio.h>
#include <stdlib.h>

#define BUF_BLOCK_SIZE 64
#define ALLOC_ERR 111
#define FILE_ERR 112

#define RESIZE(buf, buf_len){                                                       \
        buf_len += BUF_BLOCK_SIZE;                                                  \
        buf = realloc(buf, buf_len * sizeof(*buf));                                                \
        if (!(buf)){                                                                \
            fprintf(stderr, "Allocation error: not enough memory\n");               \
            exit(ALLOC_ERR);                                                        \
        }                                                                           \
    }

#define PUSH_BACK(buf, buf_len, len, elm) {                                         \
	if (len >= (buf_len)){                                                          \
		RESIZE(buf, (buf_len));                                                     \
	}                                                                               \
	(buf)[(len)++] = (elm);                                                         \
}

typedef char* str_t;

void copy(str_t dst, str_t src, size_t len){
	for (size_t i = 0; i < len; ++i){
		*(dst++) = *(src++);
	}
}

str_t read_line(str_t* buf, size_t* buf_len, FILE* fd){
	size_t len = 0;
    int ch;
	while (((ch = fgetc(fd)) != '\n') && (ch != EOF)){
		PUSH_BACK(*buf, *buf_len, len, ch);
	}
	PUSH_BACK(*buf, *buf_len, len, 0);
	str_t str = malloc(len);
	copy(str, *buf, len);
	return str;
}

size_t read_lines(str_t** result, FILE* fd){
	str_t buf = NULL;
	size_t buf_len = 0;
    size_t lb_max_len = 64, lb_len = 0;
	str_t* lines = malloc(lb_max_len * sizeof(str_t));

    while (!feof(fd)){
        str_t line = read_line(&buf, &buf_len, fd);
        if (line && *line){
            PUSH_BACK(lines, lb_max_len, lb_len, line);
        }else{
            free(line);
        }
    }
    printf("DEBUG: buf_len = %zu\n", buf_len);
    free(buf);
    *result = lines;
    return lb_len;
}

#define T str_t

int compare(T left, T right){
    while (!(*left - *right) && *left){
        ++left;
        ++right;
    }
    return *left - *right;
}

void swap(T* a, T* b){
    T t = *a;
    *a = *b;
    *b = t;
}

T* q_sort_part(T* begin, T* end){
    T* pivot = end - 1;
    T* left_ptr = begin - 1;

    for (T* right_ptr = begin; right_ptr < end; ++right_ptr){
        if (compare(*right_ptr, *pivot) > 0) continue;
        ++left_ptr;
        swap(left_ptr, right_ptr);
    }

    return left_ptr + 1;
}

void q_sort(T* begin, T* end){
    if (begin >= end) return;
    T* part_ptr = q_sort_part(begin, end);
    q_sort(begin, part_ptr-1);
    q_sort(part_ptr+1,end);
}

void free_lines_buf(str_t* lines, size_t len){
    for (size_t i = 0; i < len; ++i){
        free(lines[i]);
    }
    free(lines);
}

int main(int argc, str_t* argv){
    FILE *fd;
    if (argc == 1){
        fd = stdin;
    }else{
        fd = fopen(argv[1], "r");
        if (!fd){
            fprintf(stderr, "File error: path=%s\n", argv[1]);
            exit(FILE_ERR);
        }
    }

	str_t* lines;
    size_t lines_c = read_lines(&lines, fd);
    for (size_t i = 0; i < lines_c; ++i){
        printf("Line %zu: %p,  %s\n", i, &lines[i], lines[i]);
    }
    putchar('\n');
    T *begin = lines, *end = lines + lines_c;
    q_sort(begin, end);
    for (size_t i = 0; i < lines_c; ++i){
        printf("%s\n", lines[i]);
    }
    
    free_lines_buf(lines, lines_c);
    fclose(fd);
	return 0;
}
