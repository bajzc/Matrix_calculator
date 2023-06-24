#include "hash.h"
#include "demo.h"
identifier_t* bucket = NULL;
int hash_have_name(char* MatrixName) {
  extern identifier_t* bucket;
  identifier_t* s;
  HASH_FIND_STR(bucket, MatrixName, s);
  if (s == NULL)
    return 0;
  else
    return 1;
}
identifier_t* hash_new_matrix(char* MatrixName, matrix_t* NewMatrix) {
  identifier_t* new;
  new = malloc(sizeof(identifier_t));
  new->name = MatrixName;
  HASH_ADD_STR(bucket, name, new);
  new->matrix = NewMatrix;
  return new;
}
identifier_t* hash_find_matrix(char* MatrixName) {
  identifier_t* old;
  HASH_FIND_STR(bucket, MatrixName, old);
  return old;
}

void hash_print_all_matrix(void) {
  identifier_t* s;

  for (s = bucket; s != NULL; s = s->hh.next) {
    if (s->matrix->matrix != NULL) {
      printf("%s: %d by %d\n", s->name, s->matrix->row, s->matrix->column);
      matrix_t_print(s->matrix);
      printf("\n");
    } else {
      printf("%s: NO DATA\n", s->name);
    }
  }
}

void hash_delete_all(void) {
  identifier_t* s;
  for (s = bucket; s != NULL; s = s->hh.next) {
    if (strncmp(s->name, "ans", 3) == 0) {
      continue;
    }
    matrix_free(s->matrix);
    free(s->matrix);
    HASH_DEL(bucket, s);
  }
}
