#ifndef __IMAGE_H__
#define __IMAGE_H__

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#include "clu.h"

void error(char *msg);
unsigned char *read_jpeg_file(const char *filename,
                              int *width,
                              int *height);
void write_jpeg_file(const char *filename,
                     unsigned char *image,
                     int width,
                     int height);

void image_rotate(const char *input_file,
                  const char *output_file,
                  float rotation_degrees);
void image_rotate_parallel(clut_runtime runtime,
                           const char *input_file,
                           const char *output_file,
                           float rotation_degrees);

void image_filter(const char *input_file,
                  const char *output_file);
void image_filter_parallel(clut_runtime runtime,
                           const char *input_file,
                           const char *output_file);

#endif
