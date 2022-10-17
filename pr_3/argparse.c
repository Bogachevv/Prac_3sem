#include "argparse.h"

void print_help(int argc, char** argv, arg_t* args, int args_len){
	char* help_str = "--help";
	if ((argc == 1) || (strcmp(argv[1], help_str) != 0)) return;
	for (size_t i = 0; i < args_len; ++i){
		printf("\t-%s\t%s\n", args[i].key, args[i].description);
	}
	exit(0);
}

void parse(int argc, char** argv, arg_t* args, int args_len){
	print_help(argc, argv, args, args_len);
	for (size_t i = 1; i < argc; ++i){
		if (argv[i][0] != '-') continue;
		for (size_t j = 0; j < args_len; ++j){
			if (strcmp(argv[i] + 1, args[j].key) == 0){
				switch (args[j].arg_type){
					case string:
						args[j].val = argv[i + 1];
						break;
					case boolean:
						args[j].val = (void*)1;	
						break;
				}
			}
		}
	}

}
