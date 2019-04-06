#include "../commands.h"

void image_filter_command(clut_runtime runtime,
                          const char *input_file,
                          const char *output_file,
                          unsigned char flags)
{
  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    image_filter_parallel(runtime, input_file, output_file);
  } else {
    image_filter(input_file, output_file);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("image filter ran in: %f seconds\n",
      ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    printf("Testing image filter is not implemented yet\n");
  } else {
    /* ... */
  }
}
