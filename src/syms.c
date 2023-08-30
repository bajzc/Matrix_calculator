#include "syms.h"
#include "parser.h"

symvar* sym_table;

double matrix_get_entity(matrix_t* matrix, double x, double y) {
  printf("matrix:row:%u\tcol:%u\n", matrix->row, matrix->column);
  if (ceil(x) != x || ceil(y) != y) {
    puts("Matrix index is not an integer");
    return NAN;
  }
  if (x > matrix->row || y > matrix->column || x < 0 || y < 0) {
    puts("Matrix index out of range");
    return NAN;
  }

  return matrix->matrix[(size_t)x - 1][(size_t)y - 1];
}

matrix_t* matrix_temp(void) {
  matrix_t* temp = malloc(sizeof(matrix_t));
  if (!temp) {
    puts("matrix_temp malloc error!");
    exit(-1);
  }
  temp->column = 1;
  temp->row = 1;
  matrix_calloc(temp);
  return temp;
}

matrix_t* matrix_init(vector_t* vector) {
  if (!vector_check(vector)) {
    yyerror("Abort");
    return (matrix_t*)NULL;
  }
  matrix_t* matrix = malloc(sizeof(matrix_t));
  matrix->column = vector->arrays[0]->count + 1;
  matrix->row = vector->count + 1;
  matrix_malloc(matrix);
  for (int i = 0; i < matrix->row; i++) {
    memcpy(matrix->matrix[i], vector->arrays[i]->num,
           sizeof(double) * matrix->column);
  }
  for (int i = 0; i < matrix->row; i++) {
    for (int j = 0; j < matrix->column; j++) {
      matrix->matrix[i][j] = vector->arrays[i]->num[j];
    }
  }
  return matrix;
}

bool vector_check(vector_t* vector) {  // check if all arrays have the same size
  if (vector->count == 0)              // only one array in vector
    return true;
  int col = vector->arrays[0]->count;
  for (int i = 1; i <= vector->count; i++) {
    if (col != vector->arrays[i]->count) {
      printf("ERROR: vertical dimensions mismatch (1x%d vs 1x%d)\n", col,
             vector->arrays[i]->count);
      return false;
    }
  }
  return true;
}

vector_t* vector_init(array_t* arr) {
  vector_t* vector = malloc(sizeof(vector_t));
  if (!vector) {
    puts("vector malloc error");
    exit(-1);
  }
  vector->size = 4;
  vector->arrays = (array_t**)malloc(4 * sizeof(array_t*));
  vector->arrays[0] = malloc(sizeof(array_t));
  vector->arrays[0] = arr;
  vector->count = 0;
  return vector;
}

vector_t* vector_init_nums(double num1, double num2) {
  array_t* arr1 = array_init(num1, 0);
  array_t* arr2 = array_init(num2, 0);
  arr1->count = 0;
  arr2->count = 0;
  vector_t* vector = vector_init(arr1);
  vector_add(vector, arr2);
  return vector;
}

vector_t* vector_add(vector_t* vector, array_t* arr) {
  if (!vector) {
    puts("vector malloc error");
    exit(-1);
  }
  if (vector->count + 1 == vector->size) {
    array_t** temp =
        realloc(vector->arrays, sizeof(array_t*) * (vector->size + 4));
    if (!temp) {
      puts("vector realloc error");
      exit(-1);
    } else {
      vector->arrays = temp;
    }
    vector->size += 4;
  }
  vector->count++;
  vector->arrays[vector->count] = malloc(sizeof(array_t));
  vector->arrays[vector->count] = arr;
  return vector;
}

array_t* array_init(double num1, double num2) {
  array_t* array = malloc(sizeof(array_t));
  if (!array) {
    printf("array malloc error");
    exit(-1);
  }
  array->num = malloc(16 * sizeof(double));
  array->num[0] = num1;
  array->num[1] = num2;
  array->size = 16;
  array->count = 1;
  return array;
}

array_t* array_add(array_t* array, double num1) {
  if (!array) {
    puts("array malloc error");
    exit(-1);
  }
  if (array->count + 1 == array->size) {
    double* temp = realloc(array->num, sizeof(double) * (array->size + 16));
    if (!temp) {
      puts("array realloc error");
      exit(-1);
    } else {
      array->num = temp;
    }
    array->size += 16;
  }
  array->count += 1;
  array->num[array->count] = num1;
  return array;
}

struct init const funs[] = {
    {"atan", .value.fun = atan, 1},
    {"cos", .value.fun = cos, 1},
    {"tan", .value.fun = tan, 1},
    {"asin", .value.fun = asin, 1},
    {"acos", .value.fun = acos, 1},
    {"atan", .value.fun = atan, 1},
    {"exp", .value.fun = exp, 1},
    {"ln", .value.fun = log, 1},
    {"sin", .value.fun = sin, 1},
    {"sqrt", .value.fun = sqrt, 1},
    {0, 0, 0},
};

void init_table(void) {
  for (int i = 0; funs[i].name; i++) {
    symvar* ptr = putsym(funs[i].name, t_fun);
    ptr->value.fun = funs[i].value.fun;
  }
  symvar* ptr = putsym("pi", t_name);
  ptr->value.var = M_PI;
  ptr = putsym("e", t_name);
  ptr->value.var = M_E;
}

symvar* putsym(char const* name, int sym_type) {
  symvar* res = (symvar*)malloc(sizeof(symvar));
  res->name = strdup(name);
  res->type = sym_type;
  res->value.var = NAN;
  res->next = sym_table;
  sym_table = res;
  return res;
}

symvar* getsym(char const* name) {
  for (symvar* p = sym_table; p; p = p->next)
    if (strcmp(p->name, name) == 0)
      return p;
  return NULL;
}