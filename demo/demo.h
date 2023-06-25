//
// Created by LiZeCheng-Jason on 2023-06-25.
//
#ifndef DEMO_DEMO_H
#define DEMO_DEMO_H
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
void matrix_plus(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_t_print(matrix_t* a);
void matrix_minus(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_times_reorder(matrix_t* a, matrix_t* b, matrix_t* ans);
void matrix_LU_NbyN_Factorization(matrix_t* source, matrix_t* L, matrix_t* U,
                                  double* det, matrix_t* ans);
void matrix_inverse(matrix_t* a, matrix_t* ans);
double matrix_det(matrix_t* a);
int main(int argc, char** argv);
#endif  // DEMO_DEMO_H
