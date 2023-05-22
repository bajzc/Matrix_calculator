//
// Created by LiZeCheng-tony on 2023-05-14.
//
#include "demo.h"
#include <stdio.h>
#include <stdlib.h>

void matrix_malloc(matrix_t *a) {
    a->matrix = (float **) malloc(
            (a->row) * sizeof(float
            // this * took me a whole afternoon,泰库啦!
            *));
    for (int i = 0; i < (a->row); i++) {
        a->matrix[i] = (float *) malloc(((a->column) * (sizeof(float))));
    }
}

void matrix_calloc(matrix_t *a) {
    a->matrix = (float **) malloc(
            (a->row) * sizeof(float *));
    for (int i = 0; i < (a->row); i++) {
        a->matrix[i] = (float *) calloc((a->column), (sizeof(float)));
    }
}

void matrix_free(matrix_t *a) {
    for (int i = 0; i < (a->row); i++) {
        free(a->matrix[i]);
    }
    free(a->matrix);
}

// For ans_matrix ONLY!
void ans_matrix_malloc(ans_matrix_t *a) {
    a->matrix = (float **) malloc((a->row) * sizeof(float *));
    for (int i = 0; i < (a->row); i++) {
        // init all elements to 0
        a->matrix[i] = (float *) malloc((a->column) * (sizeof(float)));
    }
}

void ans_matrix_calloc(ans_matrix_t *a) {
    a->matrix = (float **) malloc((a->row) * sizeof(float *));
    for (int i = 0; i < (a->row); i++) {
        // init all elements to 0
        a->matrix[i] = (float *) calloc((a->column), (sizeof(float)));
    }
}


void ans_matrix_free(ans_matrix_t *a) {
    for (int i = 0; i < (a->row); i++) {
        free(a->matrix[i]);
    }
    free(a->matrix);
}

ans_matrix_t matrix_plus(matrix_t a, matrix_t b) {
    ans_matrix_t ans;
    if (a.row != b.row || a.column != b.column) {
        exit(NOT_N_BY_N_MATRIX);
    }
    ans.row = a.row;
    ans.column = a.column;
    ans_matrix_malloc(&ans);
    for (int i = 0; i < ans.row; i++) {
        for (int j = 0; j < ans.column; j++) {
            ans.matrix[i][j] = a.matrix[i][j] + b.matrix[i][j];
        }
    }
    return ans;
}

void matrix_input(matrix_t *a) {
    printf("Input a %d * %d matrix:\n", a->row, a->column);
    for (int i = 0; i < a->row; i++) {
        for (int j = 0; j < a->column; j++) {
            scanf("%f", &a->matrix[i][j]);
        }
    }
}

void ans_matrix_t_print(ans_matrix_t ans) {
    for (int i = 0; i < ans.row; i++) {
        for (int j = 0; j < ans.column; j++) {
            printf("%.5f ", ans.matrix[i][j]);
        }
        printf("\n");
    }
}

void matrix_t_print(matrix_t ans) {
    for (int i = 0; i < ans.row; i++) {
        for (int j = 0; j < ans.column; j++) {
            printf("%.5f ", ans.matrix[i][j]);
        }
        printf("\n");
    }
}

ans_matrix_t matrix_minus(matrix_t a, matrix_t b) {
    ans_matrix_t ans;
    if (a.row != b.row || a.column != b.column) {
        exit(NOT_N_BY_N_MATRIX);
    }
    ans.row = a.row;
    ans.column = a.column;
    ans_matrix_malloc(&ans);
    for (int i = 0; i < ans.row; i++) {
        for (int j = 0; j < ans.column; j++) {
            ans.matrix[i][j] = a.matrix[i][j] - b.matrix[i][j];
        }
    }
    return ans;
}

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

ans_matrix_t matrix_times_reorder(matrix_t a, matrix_t b) {
    ans_matrix_t ans;
    ans.row = a.row;
    ans.column = b.column;
    ans_matrix_calloc(&ans);
    for (int i = 0; i < a.row; i++) {
        for (int k = 0; k < a.column; k++) {
            for (int j = 0; j < ans.column; j++) {
                ans.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
            }
        }
        printf("%d/%d\n", i, ans.row);
    }
    return ans;
}

