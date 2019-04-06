#define MATRIX_INDEX(row, column, columns) (row * columns + column)

__kernel void vector_add(__global int *a,
                         __global int *b,
                         __global int *c)
{
  uint i = get_global_id(0);
  c[i] = a[i] + b[i];
}

__kernel void vector_sqrt_ab(__global int *a,
                             __global int *b,
                             __global int *c)
{
  uint i = get_global_id(0);
  c[i] = (int) sqrt((float) (a[i] * b[i]));
}

__kernel void vector_add_sub(__global int *a,
                             __global int *b,
                             __global int *c)
{
  int i = get_global_id(0);
  if (i < 32) {
    c[i] = a[i] + b[i];
  } else {
    c[i] = a[i] - b[i];
  }
}

__kernel void vector_set(__global int *a)
{
  int i = get_global_id(0);
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

__kernel void matrix_transpose(__global int *src,
                               __global int *dst)
{

  uint column  = get_global_id(0);
  uint row     = get_global_id(1);
  uint columns = get_global_size(0);
  uint rows    = get_global_size(1);

  dst[MATRIX_INDEX(column, row, rows)] =
    src[MATRIX_INDEX(row, column, columns)];
}

__kernel void matrix_add(__global int *src1,
                         __global int *src2,
                         __global int *dst)
{

  uint column  = get_global_id(0);
  uint row     = get_global_id(1);
  uint columns = get_global_size(0);

  dst[MATRIX_INDEX(row, column, columns)] =
    src1[MATRIX_INDEX(row, column, columns)] +
    src2[MATRIX_INDEX(row, column, columns)];
}

__kernel void matrix_multiply_v1(__global int *a,
                                 __global int *b,
                                 __global int *c,
                                 int a_columns_b_rows)
{

  uint col = get_global_id(0);
  uint row = get_global_id(1);
  uint b_columns = get_global_size(0);

  int sum = 0;
  for (int k = 0; k < a_columns_b_rows; k++) {
    sum += a[MATRIX_INDEX(row, k, a_columns_b_rows)] * b[MATRIX_INDEX(k, col, b_columns)];
  }
  c[MATRIX_INDEX(row, col, b_columns)] = sum;
}

__kernel void matrix_multiply_v2(__global int *a,
                                 __global int *b,
                                 __global int *c,
                                 __local int *la,
                                 __local int *lb,
                                 int a_columns_b_rows)
{
// matrices into blocks of size lsize*lsize
//   Algorithm to calculate block[r][c] of C
//     for i = 0 to num_block-1
//     copy block[r][i] of A into lA
//     copy block[i][c] of B into lB
//     multiply lA x lB and save temp result
  int lcol = get_local_id(0);
  int lrow = get_local_id(1);
  int lsize = get_local_size(0);

  int col = get_global_id(0);
  int row = get_global_id(1);
  int size = get_global_size(0);

  int sum = 0;
  int i,j;
  for (i = 0; i < size / lsize; i++) {

    // allocate local memory
    la[lrow * lsize + lcol] = a[row * size + i * lsize + lcol];
    lb[lrow * lsize + lcol] = b[(i * lsize + lrow) * size + lcol];

    // wait
    barrier(CLK_LOCAL_MEM_FENCE);

    for ( j = 0; j < lsize; j++) {
      // multiply and sum
      sum += la[lrow * lsize + j] * lb[j * lsize + lcol];
    }
    // wait
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  // allocate output aray
  c[row * size + col] = sum;
}

/* search_file: char*, char*, ulong, char*, char*, ulong, char*
 *
 * search_string:
 *   The string which we will look for in `text`. Not assumed
 *   to be NULL terminated, the length is passed explicitly.
 * local_search_string:
 *   Just a local copy of the search string, for performance.
 * search_string_length:
 *   The length of the search string. This is the number of
 *   characters the `search_string` pointer should allocate.
 * text:
 *   The full text in global memory. This will not be where each
 *   work item will look in. The work items will move parts of
 *   this into local memory. This string is not assumed to be
 *   NULL terminated, it will have it's length passed directly.
 * local_text:
 *   This is the local memory where each work item in a work
 *   group will copy part of the full text to, and then operate on.
 *   It's length cannot be assumed to be the local size. This is
 *   because the last work group might have a smaller string due
 *   to the full text not being divisible by the number of work
 *   groups evenly. To calculate the size of each work groups local
 *   text, first find the `offset` into the string which the work group
 *   will be operating on, then subtract that from the `text_length`.
 *   local_text_length = min(local_size, offset - text_length)
 * text_length:
 *   The length of the text. This is not the same as the global
 *   size due to the global size needing to cover the overlap in
 *   the work groups.
 * search_results:
 *   This is the output array. It's stored as an array of chars, but
 *   in reality it's an array of 0s and 1s. The length of this array
 *   is text_length - (search_length - 1). A 1 signifies an occurrence
 *   of search_string in text starting at that index.
 */
__kernel void search_file(__global char *search_string,
                          __local char *local_search_string,
                          ulong search_string_length,
                          __global char *text,
                          __local char *local_text,
                          ulong text_length,
                          __global int *search_results)
{
  /* global_size:
   *   The total size of all the work items, this will not be
   *   equal to the text size unless we are working with a
   *   problem that fits in one work group.
   * global_id:
   *   This will not directly map 1-1 with the text because
   *   of work group overlap.
   * group_id:
   *   The work group index. This will help calculate padding
   *   offsets.
   * local_size:
   *   The number of work items in the work group. This will
   *   only be equal to the size of the local_text if there is
   *   only one work group.
   * local_index:
   *   The offset into the local_text. This IS a 1-1 mapping to
   *   the local_text, with the stop condition on reading based on
   *   the calculated `local_text_length`.
   */
  ulong local_size  = get_local_size(0);
  ulong global_id   = get_global_id(0);
  ulong local_index = get_local_id(0);
  ulong group_id    = global_id / local_size;

  /* global_index:
   *   This is the index into the global text for this work item.
   *   Notice that some work items will overlap as intended. Use
   *   this index for reads to `text` and writes to `search_results`.
   */
  ulong global_index = local_index + (group_id * (local_size - (search_string_length - 1)));

  /* local_text_length:
   *   This is the length of local_text we are to copy into local
   *   memory and search for a matching string. Use this to determine
   *   looping stop conditions for the `local_text`.
   */
  ulong local_text_length = min(local_size, text_length - local_size * group_id);

  /* local_search_length:
   *   The length of the string in the local_text which might
   *   be able to have a match.
   */
  ulong local_search_length = local_text_length - (search_string_length - 1);

  /* Copy this work items piece of the `search_string` into
   * local memory. */
  if (local_index < search_string_length) {
    local_search_string[local_index] = search_string[local_index];
  }

  /* Copy this work items piece of the `text` into `local_text` */
  if (local_index < local_text_length) {
    local_text[local_index] = text[global_index];
  }

  /* Wait for all work items to move their piece of `text`. */
  barrier(CLK_LOCAL_MEM_FENCE);

  /* Look for the string. The workers in the padding area do
   * nothing. */
  if (local_index < local_search_length) {
    char found = 1;
    for (ulong i = 0; i < search_string_length; i++) {
      if (local_search_string[i] != local_text[local_index + i]) {
        found = 0;
        break;
      }
    }
    /* Write findings to global memory */
    search_results[global_index] = found;
  }
}

__kernel void sum(__global int *v,
                  ulong gstride,
                  __local int *lv)
{
  int lid = get_local_id(0);
  int gid = get_global_id(0);
  int lsize = get_local_size(0);
  int lstride;

  lv[lid] = v[gid * gstride];
  barrier(CLK_LOCAL_MEM_FENCE);

  for (lstride = 1; lstride < lsize; lstride <<= 1) {
    if ((lid + lstride) < lsize) {
      lv[lid] = lv[lid] + lv[lid + lstride];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  if(!lid) {
    v[gid * gstride] = lv[0];
  }

}

__kernel void image_rotate(__global unsigned char *image,
                           __global unsigned char *out_image,
                           float angle,
                           int width,
                           int height)
{

  float sin_angle = sin(angle);
  float cos_angle = cos(angle);

  int y = get_global_id(0);
  int x = get_global_id(1);

  int out_x, out_y;

  out_x = (float) x * cos_angle + (float) y * sin_angle;
  out_y = (float) x * sin_angle + (float) y * cos_angle;

  if ((out_x >= 0 && out_x < width) && (out_y >= 0 && out_y < height)) {
    out_image[(out_y * width + out_x) * 3] = image[(y * width + x) * 3];
    out_image[(out_y * width + out_x) * 3 + 1] = image[(y * width + x) * 3 + 1];
    out_image[(out_y * width + out_x) * 3 + 2] = image[(y * width + x) * 3 + 2];
  }
}

__kernel void image_filter(__global unsigned char *image,
                           __global unsigned char *out_image,
                           __global const float *mask,
                           int mask_width,
                           int mask_height)
{
  int x = get_global_id(1);
  int y = get_global_id(0);
  int width = get_global_size(1);
  int height = get_global_size(0);

  float sum_r = 0;
  float sum_g = 0;
  float sum_b = 0;

  int x_travel, y_travel;
  float mask_val;

  for (int i = 0; i < mask_width; i++) {
    for (int j = 0; j < mask_height; j++) {

      y_travel = j - (int) (mask_height / 2);
      x_travel = i - (int) (mask_width / 2);
      mask_val = mask[j * mask_width + i];

      if (x + x_travel < width && x + x_travel >= 0 && y + y_travel < height && y + y_travel >= 0) {
        sum_r += (image[((y + y_travel) * width + (x + x_travel)) * 3]     * mask_val);
        sum_g += (image[((y + y_travel) * width + (x + x_travel)) * 3 + 1] * mask_val);
        sum_b += (image[((y + y_travel) * width + (x + x_travel)) * 3 + 2] * mask_val);
      }
    }
  }

  out_image[(y * width + x) * 3] = (unsigned char) (sum_r);
  out_image[(y * width + x) * 3 + 1] = (unsigned char) (sum_g);
  out_image[(y * width + x) * 3 + 2] = (unsigned char) (sum_b);
}
