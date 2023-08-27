//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#include "config.h"
void print_help_msg(void) {
  printf("CLI release 1.0-rc-1\n");
  printf("%.80s", "Matrix calculator(1.0)\n");
  printf("License: GPL-3.0 Author: bajzc lzecheng98@gmail.com\n");
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
  printf(">>>A_1=[2 4 2 6; 4 9 6 15;2 6 9 18;6 15 18 40]\n");
  printf(">>>inv(A_1)\n");
  printf(">>>ans\n\n");
}
int cli(void) {
  setbuf(stdin, NULL);
  printf(">>>");
  char* input = malloc(sizeof(char) * 1024 * 4);
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
