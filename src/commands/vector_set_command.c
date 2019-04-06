#include "../commands.h"

void vector_set_command(clut_runtime runtime,
                        int length,
                        unsigned char flags)
{
  int *a = vector_allocate(length);

  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    vector_set_parallel(runtime, a, length);
  } else {
    vector_set(a, length);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("vector add sub ran in: %f seconds\n", ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    /* Run the other implementation. */
    int *b = vector_allocate(length);
    if (SHOULD_PARALLEL(flags)) {
      vector_set(b, length);
    } else {
      vector_set_parallel(runtime, b, length);
    }

    if (vector_equal(a, b, length)) {
      printf("SUCCCESS\n");
    } else {
      printf("FAILED\n");
    }
  } else {
    vector_println(a, length);
  }
}
