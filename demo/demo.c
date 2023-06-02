//
// Created by LiZeCheng-tony on 2023-05-14.
//
#include "demo.h"
bool ans_NULL = 0;
void matrix_malloc(matrix_t* a) {
  a->matrix = (double**)malloc(
      a->row * sizeof(double*));  // this * took me a whole afternoon,Ã©ø‚¿≤!
  for (unsigned int i = 0; i < (a->row); ++i) {
    a->matrix[i] = (double*)malloc((a->column) * sizeof(double));
  }
}

void matrix_calloc(matrix_t* a) {
  a->matrix = (double**)malloc((a->row) * sizeof(double*));
  for (unsigned int i = 0; i < (a->row); i++) {
    a->matrix[i] = (double*)calloc((a->column), (sizeof(double)));
  }
}

void matrix_free(matrix_t* a) {
  for (unsigned int i = 0; i < a->row; i++) {
    free(a->matrix[i]);
  }
  free(a->matrix);
  free(a);
}

// For ans_matrix ONLY!
void ans_matrix_malloc(ans_matrix_t* ans) {
  ans->matrix = (double**)malloc(ans->row * sizeof(double*));
  for (unsigned int i = 0; i < ans->row; i++) {
    ans->matrix[i] = (double*)malloc(ans->column * (sizeof(double)));
  }
}

void ans_matrix_calloc(ans_matrix_t* ans) {
  ans->matrix = (double**)malloc((ans->row) * sizeof(double*));
  for (unsigned int i = 0; i < ans->row; i++) {
    // init all elements to 0
    ans->matrix[i] = (double*)calloc((ans->column), (sizeof(double)));
  }
}

void ans_matrix_free(ans_matrix_t* ans) {
  for (unsigned int i = 0; i < (ans->row); i++) {
    free(ans->matrix[i]);
  }
  free(ans->matrix);
  free(ans);
}

void matrix_plus(matrix_t* a, matrix_t* b, ans_matrix_t* ans) {
  if (a->row != b->row || a->column != b->column) {
    exit(NOT_N_BY_N_MATRIX);
  }
  ans->row = a->row;
  ans->column = a->column;
  ans_matrix_malloc(ans);
  for (unsigned int i = 0; i < ans->row; i++) {
    for (unsigned int j = 0; j < ans->column; j++) {
      ans->matrix[i][j] = a->matrix[i][j] + b->matrix[i][j];
    }
  }
}

void matrix_input(matrix_t* a) {
  printf("Input a %d * %d matrix:\n", a->row, a->column);
  for (unsigned int i = 0; i < a->row; i++) {
    for (unsigned int j = 0; j < a->column; j++) {
      if (scanf_s("%lf", &a->matrix[i][j]) != 1) {
        puts("INPUT ERROR");
        exit(INPUT_ERROR);
      }
    }
  }
}

void ans_matrix_t_print(ans_matrix_t* ans) {
  for (unsigned int i = 0; i < ans->row; i++) {
    for (unsigned int j = 0; j < ans->column; j++) {
      printf("%.5f ", ans->matrix[i][j]);
    }
    putchar('\n');
  }
}

void matrix_t_print(matrix_t* a) {
  for (unsigned int i = 0; i < a->row; i++) {
    for (unsigned int j = 0; j < a->column; j++) {
      printf("%.5f ", a->matrix[i][j]);
    }
    printf("\n");
  }
}

void matrix_minus(matrix_t* a, matrix_t* b, ans_matrix_t* ans) {
  if (a->row != b->row || a->column != b->column) {
    exit(NOT_N_BY_N_MATRIX);
  }
  ans->row = a->row;
  ans->column = a->column;
  ans_matrix_malloc(ans);
  for (unsigned int i = 0; i < ans->row; i++) {
    for (unsigned int j = 0; j < ans->column; j++) {
      ans->matrix[i][j] = a->matrix[i][j] - b->matrix[i][j];
    }
  }
}

/*
ans_matrix_t matrix_times_standard(matrix_t a, matrix_t b) {
  ans_matrix_t ans;
  ans.row = a.row;
  ans.column = b.column;
  ans_matrix_malloc(&ans);
  for (int i = 0; i < ans.row; i++) {
	for (int j = 0; j < ans.column; j++) {
	  for (int k = 0; k < a.column; k++) {
		ans.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
	  }
	}
	printf("%d/%d\n", i, ans.row);
  }
  return ans;
}
*/

