#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "clu.h"

#define MATRIX_INDEX(row, column, columns) (row * columns + column)

int *matrix_allocate(int rows,
                     int columns);
int matrix_equal(int *a,
                 int *b,
                 int rows,
                 int columns);
void matrix_println(int *matrix,
                    int rows,
                    int columns);

void matrix_transpose(int *a,
                      int *b,
                      int rows,
                      int columns);
void matrix_transpose_parallel(clut_runtime runtime,
                               int *a,
                               int *b,
                               int rows,
                               int columns);

void matrix_add(int *a,
                int *b,
                int *c,
                int rows,
                int columns);
void matrix_add_parallel(clut_runtime runtime,
                         int *a,
                         int *b,
                         int *c,
                         int rows,
                         int columns);

void matrix_multiply(int *a,
                     int *b,
                     int *c,
                     int a_rows,
                     int b_columns,
                     int a_columns_b_rows);
void matrix_multiply_parallel_v1(clut_runtime runtime,
                                 int *a,
                                 int *b,
                                 int *c,
                                 int a_rows,
                                 int b_columns,
                                 int a_columns_b_rows);
void matrix_multiply_parallel_v2(clut_runtime runtime,
                                 int *a,
                                 int *b,
                                 int *c,
                                 int a_rows,
                                 int b_columns,
                                 int a_columns_b_rows);

#endif
