#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <sys/syscall.h>
#include <unistd.h>

#include "common/vector.h"
#include "common/matrix.h"
#include "common/image.h"
#include "common/clu.h"

#define SHOULD_PARALLEL(flags) ((flags >> 2) & 1)
#define SHOULD_TEST(flags) ((flags >> 1) & 1)
#define SHOULD_MEASURE(flags) (flags & 1)

void vector_add_command(clut_runtime runtime,
                        int length,
                        unsigned char flags);

void vector_sqrt_ab_command(clut_runtime runtime,
                            int length,
                            unsigned char flags);

void vector_add_sub_command(clut_runtime runtime,
                            int length,
                            unsigned char flags);

void vector_set_command(clut_runtime runtime,
                        int length,
                        unsigned char flags);

void matrix_transpose_command(clut_runtime runtime,
                              int rows,
                              int columns,
                              unsigned char flags);

void matrix_add_command(clut_runtime runtime,
                        int rows,
                        int columns,
                        unsigned char flags);

void matrix_multiply_v1_command(clut_runtime runtime,
                                int a_rows,
                                int b_columns,
                                int a_columns_b_rows,
                                unsigned char flags);

void matrix_multiply_v2_command(clut_runtime runtime,
                                int a_rows,
                                int b_columns,
                                int a_columns_b_rows,
                                unsigned char flags);

void search_file_command(clut_runtime runtime,
                         const char *search_string,
                         const char *file,
                         unsigned char flags);

void image_rotate_command(clut_runtime runtime,
                          const char *input_file,
                          const char *output_file,
                          float rotation_degrees,
                          unsigned char flags);

void image_filter_command(clut_runtime runtime,
                          const char *input_file,
                          const char *output_file,
                          unsigned char flags);

void is_native_command(void);

#endif
