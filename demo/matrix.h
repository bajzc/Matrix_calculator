//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#ifndef DEMO_MATRIX_H
#define DEMO_MATRIX_H
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>
// data type
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

// functions
void matrix_malloc(matrix_t* a);
void matrix_calloc(matrix_t* a);
void matrix_free(matrix_t* a);
void matrix_add(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_print(matrix_t* a);
void matrix_sub(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_mlp_reorder(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_LU_NbyN_Decomposition(matrix_t* source, matrix_t* L, matrix_t* U,
                                  double* det, matrix_t* ans);
void matrix_inverse(matrix_t* a, matrix_t* ans);
double matrix_det(matrix_t* a);
int main(void);
matrix_t* matrix_copy(matrix_t* a);
#endif  // DEMO_MATRIX_H