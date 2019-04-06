#include "vector.h"

int *vector_allocate(int length)
{
  return (int *) malloc(length * sizeof(int));
}

int vector_equal(int *a,
                 int *b,
                 int length)
{
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i]) {
      return 0;
    }
  }
  return 1;
}

void vector_println(int *a,
                    int length)
{
  for(int i = 0; i < length; i++) {
    printf("%d\t", a[i]);
  }
  printf("\n");
}

/* Private Helper Functions for OpenCL. */
void vector_operate1(clut_runtime runtime,
                     int *a,
                     int length,
                     const char *kernel_function)
{
  cl_mem _a = clu_create_buffer(runtime.context, CL_MEM_READ_WRITE,
                                length * sizeof(cl_int), NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, kernel_function);

  /* Set the kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_a);

  /* Launch the kernel. */
  size_t global_work_size[1] = { length };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 1, NULL,
                              global_work_size, NULL, 0, NULL, NULL);

  /* Wait for the program to finish. */
  clu_finish(runtime.command_queue);

  /* Copy the result back from the device. */
  clu_enqueue_read_buffer(runtime.command_queue, _a, CL_TRUE, 0,
                          length * sizeof(cl_int), a, 0, NULL, NULL);
}

void vector_operate3(clut_runtime runtime,
                     int *a,
                     int *b,
                     int *c,
                     int length,
                     const char *kernel_function)
{
  /* Copy the data to the device. */
  cl_mem _a = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                length * sizeof(cl_int), NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _a, CL_TRUE, 0,
                           length * sizeof(cl_int), a, 0, NULL, NULL);
  cl_mem _b = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                length * sizeof(cl_int), NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _b, CL_TRUE, 0,
                           length * sizeof(cl_int), b, 0, NULL, NULL);

  /* Create the destination vector. */
  cl_mem _c = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                length * sizeof(cl_int), NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, kernel_function);

  /* Set the kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_a);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_b);
  clu_set_kernel_arg(kernel, 2, sizeof(cl_mem), &_c);

  /* Launch the kernel. */
  size_t global_work_size[1] = { length };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 1, NULL,
                              global_work_size, NULL, 0, NULL, NULL);

  /* Wait for the program to finish. */
  clu_finish(runtime.command_queue);

  /* Copy the result back from the device. */
  clu_enqueue_read_buffer(runtime.command_queue, _c, CL_TRUE, 0,
                          length * sizeof(cl_int), c, 0, NULL, NULL);
}

/* Arithmetic
 * ---------- */

void vector_add(int *a,
                int *b,
                int *c,
                int length)
{
  for (int i = 0; i < length; i++) {
    c[i] = a[i] + b[i];
  }
}

void vector_add_parallel(clut_runtime runtime,
                         int *a,
                         int *b,
                         int *c,
                         int length)
{
  vector_operate3(runtime, a, b, c, length, "vector_add");
}

void vector_sqrt_ab(int *a,
                    int *b,
                    int *c,
                    int length)
{
  for (int i = 0; i < length; i++) {
    c[i] = (int) sqrt((float) (a[i] * b[i]));
  }
}

void vector_sqrt_ab_parallel(clut_runtime runtime,
                             int *a,
                             int *b,
                             int *c,
                             int length)
{
  vector_operate3(runtime, a, b, c, length, "vector_sqrt_ab");
}

void vector_add_sub(int *a,
                    int *b,
                    int *c,
                    int length)
{
  for (int i = 0; i < length; i++) {
    if (i < 32) {
      c[i] = a[i] + b[i];
    } else {
      c[i] = a[i] - b[i];
    }
  }
}

void vector_add_sub_parallel(clut_runtime runtime,
                             int *a,
                             int *b,
                             int *c,
                             int length)
{
  vector_operate3(runtime, a, b, c, length, "vector_add_sub");
}

void vector_set(int *a,
                int length)
{
  /* This could obviously be optimized. */
  for (int i = 0; i < length; i++) {
    if (i < 32) {
      a[i] = -i;
    } else {
      int x = 0;
      for (int j = 0; j < i; ++j) {
        x++;
      }
      a[i] = x;
    }
  }
}

void vector_set_parallel(clut_runtime runtime,
                         int *a,
                         int length)
{
  vector_operate1(runtime, a, length, "vector_set");
}
