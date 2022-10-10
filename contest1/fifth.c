#include <stdlib.h>
#include <stdio.h>

typedef FILE* file_t;

int get_min_pos(int* arr, int len){
	int min_pos = 0;
	for (int i = 1; i < len; ++i){
		if (arr[i] < arr[min_pos]) min_pos = i;
	}
	return min_pos;
}

int main(int argc, char** argv){
	int *buf = (int*)malloc((argc - 1) * sizeof(int));
	file_t *files = (file_t*)malloc((argc - 1) * sizeof(file_t)); 
	int files_c = argc - 1;
	for (int i = 1, wp = 0; i < argc; ++i){
		files[wp] = fopen(argv[i], "r");
		if (!files[wp]) {
			--files_c;
			continue;
		}
		int tmp = -1;
		int f_res = fscanf(files[wp], "%d", &tmp);
		buf[wp] = tmp;
		if (f_res != 1){
			fclose(files[wp]);
			--files_c;
			continue;
		}
		++wp;
	}
	
	while (files_c > 0){
		int pos = get_min_pos(buf, files_c);
		printf("%d ", buf[pos]); // need '\n'??
		//printf("buf[0] = %d, buf[1] = %d\n", buf[0], buf[1]);
		if (fscanf(files[pos], "%d", (buf + pos)) != 1){
			fclose(files[pos]);
			--files_c;
			for (int i = pos; i < files_c; ++i){
				buf[i] = buf[i + 1];
				files[i] = files[i + 1];
			}
		}
	}

	putchar('\n');
	free(buf);
	free(files);
}
