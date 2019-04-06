#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/clu.h"

int main(int argc, char const **argv) {

  const char *file_path;
  if (argc == 2) {
    file_path = argv[1];
  } else {
    printf("usage: opencl-compiler <FILE>\n");
    exit(1);
  }

  clut_runtime runtime;
  const char *kernel_files[1] = { file_path };
  clut_setup(&runtime, CL_DEVICE_TYPE_GPU, 1, kernel_files,
             CLUT_BUILD_WITH_SOURCE);

  size_t binary_size;
  clu_get_program_info(runtime.program, CL_PROGRAM_BINARY_SIZES,
                       sizeof(binary_size), &binary_size, NULL);

  unsigned char *binary = malloc(binary_size);
  clu_get_program_info(runtime.program, CL_PROGRAM_BINARIES,
                       sizeof(binary), &binary, NULL);

  char file_name[255];
  strcpy(file_name, file_path);
  char *dot = strrchr(file_name, '.');
  strcpy(dot, ".bin\0");
  clut_file_write(file_name, (char *) binary, binary_size);

  return 0;
}
