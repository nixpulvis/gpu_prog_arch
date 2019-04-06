#include "../commands.h"

void do_command(clut_runtime runtime,
                int a_rows,
                int b_columns,
                int a_columns_b_rows,
                unsigned char flags,
                int version)
{
  int *a = matrix_allocate(a_rows, a_columns_b_rows);
  int *b = matrix_allocate(a_columns_b_rows, b_columns);
  int *c = matrix_allocate(a_rows, b_columns);

  /* Populate a and b with random data. */
  srand(time(NULL));
  for (int i = 0; i < a_rows * a_columns_b_rows; i++) {
    a[i] = rand() % 100;
  }
  for (int i = 0; i < a_columns_b_rows * b_columns; i++) {
    b[i] = rand() % 100;
  }

  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    if (version == 1) {
      matrix_multiply_parallel_v1(runtime, a, b, c, a_rows, b_columns,
                                  a_columns_b_rows);
    } else {
      matrix_multiply_parallel_v2(runtime, a, b, c, a_rows, b_columns,
                                  a_columns_b_rows);
    }
  } else {
    matrix_multiply(a, b, c, a_rows, b_columns, a_columns_b_rows);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("matrix transpose ran in: %f seconds\n",
      ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    /* Run the other implementation. */
    int *d = matrix_allocate(a_rows, b_columns);
    if (SHOULD_PARALLEL(flags)) {
      matrix_multiply(a, b, d, a_rows, b_columns, a_columns_b_rows);
    } else {
      if (version == 1) {
        matrix_multiply_parallel_v1(runtime, a, b, d, a_rows, b_columns,
                                    a_columns_b_rows);
      } else {
        matrix_multiply_parallel_v2(runtime, a, b, d, a_rows, b_columns,
                                    a_columns_b_rows);
      }
    }

    if (matrix_equal(c, d, a_rows, b_columns)) {
      printf("SUCCCESS\n");
    } else {
      printf("FAILED\n");
    }
  } else {
    matrix_println(a, a_rows, a_columns_b_rows);
    printf("+\n");
    matrix_println(b, a_columns_b_rows, b_columns);
    printf("=\n");
    matrix_println(c, a_rows, b_columns);
  }
}

void matrix_multiply_v1_command(clut_runtime runtime,
                                int a_rows,
                                int b_columns,
                                int a_columns_b_rows,
                                unsigned char flags)
{
  do_command(runtime, a_rows, b_columns, a_columns_b_rows, flags, 1);
}

void matrix_multiply_v2_command(clut_runtime runtime,
                                int a_rows,
                                int b_columns,
                                int a_columns_b_rows,
                                unsigned char flags)
{
  do_command(runtime, a_rows, b_columns, a_columns_b_rows, flags, 2);
}
