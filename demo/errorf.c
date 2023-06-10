#include <stdio.h>
void error_exit(char* errormsg) {
  printf("\n\033[31m%s\033[0m\n", errormsg);
  printf("Press any key to exit...");
  setbuf(stdin, NULL);
  (void)getchar();
  exit(-1);
}
void error_main(char* errormsg) {
  printf("\n\n\033[31m%s\033[0m\n\n", errormsg);
  printf("Press any key to restart...");
  setbuf(stdin, NULL);
  (void)getchar();
  main();
}