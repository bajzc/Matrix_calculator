//
// Created by LiZeCheng-Jason on 2023-06-28.
//
#include "config.h"
int main(void) {
  matrix_t* ans = malloc(sizeof(matrix_t));
  if (ans) { // successfully allocated memory
    ans->column = 1;
    ans->row = 1;
    matrix_calloc(ans);
    hash_new_matrix("ans", ans);
    regex_malloc_all();
    while (1)
      cli();
  }
  return 0;
}
