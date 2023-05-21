//
// Created by LiZeCheng-tony on 2023-04-07.
//
#include "main.h"
bool start_times = 0;
void print_welcome_msg(void) {
  printf("\nA tiny matrix calculator Beta\n");
  printf("Copyright: GNU v3\n");
  printf("Tiny Matrix Calculator\n");
  start_times=1;
}
void print_help_msg(char *input, bool is_error_input) {
  clean_screen();

  if (is_error_input) {
    printf("\033[31mUnrecognized command: '%s'\033[0m\n\n",
           input); // print red characters
  }
  printf("%.80s",
         "This calculator enable several matrix operations from the command "
         "line.\n");
  printf("Functions:\n");
  printf("%-20s", "det(name)");
  printf("%-60s\n", "The determinant.");
  printf("%-20s", "inv(name)");
  printf("%-60s\n", "Find the inverse of matrix.");
  printf("%-20s", "example");
  printf("%-60s\n", "Show examples");
  printf("%-20s", "h");
  printf("%-60s", "Show information above.");

}

void clean_screen(void) {
#ifdef __WIN32__
  system("cls");
#else
  system("clear");
#endif
}

void parses();

int main(void) {
  char *input;
  if (!start_times) {
   print_welcome_msg();
  }
  printf("(h for help)>");
  gets(input);
  print_help_msg(input, 1);
  return 0;
}