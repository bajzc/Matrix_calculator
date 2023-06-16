#ifndef DEMO_REGEX_H
#define DEMO_REGEX_H

int regex(const char* string);

#ifdef __GNUC__
#define CLEAR "clear"
#elif defined _MSC_VER
#define CLEAR "cls"
#endif
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif  // !PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#define ADD 0
#define SUB 1
#define MLP 2
#define DET 3
#define INV 4

#define TWO_MATRIX(require_nameA, require_nameB, name_A, name_B, namelength, \
                   func_name)                                                \
  do {                                                                       \
    int error_code_a = pcre2_substring_get_byname(                           \
        FunctionMatchData, require_nameA, &name_A, &namelength);             \
    int error_code_b = pcre2_substring_get_byname(                           \
        FunctionMatchData, require_nameB, &name_B, &namelength);             \
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
    identifier_t* matrix_A = hash_find_matrix(name_A);                       \
    identifier_t* matrix_B = hash_find_matrix(name_B);                       \
    identifier_t* matrix_ans = hash_find_matrix("ans");                      \
    func_name(matrix_A->matrix, matrix_B->matrix, matrix_ans->matrix);       \
    matrix_t_print(matrix_ans->matrix);                                      \
  } while (0)
#endif  // !DEMO_REGEX_H