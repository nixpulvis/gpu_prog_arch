#include "matrix.h"

int *matrix_allocate(int rows,
                     int columns)
{
  return (int *) calloc(rows * columns, sizeof(int));
}

int matrix_equal(int *a,
                 int *b,
                 int rows,
                 int columns)
{
  for (int i = 0; i < rows * columns; i++) {
    if (a[i] != b[i]) {
      return 0;
    }
  }
  return 1;
}

void matrix_println(int *matrix,
                    int rows,
                    int columns)
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      printf("%d\t", matrix[MATRIX_INDEX(i, j, columns)]);
    }
    printf("\n");
  }
}

/* Arithmetic
 * ---------- */

void matrix_transpose(int *a,
                      int *b,
                      int rows,
                      int columns)
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      b[MATRIX_INDEX(j, i, rows)] = a[MATRIX_INDEX(i, j, columns)];
    }
  }
}

void matrix_transpose_parallel(clut_runtime runtime,
                               int *a,
                               int *b,
                               int rows,
                               int columns)
{
  /* Copy the data to the device. */
  cl_mem _a = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                rows * columns * sizeof(cl_int), NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _a, CL_TRUE, 0,
                           rows * columns * sizeof(cl_int), a, 0, NULL, NULL);

  /* Create the destination matrix. */
  cl_mem _b = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                rows * columns * sizeof(cl_int), NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, "matrix_transpose");

  /* Set the kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_a);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_b);

  /* Launch the kernel. */
  size_t global_work_size[2] = { columns, rows };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 2, NULL,
                              global_work_size, NULL, 0, NULL, NULL);

  /* Wait for the program to finish. */
  clu_finish(runtime.command_queue);

  /* Copy the result back from the device. */
  clu_enqueue_read_buffer(runtime.command_queue, _b, CL_TRUE, 0,
                          rows * columns * sizeof(cl_int), b, 0, NULL, NULL);
}

void matrix_add(int *a,
                int *b,
                int *c,
                int rows,
                int columns)
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      c[MATRIX_INDEX(i, j, columns)] =
        a[MATRIX_INDEX(i, j, columns)] +
        b[MATRIX_INDEX(i, j, columns)];
    }
  }
}

void matrix_add_parallel(clut_runtime runtime,
                         int *a,
                         int *b,
                         int *c,
                         int rows,
                         int columns)
{
  /* Copy the data to the device. */
  cl_mem _a = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                rows * columns * sizeof(cl_int), NULL);
  cl_mem _b = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                rows * columns * sizeof(cl_int), NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _a, CL_TRUE, 0,
                           rows * columns * sizeof(cl_int), a, 0, NULL, NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _b, CL_TRUE, 0,
                           rows * columns * sizeof(cl_int), b, 0, NULL, NULL);

  /* Create the destination matrix. */
  cl_mem _c = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                rows * columns * sizeof(cl_int), NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, "matrix_add");

  /* Set the kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_a);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_b);
  clu_set_kernel_arg(kernel, 2, sizeof(cl_mem), &_c);

  /* Launch the kernel. */
  size_t global_work_size[2] = { columns, rows };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 2, NULL,
                              global_work_size, NULL, 0, NULL, NULL);

  /* Wait for the program to finish. */
  clu_finish(runtime.command_queue);

  /* Copy the result back from the device. */
  clu_enqueue_read_buffer(runtime.command_queue, _c, CL_TRUE, 0,
                          rows * columns * sizeof(cl_int), c, 0, NULL, NULL);
}