void matrix_times_reorder(matrix_t* a, matrix_t* b, ans_matrix_t* ans) {
  ans->row = a->row;
  ans->column = b->column;
  ans_matrix_calloc(ans);
  for (unsigned int i = 0; i < a->row; i++) {
    for (unsigned int k = 0; k < a->column; k++) {
      for (unsigned int j = 0; j < ans->column; j++) {
        ans->matrix[i][j] += a->matrix[i][k] * b->matrix[k][j];
      }
    }
    printf("Multiplication:%d/%d\n", i, ans->row);
  }
}

void matrix_LU_NbyN_Factorization(matrix_t* source, matrix_t* L, matrix_t* U,
                                  double* det, ans_matrix_t* ans) {
  bool flag_return_det = det != NULL ? 1 : 0;

  unsigned const int size = source->row;
  L->column = L->row = size;
  U->column = U->row = size;  // U=L
  matrix_calloc(L);
  matrix_calloc(U);
  // init L and U
  for (unsigned int i = 0; i < size; i++) {
    L->matrix[i][i] = 1;
    for (unsigned int j = i + 1; j < size; j++) {
      L->matrix[i][j] = 0;
    }
  }
  for (unsigned int i = 1; i < size; i++) {
    for (unsigned int j = 0; j < i; j++) {
      U->matrix[i][j] = 0;
    }
  }
  // init first row of U
  for (unsigned int j = 0; j < size; j++) {
    U->matrix[0][j] = source->matrix[0][j];
  }
  // init first column of L
  double U11 = source->matrix[0][0];
  for (unsigned i = 1; i < size; i++) {
    L->matrix[i][0] = source->matrix[i][0] / U11;
  }
  for (unsigned int x = 1; x < size; x++) {
    for (unsigned int j = x; j < size; j++) {
      double sum = 0;
      for (unsigned int k = 0; k < x; k++) {
        sum += L->matrix[x][k] * U->matrix[k][j];
      }
      U->matrix[x][j] = source->matrix[x][j] - sum;
    }
    for (unsigned int i = x + 1; i < size; i++) {
      double sum = 0;
      for (unsigned int k = 0; k < x + 1; k++) {
        sum += L->matrix[i][k] * U->matrix[k][x];
      }
      L->matrix[i][x] = (source->matrix[i][x] - sum);
      L->matrix[i][x] /= U->matrix[x][x];
    }
  }
  if (flag_return_det) {
    *det = 1;
    for (unsigned int i = 0; i < size; i++) {
      *det *= U->matrix[i][i];
    }
    return;
  } else {
    double det_ = 1;
    for (unsigned int i = 0; i < size; i++) {
      det_ *= U->matrix[i][i];
    }
    if (fabs(det_) < 10E-7) {
      puts("Det() is 0!");
      ans_NULL = 1;
      return;
    }
  }
  // inverse of L
  matrix_t* L_inv = (matrix_t*)malloc(sizeof(matrix_t));
  L_inv->row = L_inv->column = size;
  matrix_calloc(L_inv);
  for (unsigned int i = 0; i < size; i++) {
    L_inv->matrix[i][i] = 1;
    for (unsigned int j = i + 1; j < size; j++) {
      L_inv->matrix[i][j] = 0;
    }
    for (unsigned int k = i + 1; k < size; k++) {
      double sum = 0;
      for (unsigned j = i; j <= k - 1; j++) {
        sum -= L->matrix[k][j] * L_inv->matrix[j][i];
      }
      L_inv->matrix[k][i] = sum;
    }
  }
  // inverse of U
  matrix_t* U_inv = (matrix_t*)malloc(sizeof(matrix_t));
  U_inv->row = U_inv->column = size;
  matrix_calloc(U_inv);
  for (unsigned int j = 0; j < size; j++) {
    for (unsigned int i = 0; i < j; ++i) {
      for (unsigned int k = 0; k < j; k++) {
        U_inv->matrix[i][j] += U_inv->matrix[i][k] * U->matrix[k][j];
      }
    }
    for (unsigned int k = 0; k < j; k++) {
      U_inv->matrix[k][j] /= -1 * U->matrix[j][j];
    }
    U_inv->matrix[j][j] = 1 / U->matrix[j][j];
  }
  matrix_times_reorder(U_inv, L_inv, ans);
  matrix_free(U_inv);
  matrix_free(L_inv);
}

