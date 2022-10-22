#include "lex.h"
#include "dyn_arrs.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

char **parse_input(char *str){
	char **res_buf = NULL;
	int res_buf_cap = 0;
	int res_buf_len = 0;
	int ch, ignore_space = FALSE;
	int prev_sep = -1, i;
	for (i = 0, ch = *str; str[i] != 0; ++i, ch = str[i]){
		if ((ch == '&') || (ch == '|') || (ch == '>')){
			if (i > 0){
				int res_len = i - prev_sep;
				char *res = malloc(res_len + 1);
				strncpy(res, str + prev_sep + 1, res_len);
				res[res_len] = 0;
				prev_sep = i;
				PUSH_BACK(res_buf, res_buf_cap, res_buf_len, res);
			}
			int ln = 1 + (str[i+1] == ch);
			i += (str[i+1] == ch);
			prev_sep = i;
			char *res = malloc(ln + 1);
			for (int j = 0; j < ln; ++j) res[j] = ch;
			res[ln] = 0;
			PUSH_BACK(res_buf, res_buf_cap, res_buf_len, res);
			continue;
		}
		if ((ch == '"') && (str[i + 1] != ' ')){
			ignore_space = TRUE;
			continue;
		}
		if (((ch == ' ') && (!ignore_space)) || ((ch == '"') && (str[i + 1] == ' '))){
			//store str
			int res_len = i - prev_sep;
			char *res = malloc(res_len + 1);
			strncpy(res, str + prev_sep + 1, res_len);
			res[res_len] = 0;
			prev_sep = i;
			PUSH_BACK(res_buf, res_buf_cap, res_buf_len, res);
			i += (ch == '"');
			prev_sep += (ch == '"');
		}
	}
		
	int res_len = i - prev_sep;
	char *res = malloc(res_len + 1);
	strncpy(res, str + prev_sep + 1, res_len);
	res[res_len] = 0;
	prev_sep = i;
	PUSH_BACK(res_buf, res_buf_cap, res_buf_len, res);

	PUSH_BACK(res_buf, res_buf_cap, res_buf_len, NULL);
	return res_buf;
}
