#ifndef DEMO_HASH_H
#define DEMO_HASH_H
#include <uthash.h>
int hash_have_name(char *MatrixName);
identifier_t *hash_new_matrix(char *MatrixName, matrix_t *NewMatrix);
identifier_t *hash_find_matrix(char *MatrixName);
void hash_print_all_matrix(void);
void hash_delete_all(void);
#endif // !DEMO_HASH_H
