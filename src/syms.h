#ifndef SRC_SYMS_H
#define SRC_SYMS_H

#include "config.h"

typedef double(func_t)(double);

struct array_t {
  size_t size;
  int count;    // last number in *num (count from 0)
  double* num;  // flexible array
};

typedef struct array_t array_t;

struct vector_t {
  size_t size;
  int count;
  array_t** arrays;  // an array of all *arrays in the vector
};

typedef struct vector_t vector_t;

struct symvar {
  char* name;
  int type;  // t_name OR t_fun OR t_matrix
  union {
    double var;
    func_t* fun;
    matrix_t* matrix;
  } value;
  struct symvar* next;
};

typedef struct symvar symvar;

struct init {
  char const* name;
  union {
    func_t* fun;
    matrix_t* matrix;
  } value;
  int params;  // number of arguments
};

extern symvar* sym_table;

symvar* putsym(char const* name, int sym_type);
symvar* getsym(char const* name);
array_t* array_init(double num1, double num2);
array_t* array_add(array_t* array, double num1);
vector_t* vector_init(array_t* arr);
vector_t* vector_add(vector_t* vector, array_t* arr);
vector_t* vector_init_nums(double num1,double num2);
bool vector_check(vector_t* vector);
matrix_t* matrix_init(vector_t* vector);
matrix_t* matrix_temp(void);
void init_table(void);
double matrix_get_entity(matrix_t* matrix, double x, double y);
void yyerror(char const *);

#endif
