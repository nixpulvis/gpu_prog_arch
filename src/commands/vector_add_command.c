#include "../commands.h"

void vector_add_command(clut_runtime runtime,
                        int length,
                        unsigned char flags)
{
  int *a = vector_allocate(length);
  int *b = vector_allocate(length);
  int *c = vector_allocate(length);

  /* Populate a and b with random data. */
  srand(time(NULL));
  for (int i = 0; i < length; i++) {
    a[i] = rand() % 100;
    b[i] = rand() % 100;
  }

  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    vector_add_parallel(runtime, a, b, c, length);
  } else {
    vector_add(a, b, c, length);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("vector add ran in: %f seconds\n",
      ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    /* Run the other implementation. */
    int *d = vector_allocate(length);
    if (SHOULD_PARALLEL(flags)) {
      vector_add(a, b, d, length);
    } else {
      vector_add_parallel(runtime, a, b, d, length);
    }

    if (vector_equal(c, d, length)) {
      printf("SUCCCESS\n");
    } else {
      printf("FAILED\n");
    }
  } else {
    vector_println(a, length);
    printf("+\n");
    vector_println(b, length);
    printf("=\n");
    vector_println(c, length);
  }
}
