//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#ifndef DEMO_REGEX_H
#define DEMO_REGEX_H

int regex(const char* string);
void regex_malloc_all(void);

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
enum { ADD = 0, SUB, MLP, DET, INV, MATRIX_FUNCTION_NUMBER };
enum { NAME = 0, FUNCTION, STATEMENT, COLON, NUMBER, REGEX_OBJ_NUMBER };

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
    matrix_print(matrix_ans->matrix);                                        \
  } while (0)
#endif  // !DEMO_REGEX_H