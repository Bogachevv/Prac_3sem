#include "lex.h"
#include "dyn_arrs.h"
#include <string.h>

#define TRUE 1
#define FALSE 0
#define SPECIAL "&|;><()"

void non_quotes_mode(char *ch_p, char **str_beg, char **str_end, int *state){
	if (*ch_p == '"') {
		*state = 2;
		*str_beg = ch_p + 1;
		return;
	}
	if (*state == 0){
		*state = 1;
		*str_beg = ch_p;
	}
	*str_end = ch_p;
}

void quotes_mode(char *ch_p, char **str_beg, char **str_end, int *state){
	*state = 2;
	if (*ch_p == '"') {
		*state = 3;
		*str_end = ch_p - 1;
	}
}

int char_in_str(char ch, const char *str){
	for (size_t i = 0; str[i]; ++i) if (str[i] == ch) return TRUE;
	return FALSE;
}

char *build_str(char *str_beg, char *str_end){
	int str_len = str_end - str_beg + 1;
	char *new_str = malloc(str_len + 1);
	memcpy(new_str, str_beg, str_len);
	new_str[str_len] = 0;
	return new_str;
}

char **parse_input(char *str){
	int state = 0;
	char *str_beg = str, *str_end = str;
	char **res_buf = NULL;
	int buf_cap = 0, buf_len = 0;
	for (char *ch_p = str; ; ++ch_p){
		if ((*ch_p == ' ') && (state == 0)) continue;
		int is_spec = char_in_str(*ch_p, SPECIAL);
		if ((((*ch_p == ' ') || is_spec) && (state != 2)) || 
						(*ch_p == 0) || (*ch_p == '\n')){
			if (state != 0){
				//push str
				char *new_str = build_str(str_beg, str_end);
				PUSH_BACK(res_buf, buf_cap, buf_len, new_str);
				//!push str
			}
			state = 0;
			if ((*ch_p == 0) || (*ch_p == '\n')) break; //loop exit point
			if (is_spec){
				//push special
				str_beg = ch_p;
				str_end = (*(ch_p + 1) == *ch_p) ? (str_beg + 1) : str_beg;
				ch_p = str_end;
				char *new_str = build_str(str_beg, str_end);
				PUSH_BACK(res_buf, buf_cap, buf_len, new_str);
				//!push special
			}
			continue;
		}
		if (state < 2) non_quotes_mode(ch_p, &str_beg, &str_end, &state);
		else quotes_mode(ch_p, &str_beg, &str_end, &state);
	}	

	PUSH_BACK(res_buf, buf_cap, buf_len, NULL);
	return res_buf;
}
