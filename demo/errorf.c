#include "errorf.h"
void error_exit(char* errormsg) {
  printf("\n\033[31m%s\033[0m\n", errormsg);
  printf("Press any key to exit...");
  setbuf(stdin, NULL);
  (void)getchar();
  exit(-1);
}
void error_print(char* errormsg) {
  printf("\n\033[31m%s\033[0m\n", errormsg);
  setbuf(stdin, NULL);
}
