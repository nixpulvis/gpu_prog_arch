#include "../commands.h"

void matrix_transpose_command(clut_runtime runtime,
                              int rows,
                              int columns,
                              unsigned char flags)
{
  int *a = matrix_allocate(rows, columns);
  int *b = matrix_allocate(columns, rows);

  /* Populate a with random data. */
  srand(time(NULL));
  for (int i = 0; i < rows * columns; i++) {
    a[i] = rand() % 100;
  }

  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    matrix_transpose_parallel(runtime, a, b, rows, columns);
  } else {
    matrix_transpose(a, b, rows, columns);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("matrix transpose ran in: %f seconds\n",
      ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    /* Run the other implementation. */
    int *c = matrix_allocate(columns, rows);
    if (SHOULD_PARALLEL(flags)) {
      matrix_transpose(a, c, rows, columns);
    } else {
      matrix_transpose_parallel(runtime, a, c, rows, columns);
    }

    if (matrix_equal(b, c, rows, columns)) {
      printf("SUCCCESS\n");
    } else {
      printf("FAILED\n");
    }
  } else {
    matrix_println(a, rows, columns);
    printf("---\n");
    matrix_println(b, rows, columns);
  }
}