ans_matrix_t matrix_LU_Factorization(matrix_t const *source, matrix_t *L, matrix_t *U) {
    unsigned int size = source->row;
    L->column = L->row = size;
    U->column = U->row = size; // U=L
    matrix_malloc(L);
    matrix_malloc(U);
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
    float U11 = source->matrix[0][0];
    for (unsigned i = 1; i < size; i++) {
        L->matrix[i][0] = source->matrix[i][0] / U11;
    }

    for (unsigned int x = 1; x < size; x++) {
        for (unsigned int j = x; j < size; j++) {
            float sum = 0;
            for (unsigned int k = 0; k < x; k++) {
                sum += L->matrix[x][k] * U->matrix[k][j];
            }
            U->matrix[x][j] = source->matrix[x][j] - sum;
        }
        for (unsigned int i = x + 1; i < size; i++) {
            float sum = 0;
            for (int k = 0; k < x + 1; k++) {
                sum += L->matrix[i][k] * U->matrix[k][x];
            }
            L->matrix[i][x] = (source->matrix[i][x] - sum);
            L->matrix[i][x] /= U->matrix[x][x];
        }
    }
    puts("init L and U:\nL:");
    matrix_t_print(*L);
    puts("U:");
    matrix_t_print(*U);
    puts("--------------");
    // inverse of L
    matrix_t L_inv;
    L_inv.row = L_inv.column = size;
    matrix_calloc(&L_inv);
    for (unsigned int i = 0; i < size; i++) {
        L_inv.matrix[i][i] = 1;
        for (unsigned int j = i + 1; j < size; j++) {
            L_inv.matrix[i][j] = 0;
        }
        for (unsigned int k = i + 1; k < size; k++) {
            float sum = 0;
            for (unsigned j = i; j <= k - 1; j++) {
                sum -= L->matrix[k][j] * L_inv.matrix[j][i];
            }
            L_inv.matrix[k][i] = sum;
        }
    }
    puts("L_inv:");
    matrix_t_print(L_inv);
    matrix_free(L);
    // inverse of U
    matrix_t U_inv;
    U_inv.row = U_inv.column = size;
    matrix_calloc(&U_inv);
    for (unsigned int j = 0; j < size; j++) {
        for (unsigned int i = 0; i < j; ++i) {
            for (unsigned int k = 0; k < j; k++) {
                U_inv.matrix[i][j] += U_inv.matrix[i][k] * U->matrix[k][j];
            }
        }
        for (unsigned int k = 0; k < j; k++) {
            U_inv.matrix[k][j] /= -1 * U->matrix[j][j];
        }
        U_inv.matrix[j][j] = 1 / U->matrix[j][j];
    }

    puts("U_inv:");
    matrix_t_print(U_inv);
    matrix_free(U);
    return matrix_times_reorder(U_inv, L_inv);
}

ans_matrix_t matrix_inverse(matrix_t *a) {
    if (a->column != a->row) {
        printf("Only n by n matrix has inverse!\n");
        exit(NOT_N_BY_N_MATRIX);
    }
    if (a->row == 1) {
        printf("No inverse matrix for 1 by 1 matrix!\n");
        exit(NOT_INVERTIBLE_MATRIX);
    }
    if (a->row == 2) {
        ans_matrix_t ans;
        ans.row = a->row;
        ans.column = a->column;
        ans_matrix_malloc(&ans);
        float det =
                a->matrix[0][0] * a->matrix[1][1] - a->matrix[0][1] * a->matrix[1][0];
        printf("det:%.5f\n", det);
        if (det == 0) {
            puts("Det(MATRIX)=0");
            exit(NOT_INVERTIBLE_MATRIX);
        }
        ans.matrix[0][0] = a->matrix[1][1] / det;
        ans.matrix[0][1] = a->matrix[0][1] * -1 / det;
        ans.matrix[1][0] = a->matrix[1][0] * -1 / det;
        ans.matrix[1][1] = a->matrix[0][0] / det;
        ans_matrix_t_print(ans);
        return ans;
    }

    if (a->matrix[0][0] == 0) {
        printf("LU decomposition is not suitable for this matrix!\n");
        exit(NOT_INVERTIBLE_MATRIX);
    }
    matrix_t L;
    matrix_t U;
    return matrix_LU_Factorization(a, &L, &U);
}

// main for demo
int main(int argc, char **argv) {
    matrix_t a;
    matrix_t b;

    //  if (argc != 1) {
    //    freopen(argv[1], "r", stdin);
    //  }

    puts("row and column for first matrix:");
    scanf("%d%d", &a.row, &a.column);
    matrix_malloc(&a);
    matrix_input(&a);
    puts("row and column for second matrix:");
    scanf("%d%d", &b.row, &b.column);
    matrix_malloc(&b);
    matrix_input(&b);
    printf("1.Plus\n2.Minus\n3.Times\n4.Inverse\n");
    int input_case;
    // getchar for '\n'
    getchar();
    scanf("%d", &input_case);
    ans_matrix_t ans;
    switch (input_case) {
        case 1:
            ans = matrix_plus(a, b);
            break;
        case 2:
            ans = matrix_minus(a, b);
            break;
        case 3:
            ans = matrix_times_reorder(a, b);
            break;
        case 4:
            printf("Inverse will use the first matrix\n");
            ans = matrix_inverse(&a);
            break;
        default:
            exit(UNKNOWN_INPUT);
    }
    ans_matrix_t_print(ans);
    // free the memory. as usually took 30min to free all
    matrix_free(&a);
    matrix_free(&b);
    ans_matrix_free(&ans);
    return 0;
}