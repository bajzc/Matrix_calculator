//
// Created by LiZeCheng-Jason on 2023-06-28.
//
#include "config.h"
FILE *ifp=NULL;
	
int main(int argc, char **argv) {
  matrix_t* ans = malloc(sizeof(matrix_t));
	if (!ans) {// successfully allocated memory
  return -1;
  }
  	
#if HAVE_LOCALE_H
	(void)setlocale (LC_ALL, "");
#endif

	rl_readline_name = PACKAGE_NAME;
	rl_attempted_completion_function=cal_completion;
	rl_bind_key('\t',rl_complete);
	using_history();
	// readline library init
	
  ans->column = 1;
  ans->row = 1;
  matrix_calloc(ans);
  hash_new_matrix("ans", ans);
  regex_malloc_all();
  
	int opt=256;
	while((opt = getopt(argc,argv,"f:hv"))!=EOF){
		switch (opt)
		{
			case 'f':
			puts(optarg);
			if((ifp=fopen(optarg,"r"))==NULL){
				fprintf(stderr,"ERROR: cannot open file: '%s'",optarg);
				return -1;
			}
			cli('f');
			break;
			case 'h':
			print_help_msg();
			return 0;
			case 'v':
			print_ver_msg();
			return 0;
		}
	}
	
	// ./CLI -hv OR ./CLI -vh
	if(opt=='h'||opt=='v')
		return 0;
	
	// default model
  while (1)
      cli(0);
  return 0;
}