void matrix_multiply(int *a,
                     int *b,
                     int *c,
                     int a_rows,
                     int b_columns,
                     int a_columns_b_rows)
{
  int sum;
  for (int i = 0; i < a_rows; i++) {
    for (int j = 0; j < b_columns; j++) {
      sum = 0;
      for (int k = 0; k < a_columns_b_rows; k++) {
        sum += a[MATRIX_INDEX(i, k, a_columns_b_rows)] *
               b[MATRIX_INDEX(k, j, b_columns)];
      }
      c[MATRIX_INDEX(i, j, b_columns)] = sum;
    }
  }
}
void matrix_multiply_parallel_v1(clut_runtime runtime,
                                 int *a,
                                 int *b,
                                 int *c,
                                 int a_rows,
                                 int b_columns,
                                 int a_columns_b_rows)
{
  /* Copy over data to the device. */
  cl_mem _a = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                a_rows * a_columns_b_rows * sizeof(cl_int),
                                NULL);
  cl_mem _b = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                a_columns_b_rows * b_columns * sizeof(cl_int),
                                NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _a, CL_TRUE, 0,
                           a_rows * a_columns_b_rows * sizeof(cl_int), a,
                           0, NULL, NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _b, CL_TRUE, 0,
                           a_columns_b_rows * b_columns * sizeof(cl_int), b,
                           0, NULL, NULL);

  /* Create device destination buffer. */
  cl_mem _c = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                a_rows * b_columns * sizeof(cl_int),
                                NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, "matrix_multiply_v1");

  /* Set kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_a);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_b);
  clu_set_kernel_arg(kernel, 2, sizeof(cl_mem), &_c);
  clu_set_kernel_arg(kernel, 3, sizeof(int), &a_columns_b_rows);

  /* Launch the kernel. */
  const size_t global_work_size[2] = { b_columns, a_rows };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 2, NULL,
                              global_work_size, NULL,
                              0, NULL, NULL);

  /* Wait for it to finish. */
  clu_finish(runtime.command_queue);

  /* Read results from kernel. */
  clu_enqueue_read_buffer(runtime.command_queue, _c, CL_TRUE,
                          0, a_rows * b_columns * sizeof(cl_int),
                          c, 0, NULL, NULL);
}

void matrix_multiply_parallel_v2(clut_runtime runtime,
                                 int *a,
                                 int *b,
                                 int *c,
                                 int a_rows,
                                 int b_columns,
                                 int a_columns_b_rows)
{
  /* HACK: For now. */
  if (a_rows % 10 != 0 || b_columns % 10 != 0 || a_rows != b_columns) {
    printf("error: parallel matrix multiply assumes square matrices of dimensions divisible by 10\n");
    exit(1);
  }

  /* Copy over data to the device. */
  cl_mem _a = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                a_rows * a_columns_b_rows * sizeof(cl_int),
                                NULL);
  cl_mem _b = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                a_columns_b_rows * b_columns * sizeof(cl_int),
                                NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _a, CL_TRUE, 0,
                           a_rows * a_columns_b_rows * sizeof(cl_int), a,
                           0, NULL, NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _b, CL_TRUE, 0,
                           a_columns_b_rows * b_columns * sizeof(cl_int), b,
                           0, NULL, NULL);

  /* Create device destination buffer. */
  cl_mem _c = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                a_rows * b_columns * sizeof(cl_int),
                                NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, "matrix_multiply_v2");

  /* Get work group size limits. */
  size_t max_work_group_size;
  clu_get_kernel_work_group_info(kernel, runtime.device,
                                 CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t),
                                 &max_work_group_size, NULL);

  /* Make sure we can fit this thing. `sqrt` will floor here. */
  size_t sizes = sqrt(max_work_group_size);
  sizes -= (sizes % 10);

  if (a_rows < (int) sizes) {
    sizes = a_rows;
  }
  else {
    for (int i = sizes; i > 0; i--) {
      if (a_rows % i == 0) {
        sizes = i;
        break;
      }
    }
  }

  /* Set kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_a);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_b);
  clu_set_kernel_arg(kernel, 2, sizeof(cl_mem), &_c);
  clu_set_kernel_arg(kernel, 3, max_work_group_size * sizeof(cl_int), NULL);
  clu_set_kernel_arg(kernel, 4, max_work_group_size * sizeof(cl_int), NULL);
  clu_set_kernel_arg(kernel, 5, sizeof(int), &a_columns_b_rows);

  /* Launch the kernel. */
  const size_t global_work_size[2] = { b_columns, a_rows };
  const size_t local_work_size[2] = { sizes, sizes } ;
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 2, NULL,
                              global_work_size, local_work_size,
                              0, NULL, NULL);

  /* Wait for it to finish. */
  clu_finish(runtime.command_queue);

  /* Read results from kernel. */
  clu_enqueue_read_buffer(runtime.command_queue, _c, CL_TRUE,
                          0, a_rows * b_columns * sizeof(cl_int),
                          c, 0, NULL, NULL);
}
