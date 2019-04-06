#include "../commands.h"

void image_rotate_command(clut_runtime runtime,
                          const char *input_file,
                          const char *output_file,
                          float rotation_degrees,
                          unsigned char flags)
{
  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    image_rotate_parallel(runtime, input_file, output_file, rotation_degrees);
  } else {
    image_rotate(input_file, output_file, rotation_degrees);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("image rotate ran in: %f seconds\n",
      ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    printf("Testing image rotate is not implemented yet\n");
  } else {
    /* ... */
  }
}