void matrix_inverse(matrix_t* a, ans_matrix_t* ans) {
  if (a->column != a->row) {
    printf("Only n by n matrix has inverse!\n");
    exit(NOT_N_BY_N_MATRIX);
  }
  if (a->row == 1) {
    printf("No inverse matrix for 1 by 1 matrix!\n");
    exit(NOT_INVERTIBLE_MATRIX);
  }
  if (a->row == 2) {
    ans->row = a->row;
    ans->column = a->column;
    ans_matrix_malloc(ans);
    double det =
        a->matrix[0][0] * a->matrix[1][1] - a->matrix[0][1] * a->matrix[1][0];
    if (det == 0) {
      puts("Det() = 0!");
      exit(NOT_INVERTIBLE_MATRIX);
    }
    printf("det:%.5f\n", det);
    ans->matrix[0][0] = a->matrix[1][1] / det;
    ans->matrix[0][1] = a->matrix[0][1] * -1 / det;
    ans->matrix[1][0] = a->matrix[1][0] * -1 / det;
    ans->matrix[1][1] = a->matrix[0][0] / det;
    return;
  }

  if (a->matrix[0][0] == 0) {
    printf("LU decomposition is not suitable for this matrix!\n");
    exit(NOT_INVERTIBLE_MATRIX);
  }
  matrix_t* L = (matrix_t*)malloc(sizeof(matrix_t));
  matrix_t* U = (matrix_t*)malloc(sizeof(matrix_t));
  matrix_LU_NbyN_Factorization(a, L, U, NULL, ans);
  matrix_free(L);
  matrix_free(U);
}

double matrix_det(matrix_t* a) {
  double det = 0;
  if (a->row == a->column) {
    if (a->row > 2) {
      matrix_t* L = (matrix_t*)malloc(sizeof(matrix_t));
      matrix_t* U = (matrix_t*)malloc(sizeof(matrix_t));
      matrix_LU_NbyN_Factorization(a, L, U, &det, NULL);
      matrix_free(L);
      matrix_free(U);
    } else {
      det =
          a->matrix[0][0] * a->matrix[1][1] - a->matrix[0][1] * a->matrix[1][0];
    }
  } else {
    puts("matrix should be N by N!");
    exit(NOT_N_BY_N_MATRIX);
  }
  return det;
}

// main for demo
int main(int argc, char** argv) {
  regex();
  return 0;

  matrix_t* a = (matrix_t*)malloc(sizeof(matrix_t));
  matrix_t* b = (matrix_t*)malloc(sizeof(matrix_t));
  //  if (argc != 1) {
  //    freopen(argv[1], "r", stdin);
  //  }
  puts("row and column for first matrix:");
  scanf_s("%d%d", &a->row, &a->column);
  matrix_malloc(a);
  matrix_input(a);
  puts("row and column for second matrix:");
  scanf_s("%d%d", &b->row, &b->column);
  matrix_malloc(b);
  matrix_input(b);
  printf("1.Plus\n2.Minus\n3.Times\n4.Inverse\n5.Det\n");
  int input_case;
  // getchar for '\n'
  char temp_char;
  while ((temp_char = getchar()) != '\n' && temp_char != ' ') {
    ungetc(temp_char, stdin);
  }
  scanf_s("%d", &input_case);
  ans_matrix_t* ans = (ans_matrix_t*)malloc(sizeof(ans_matrix_t));
  switch (input_case) {
    case 1:
      matrix_plus(a, b, ans);
      break;
    case 2:
      matrix_minus(a, b, ans);
      break;
    case 3:
      matrix_times_reorder(a, b, ans);
      break;
    case 4:
      printf("Inverse() will use the first matrix\n");
      matrix_inverse(a, ans);
      break;
    case 5:
      puts("Det() will use the first matrix");
      printf("%.5f", matrix_det(a));
      matrix_free(a);
      matrix_free(b);
      free(ans);
      return 0;
    default:
      exit(UNKNOWN_INPUT);
  }
  if (ans_NULL) {
    exit(-1);
  }
  ans_matrix_t_print(ans);
  // free the memory. as usual took 30min to free all
  // + 1h
  matrix_free(a);
  matrix_free(b);
  ans_matrix_free(ans);
  return 0;
}