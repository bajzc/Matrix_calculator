//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#include "config.h"

void print_help_msg(void) {
  printf("%.80s\n", PACKAGE_STRING);
  printf("License: GPL-3.0 Author: bajzc %s\n", PACKAGE_BUGREPORT);
  printf("Math Functions:\n");
  printf("%-40s", "1.add(matrix_A,matrix_B)");
  printf("%-40s\n", "matrix_A + matrix_B");
  printf("%-40s", "2.sub(matrix_A,matrix_B)");
  printf("%-40s\n", "matrix_A - matrix_B");
  printf("%-40s", "3.mlp(matrix_A,matrix_B)");
  printf("%-40s\n", "matrix_A * matrix_B");
  printf("%-40s", "4.inv(matrix)");
  printf("%-40s\n", "Find the inverse of matrix");
  printf("%-40s", "5.det(matrix)");
  printf("%-40s\n", "The determinant of matrix");
  printf("Answer of 1-4 will be stored in \"ans\"\n");
  printf("\n\nIntrinsic function:\n");
  printf("%-10s%-70s\n", "\"name\"", "Print the matrix");
  printf("%-10s", "list");
  printf("%-70s\n", "Show all initialized matrix");
  printf("%-10s", "clean");
  printf("%-70s\n", "Delete all matrix stored in memory! (ans will remain)");
  printf("%-10s", "quit(exit)");
  printf("%-70s\n", "quit");
  printf("%-10s", "clear");
  printf("%-70s\n", "Clean the screen");
  printf("%-10s", "help");
  printf("%-70s\n", "Show this information.");
  printf("\nHave a try:\n");
  printf(">>> A_1=[2 4 2 6; 4 9 6 15;2 6 9 18;6 15 18 40]\n");
  printf(">>> inv(A_1)\n");
  printf(">>> ans\n\n");
}
void print_ver_msg(void) {
  printf("%.80s\n", PACKAGE_STRING);
  printf("Copyright (C) 2023 Li Zecheng.\n");
  printf(
      "This program is free software: you can redistribute it and/or modify it "
      "under \n"
      "the terms of the GNU General Public License as published by the Free "
      "Software \n"
      "Foundation, either version 3 of the License, or (at your option) any "
      "later version.\n"
      "This program is distributed in the hope that it will be useful, but "
      "WITHOUT ANY \n"
      "WARRANTY; without even the implied warranty of MERCHANTABILITY or "
      "FITNESS \n"
      "FOR A PARTICULAR PURPOSE. See the GNU General Public License for more "
      "details.\n"
      "You should have received a copy of the GNU General Public License along "
      "with \n"
      "this program. If not, see <https://www.gnu.org/licenses/>.\n");
}
/*
int cli(void) {
  setbuf(stdin, NULL);
  printf(">>>");
  char* input = malloc(sizeof(char) * 1024 * 2);
  if (input == NULL) {
    printf("Memory error!\n");
    exit(-1);
  }
  char* ptr = input;
  char ch = (char)getchar();
  if (ch == '\n')
    cli();
  *ptr = ch;
  ptr++;
  while ((ch = (char)getchar()) != '\n') {
    *ptr = ch;
    ptr++;
  }
  *ptr = '\0';
  (void)regex(input);
  return 0;
}
*/

// reference: https://tiswww.case.edu/php/chet/readline/readline.html#A-Short-Completion-Example
COMMAND commands[] = {
  { "add", "add(matrix_A,matrix_B)" },
  { "sub", "matrix_A - matrix_B" },
  { "inv",  "1 / matrix" },
  { "mlp",  "matrix_A * matrix_B" },
  { "det",  "The determinant of matrix" },
	{"list","Show all initialized matrix"},
	{"clean","Delete all matrix stored in memory! (ans will remain)"},
	{"quit","quit this program"},
	{"exit","same as above"},
	{"help","print help message"},
  { (char *)NULL, (char *)NULL }
};//readline completion commands

char *dupstr (char *s)
{
  char *r;

  r = malloc (strlen (s) + 1);
  strcpy (r, s);
  return r;
}

char **cal_completion(const char *text,int start,int end){
	char **matches=NULL;
	
	rl_attempted_completion_over=1;// disable default completion
	
	int list_index = 0;
	char *name;

  while (name = commands[list_index].name)
    {
    	if (strncmp (name, text, 3) == 0)
        break;
       list_index++;
    }
    
    
	
	if(start==0){
		if(list_index<5){
			matches = rl_completion_matches (text, var_generator);
		}
		matches = rl_completion_matches (text, command_generator);
	}
	return matches;
}

char *var_generator (const char *text,int state){
	// TODO list all variable
	
	return NULL;
}

char *command_generator (const char *text,int state)
{
  static int list_index, len;
  char *name;

  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  while (name = commands[list_index].name)
    {
   // TODO functions should follow by '('
		rl_completion_suppress_append=1;// "not appended to matches at the end of the command line"
      list_index++;
      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    }

  return NULL;
}

int cli(int opt) {
  if (opt == 'f') {
    extern FILE* ifp;
    char* input = malloc(sizeof(char) * 1024 * 2);
    while (fgets(input, 1024 * 2 - 1, ifp) != NULL) {
      input[strcspn(input, "\n")] = 0;
      add_history(input);
      (void)regex(input);
    }
    free(input);
    fclose(ifp);
    return 0;
  }

  // normal mode
  setbuf(stdin, NULL);
  char* input = readline(">>> ");
  if (*input == EOF || *input == '\n') {
    free(input);
    cli(0);
    return 0;
  }
	add_history(input);
  (void)regex(input);
  return 0;
}
