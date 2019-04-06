#include "image.h"

#define BYTES_PER_PIXEL 3;
#define COLOR_SPACE JCS_RGB;

void error(char *msg)
{
  fprintf(stderr, "error: %s\n", msg);
  exit(1);
}

unsigned char *read_jpeg_file(const char *filename,
                              int *width,
                              int *height)
{
  unsigned char *image;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];

  unsigned long location = 0;

  FILE *infile = fopen(filename, "rb");
  if (!infile) {
    error("Error opening jpeg file");
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);

  *width = cinfo.image_width;
  *height = cinfo.image_height;

  jpeg_start_decompress(&cinfo);
  image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
  row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);

  while (cinfo.output_scanline < cinfo.image_height)
  {
    jpeg_read_scanlines(&cinfo, row_pointer, 1);
    for (unsigned int i = 0; i < cinfo.image_width * cinfo.num_components; i++)
    image[location++] = row_pointer[0][i];
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo); free(row_pointer[0]);
  fclose(infile);

  return image;
}

void write_jpeg_file(const char *filename,
                     unsigned char *image,
                     int width,
                     int height)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];

  FILE *outfile = fopen(filename, "wb");
  if (!outfile) {
    error("Error opening output jpeg file");
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = BYTES_PER_PIXEL;
  cinfo.in_color_space = COLOR_SPACE;

  jpeg_set_defaults( &cinfo );
  jpeg_start_compress( &cinfo, TRUE );

  while (cinfo.next_scanline < cinfo.image_height)
  {
    cinfo.image_width = width;
    row_pointer[0] = &image[cinfo.next_scanline *
    cinfo.image_width *  cinfo.input_components];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  fclose(outfile);
}

void image_rotate(const char *input_file,
                  const char *output_file,
                  float rotation_degrees)
{
  unsigned char *image;
  unsigned char *out_image;
  double sin_angle;
  double cos_angle;
  int width, height;

  int x;
  int y;
  int center_x;
  int center_y;

  /* Convert to radians by multiplying by PI and
   * dividing by 180. */
  double angle = rotation_degrees * M_PI / 180.0;

  /* Read image from file.
   * This initializes 'image', 'width', and 'height'. */
  image = read_jpeg_file(input_file, &width, &height);

  /* Allocate space for a new output image */
  out_image = calloc(width * height, 3);

  /* Rotate. The loops below walk the input image in
   * every coordinate. */
  center_x = width / 2;
  center_y = height / 2;
  sin_angle = sin(angle);
  cos_angle = cos(angle);
  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      int out_x;
      int out_y;

      /* Get the corresponding coordinates from
       * the input image. */
      out_x = (double) x * cos_angle + (double) y * sin_angle;
      out_y = (double) x * sin_angle + (double) y * cos_angle;

      /* If input pixel is out of the image, discard. */
      if (out_x < 0 || out_x >= width) {
        continue;
      }
      if (out_y < 0 || out_y >= height) {
        continue;
      }

      /* Copy pixel (R, G, B components) */
      out_image[(out_y * width + out_x) * 3] = image[(y * width + x) * 3];
      out_image[(out_y * width + out_x) * 3 + 1] = image[(y * width + x) * 3 + 1];
      out_image[(out_y * width + out_x) * 3 + 2] = image[(y * width + x) * 3 + 2];
    }
  }

  /* Write new image to file 'out.jpg' */
  write_jpeg_file(output_file, out_image, width, height);
}

void image_rotate_parallel(clut_runtime runtime,
                           const char *input_file,
                           const char *output_file,
                           float rotation_degrees)
{
  /* Convert to radians. */
  cl_float rotation_radians = rotation_degrees * M_PI / 180.0;

  /* Read the image. */
  cl_int width, height;
  unsigned char *input_image = read_jpeg_file(input_file, &width, &height);
  cl_int image_size = width * height;

  /* Copy the data to the device. */
  cl_mem _input_image = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                          image_size * 3, NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _input_image, CL_TRUE, 0,
                           image_size * 3, input_image, 0, NULL, NULL);

  /* Setup the output buffer. */
  unsigned char *output_image = malloc(image_size * 3);
  cl_mem _output_image = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                           image_size * 3, NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, "image_rotate");

  /* Set the kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_input_image);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_output_image);
  clu_set_kernel_arg(kernel, 2, sizeof(cl_float), &rotation_radians);
  clu_set_kernel_arg(kernel, 3, sizeof(cl_int), &width);
  clu_set_kernel_arg(kernel, 4, sizeof(cl_int), &height);

  /* Run the kernel. */
  size_t global_work_size[2] = { height, width };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 2, NULL,
                              global_work_size, NULL, 0, NULL, NULL);

  /* Read the rotated image from the device. */
  clu_enqueue_read_buffer(runtime.command_queue, _output_image, CL_TRUE, 0,
                          image_size * 3, output_image, 0, NULL, NULL);

  /* Write the rotated image to file. */
  write_jpeg_file(output_file, output_image, width, height);
}

void image_filter(__attribute__ ((unused)) const char *input_file,
                  __attribute__ ((unused)) const char *output_file)
{
  printf("error: 'image_filter' is not implemented\n");
}

void image_filter_parallel(clut_runtime runtime,
                           const char *input_file,
                           const char *output_file)
{
  /* Setup data. */
  cl_int width, height;
  unsigned char *input_image = read_jpeg_file(input_file, &width, &height);
  cl_int image_size = width * height;

  /* Blur mask */
  int mask_width = 3;
  int mask_height = 3;
  int mask_size = mask_width * mask_height;
  static const float mask[9] = {
    0.111111, 0.111111, 0.111111,
    0.111111, 0.111111, 0.111111,
    0.111111, 0.111111, 0.111111
  };

  if (mask_width % 2 == 0 || mask_height % 2 == 0) {
    printf("error: mask must have odd sized dimensions.\n");
    exit(1);
  }

  /* Copy image and mask to the device. */
  cl_mem _input_image = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                    image_size * 3, NULL);
  cl_mem _mask = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                   mask_size * sizeof(cl_float), NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _input_image,
                           CL_TRUE, 0, image_size * 3,
                           input_image, 0, NULL, NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _mask,
                           CL_TRUE, 0, mask_size * sizeof(cl_float),
                           mask, 0, NULL, NULL);

  /* Create the output image buffer. */
  unsigned char *output_image = malloc(image_size * 3);
  cl_mem _output_image = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                           image_size * 3, NULL);

  /* Create the kernel. */
  cl_kernel kernel = clu_create_kernel(runtime.program, "image_filter");

  /* Set the kernel arguments. */
  clu_set_kernel_arg(kernel, 0, sizeof(cl_mem), &_input_image);
  clu_set_kernel_arg(kernel, 1, sizeof(cl_mem), &_output_image);
  clu_set_kernel_arg(kernel, 2, sizeof(cl_mem), &_mask);
  clu_set_kernel_arg(kernel, 3, sizeof(cl_int), &mask_width);
  clu_set_kernel_arg(kernel, 4, sizeof(cl_int), &mask_height);

  /* Launch the kernel. */
  size_t global_work_size[2] = { height, width };
  clu_enqueue_nd_range_kernel(runtime.command_queue, kernel, 2, 0,
                              global_work_size, NULL, 0, NULL, NULL);

  /* Wait for the program to finish. */
  clu_finish(runtime.command_queue);

  clu_enqueue_read_buffer(runtime.command_queue, _output_image, CL_TRUE,
                          0, image_size * 3, output_image, 0, NULL, NULL);

  write_jpeg_file(output_file, output_image, width, height);
}
