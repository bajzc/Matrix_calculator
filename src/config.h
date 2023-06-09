#pragma once
// Preprocess
#ifdef __GNUC__
#define CLEAR "clear"
#elif defined _MSC_VER
#define CLEAR "cls"
#endif
#define PCRE2_CODE_UNIT_WIDTH 8

// Header files
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <uthash.h>

// Macro
#define TWO_MATRIX(require_nameA, require_nameB, name_A, name_B, namelength, \
                   func_name)                                                \
  do {                                                                       \
    (void)pcre2_substring_get_byname(MatchData[FUNCTION], require_nameA,     \
                                     &name_A, &namelength);                  \
    (void)pcre2_substring_get_byname(MatchData[FUNCTION], require_nameB,     \
                                     &name_B, &namelength);                  \
    if (!hash_have_name((char*)name_A)) {                                    \
      printf("Can't find matrix %s", (char*)name_A);                         \
      error_print("No matrix matched");                                      \
      return -1;                                                             \
    }                                                                        \
    if (!hash_have_name((char*)name_B)) {                                    \
      printf("Can't find matrix %s", (char*)name_B);                         \
      error_print("No matrix matched");                                      \
      return -1;                                                             \
    }                                                                        \
    identifier_t* matrix_A = hash_find_matrix((char*)name_A);                \
    identifier_t* matrix_B = hash_find_matrix((char*)name_B);                \
    identifier_t* matrix_ans = hash_find_matrix("ans");                      \
    func_name(matrix_A->matrix, matrix_B->matrix, matrix_ans->matrix);       \
    matrix_t_print(matrix_ans->matrix);                                      \
  } while (0)

// Data type
typedef struct {
  unsigned int row;
  unsigned int column;
  double** matrix;
} matrix_t;

typedef struct {
  char* name;
  UT_hash_handle hh;
  matrix_t* matrix;
} identifier_t;

// enum
enum {
  NOT_N_BY_N_MATRIX = 2,
  NOT_INVERTIBLE_MATRIX,
  UNKNOWN_INPUT,
  INPUT_ERROR
};
enum { ADD = 0, SUB, MLP, DET, INV, MATRIX_FUNCTION_NUMBER };
enum { NAME = 0, FUNCTION, STATEMENT, COLON, NUMBER, REGEX_OBJ_NUMBER };

// Functions
void matrix_malloc(matrix_t* a);
void matrix_calloc(matrix_t* a);
void matrix_free(matrix_t* a);
void matrix_add(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_t_print(matrix_t* a);
void matrix_sub(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_times_reorder(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_LU_NbyN_Decomposition(matrix_t* source, matrix_t* L, matrix_t* U,
                                  double* det, matrix_t* ans);
void matrix_inverse(matrix_t* a, matrix_t* ans);
double matrix_det(matrix_t* a);
int main(void);
matrix_t* matrix_t_copy(matrix_t* a);
int hash_have_name(char* MatrixName);
identifier_t* hash_new_matrix(char* MatrixName, matrix_t* NewMatrix);
identifier_t* hash_find_matrix(char* MatrixName);
void hash_print_all_matrix(void);
void hash_delete_all(void);
int cli(void);
void print_help_msg(void);
void error_exit(char* errormsg);
void error_print(char* errormsg);
int regex(const char* string);
void regex_malloc_all(void);
