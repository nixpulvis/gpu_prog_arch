#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "clu.h"

int *vector_allocate(int length);
int  vector_equal(int *c1,
                  int *c2,
                  int length);
void vector_println(int *array,
                    int length);

void vector_add(int *a,
                int *b,
                int *c,
                int length);
void vector_add_parallel(clut_runtime runtime,
                         int *a,
                         int *b,
                         int *c,
                         int length);

void vector_sqrt_ab(int *a,
                    int *b,
                    int *c,
                    int length);
void vector_sqrt_ab_parallel(clut_runtime runtime,
                             int *a,
                             int *b,
                             int *c,
                             int length);

void vector_add_sub(int *a,
                    int *b,
                    int *c,
                    int length);
void vector_add_sub_parallel(clut_runtime runtime,
                             int *a,
                             int *b,
                             int *c,
                             int length);

void vector_set(int *a,
                int length);
void vector_set_parallel(clut_runtime runtime,
                         int *a,
                         int length);


#endif
