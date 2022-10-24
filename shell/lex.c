#include "lex.h"
#include "dyn_arrs.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

void non_quotes_mode(char *ch_p, char **str_beg, char **str_end, int *state){
	if (*ch_p == '"'){
		*str_beg = ch_p + 1;
		*state |= 1; //set last bit
		return;
	}
	if (*ch_p == ' '){
		*str_end = ch_p;
		return;
	}
}

void quotes_mode(char *ch_p, char **str_beg, char **str_end, int *state){
	if (*ch_p != '"') return;
	if (*(ch_p + 1) == ' '){
		*str_end = ch_p + 1;
		*state &= -2; //clear last bit
	}
}

void spec_mode(char *ch_p, size_t cnt, char **str_beg, char **str_end){
	*str_beg = ch_p;
	*str_end = ch_p + cnt;
}

int char_in_str(char ch, const char *str){
	for (size_t i = 0; str[i]; ++i) if (str[i] == ch) return TRUE;
	return FALSE;
}

char **parse_input(char *str){
	char **res_buf = NULL;
	int state = 0;
	char *str_beg = str, *str_end = str - 1;
	for (char *ch_p = str; *ch_p; ++ch_p){
		if (char_in_str(*ch_p, "&|;><()")){
			size_t cnt = 1 + (*(ch_p + 1) == *ch_p);
			spec_mode(ch_p, cnt, &str_beg, &str_end);
		}
		if (state & 1) quotes_mode(ch_p, &str_beg, &str_end, &state);
		else non_quotes_mode(ch_p, &str_beg, &str_end, &state);

		if (str_end >= str_beg){
			size_t str_len = str_end - str_beg + 1;
			char *new_str = malloc(str_len + 1);
			memcpy(new_str, str_beg, str_len);
			new_str[str_len] = 0;
			printf("Parse res: %s\n", new_str);
			str_beg = ch_p + 1;
		}	
	}

	return res_buf;
}
