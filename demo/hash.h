#ifndef DEMO_HASH_H
#define DEMO_HASH_H
int hash_have_name(char* MatrixName);
identifier_t* hash_new_matrix(char* MatrixName, matrix_t* NewMatrix);
identifier_t* hash_find_matrix(char* MatrixName);

#endif  // !DEMO_HASH_H
