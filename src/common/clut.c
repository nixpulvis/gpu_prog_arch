#include "clu.h"

/* OpenCL Helper Functions. */

void clut_setup(clut_runtime *runtime,
                cl_device_type device_type,
                cl_uint kernel_files_count,
                const char **kernel_files,
                int build_mode)
{
  clu_get_platform_ids(1, &runtime->platform, NULL);
  clu_get_device_ids(runtime->platform, device_type, 1,
                     &runtime->device, NULL);
  runtime->context = clu_create_context(NULL, 1, &runtime->device, NULL, NULL);
  runtime->command_queue = clu_create_command_queue(runtime->context,
                                                    runtime->device, 0);

  const char **strings = malloc(kernel_files_count * sizeof(char *));
  const size_t *lengths = malloc(kernel_files_count * sizeof(size_t));
  for (cl_uint i = 0; i < kernel_files_count; i++) {
    strings[i] = clut_file_read(kernel_files[i], (size_t *) &lengths[i]);
    if (strings[i] == NULL) {
      printf("error: could not read file %s\n", kernel_files[i]);
      exit(1);
    }
  }

  if (build_mode == CLUT_BUILD_WITH_SOURCE) {
    runtime->program = clu_create_program_with_source(runtime->context,
                                                      kernel_files_count,
                                                      strings, lengths);
  } else if (build_mode == CLUT_BUILD_WITH_BINARY) {
    runtime->program = clu_create_program_with_binary(runtime->context,
                                                      1, &runtime->device,
                                                      lengths,
                                                      (const unsigned char **) strings,
                                                      NULL);

  } else {
    printf("error: invalid build mode in clut_setup");
    exit(1);
  }

  clu_build_program(runtime->program, 1, &runtime->device, "-Werror",
                    NULL, NULL);
}

/* General Helper Functions. */

const char *clut_file_read(const char *file_name, size_t *size) {
  FILE *file;
  char *buffer;

  file = fopen(file_name, "rb");
  if (!file) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  *size = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = malloc(*size + 1);
  fread(buffer, 1, *size, file);
  buffer[*size] = '\0';

  return buffer;
}

int clut_file_write(const char *file_name, char *buffer, size_t size) {
  FILE *file;

  file = fopen(file_name, "wb");
  if (!file) {
    return 0;
  }

  fwrite(buffer, 1, size, file);
  fclose(file);
  return 1;
}
