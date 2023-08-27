//
// Created by LiZeCheng-Jason on 2023-06-28.
//
#include "config.h"
#include "syms.h"
FILE* ifp = NULL;
int opt = 256;

int main(int argc, char** argv) {
  init_table();

#if HAVE_LOCALE_H
  (void)setlocale(LC_ALL, "");
#endif

  rl_readline_name = PACKAGE_NAME;
  rl_attempted_completion_function = cal_completion;
  rl_bind_key('\t', rl_complete);
  using_history();
  // readline library init

  while ((opt = getopt(argc, argv, "f:hv")) != EOF) {
    switch (opt) {
      case 'f':
        puts(optarg);
        if ((ifp = fopen(optarg, "r")) == NULL) {
          fprintf(stderr, "ERROR: cannot open file: '%s'", optarg);
          fclose(ifp);
          return -1;
        }
        cli('f');
        break;
      case 'h':
        print_help_msg();
        break;
      case 'v':
        print_ver_msg();
        break;
    }
  }

  // ./CLI -hv OR ./CLI -vh
  if (opt == 'h' || opt == 'v')
    return 0;

  // default model
  while (1)
    cli(0);
  return 0;
}
