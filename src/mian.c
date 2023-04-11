//
// Created by LiZeCheng-tony on 2023-04-07.
//
#include "main.h"
void print_help_msg(char *error_input,bool is_error_input){
  printf("A tiny matrix calculator v1\n");
  printf("Copyright: GNU v3\n");
  if(error_input){
    printf("\033[31m\nUnrecognized command: '%s'\033[0m\n\n",error_input);//print red characters
  }
  printf("This calculator enable several matrix operations from the command line.\n");

}

int main(){
  char *input=NULL;
  printf("Tiny Matrix Calculator\n");
  printf("(h for help)>");
  scanf("%s",input);
}