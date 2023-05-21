//
// Created by LiZeCheng-tony on 2023-05-14.
//

#ifndef MATRIX_CALCULATOR_DEMO_H
#define MATRIX_CALCULATOR_DEMO_H
// #include "uthash.h"
typedef struct {
  unsigned int row;
  unsigned int column;
  float **matrix;
} matrix_t;

typedef struct {
  unsigned int row;
  unsigned int column;
  float **matrix;
} ans_matrix_t;

typedef struct {
  char *name;
  //  UT_hash_handle hh;
  matrix_t matrix;
} identifier_t;

enum { NOT_N_BY_N_MATRIX, NOT_INVERTIBLE_MATRIX, UNKNOWN_INPUT };
#endif // MATRIX_CALCULATOR_DEMO_H
