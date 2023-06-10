#include <uthash.h>
#include "demo.h"

identifier_t* bucket = NULL;
int hash_have_name(char* MatrixName) {
  identifier_t* s;
  HASH_FIND_STR(bucket, MatrixName, s);
  if (s == NULL)
    return 0;
  else
    return 1;
}
identifier_t* hash_new_matrix(char* MatrixName, matrix_t* NewMatrix) {
  identifier_t* new;
  new = malloc(sizeof(identifier_t*));
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