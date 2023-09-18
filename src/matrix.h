#ifndef SRC_MATRIX_H
#define SRC_MATRIX_H

typedef struct
{
  unsigned int row;
  unsigned int column;
  double **matrix;
} matrix_t;

enum
{
  NOT_N_BY_N_MATRIX = 2,
  NOT_INVERTIBLE_MATRIX,
  UNKNOWN_INPUT,
  INPUT_ERROR
};

void
matrix_malloc (matrix_t *a);
void
matrix_calloc (matrix_t *a);
void
matrix_free (matrix_t *a);
void
matrix_add (matrix_t *a, matrix_t *b, matrix_t *ans);
void
matrix_t_print (matrix_t *a);
void
matrix_sub (matrix_t *a, matrix_t *b, matrix_t *ans);
void
matrix_times_reorder (matrix_t *a, matrix_t *b, matrix_t *ans);
void
matrix_LU_NbyN_Decomposition (matrix_t *source, matrix_t *L, matrix_t *U,
			      double *det, matrix_t *ans);
void
matrix_inverse (matrix_t *a, matrix_t *ans);
double
matrix_det (matrix_t *a);
matrix_t *
matrix_t_copy (matrix_t *a);
void
matrix_transpose (matrix_t *a, matrix_t *ans);

#endif