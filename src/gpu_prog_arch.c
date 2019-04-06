#include <getopt.h>

#include "commands.h"

/* Flag set by '--parallel'. */
static int parallel_flag;

/* Flag set by '--test'. */
static int test_flag;

/* Flag set by '--measure'. */
static int measure_flag;

#define DEVICE_TYPE CL_DEVICE_TYPE_GPU

void usage();
unsigned char flags();

int main(int argc,
         char **argv)
{
  static struct option long_options[] = {
    {"parallel",   no_argument, &parallel_flag,   'p'},
    {"test",       no_argument, &test_flag,       't'},
    {"measure",    no_argument, &measure_flag,    'm'},
    {0, 0, 0, 0}
  };
  int ch;
  while ((ch = getopt_long(argc, argv, "ptm", long_options, NULL)) != -1) {
    switch (ch) {
      case 'p':
        parallel_flag = 1;
        break;
      case 't':
        test_flag = 1;
        break;
      case 'm':
        measure_flag = 1;
        break;
    }
  }

  if (argc - optind < 1) {
    usage();
  }

  /* Determain the device depending on DEVICE_TYPE and
   * getenv(M2S_OPENCL_BINARY) */
  cl_device_type device_type = DEVICE_TYPE;
  char *m2s_binary = getenv("M2S_OPENCL_BINARY");
  if (m2s_binary != NULL) {
    device_type = CL_DEVICE_TYPE_GPU;
  }

  /* Setup the OpenCL runtime. */
  clut_runtime runtime;
  const char *kernel_files[1] = { "cl/gpu_prog_arch.cl" };
  clut_setup(&runtime, device_type, 1, kernel_files,
             CLUT_BUILD_WITH_SOURCE);

  char *command = argv[optind++];
  if (strcmp(command, "vector-add") == 0) {
    if (argc - optind == 1) {
      int length = atoi(argv[optind++]);
      vector_add_command(runtime, length, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "vector-sqrt-ab") == 0) {
    if (argc - optind == 1) {
      int length = atoi(argv[optind++]);
      vector_sqrt_ab_command(runtime, length, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "vector-add-sub") == 0) {
    if (argc - optind == 1) {
      int length = atoi(argv[optind++]);
      vector_add_sub_command(runtime, length, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "vector-set") == 0) {
    if (argc - optind == 1) {
      int length = atoi(argv[optind++]);
      vector_set_command(runtime, length, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "matrix-transpose") == 0) {
    if (argc - optind == 2) {
      int rows = atoi(argv[optind++]);
      int columns = atoi(argv[optind++]);
      matrix_transpose_command(runtime, rows, columns, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "matrix-add") == 0) {
    if (argc - optind == 2) {
      int rows = atoi(argv[optind++]);
      int columns = atoi(argv[optind++]);
      matrix_add_command(runtime, rows, columns, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "matrix-multiply-v1") == 0) {
    if (argc - optind == 3) {
      int a_rows = atoi(argv[optind++]);
      int b_columns = atoi(argv[optind++]);
      int a_columns_b_rows = atoi(argv[optind++]);
      matrix_multiply_v1_command(runtime, a_rows, b_columns, a_columns_b_rows, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "matrix-multiply-v2") == 0) {
    if (argc - optind == 3) {
      int a_rows = atoi(argv[optind++]);
      int b_columns = atoi(argv[optind++]);
      int a_columns_b_rows = atoi(argv[optind++]);
      matrix_multiply_v2_command(runtime, a_rows, b_columns, a_columns_b_rows, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "search-file") == 0) {
    if (argc - optind == 2) {
      const char *search_string = argv[optind++];
      const char *file = argv[optind++];
      search_file_command(runtime, search_string, file, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "image-rotate") == 0) {
    if (argc - optind == 3) {
      const char *input_file = argv[optind++];
      const char *output_file = argv[optind++];
      float rotation_degrees = atof(argv[optind++]);
      image_rotate_command(runtime, input_file, output_file, rotation_degrees, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "image-filter") == 0) {
    if (argc - optind == 2) {
      const char *input_file = argv[optind++];
      const char *output_file = argv[optind++];
      image_filter_command(runtime, input_file, output_file, flags());
    } else {
      usage();
    }
  } else if (strcmp(command, "is-native") == 0) {
    if (argc - optind == 0) {
      is_native_command();
    } else {
      usage();
    }
  } else {
    usage();
  }

  exit(0);
}

void usage()
{
  printf(
  "usage: gpu-prog-arch [--test] [--parallel] [--measure] <command> [<args>]\n\n"
  "Commands:\n"
  "    vector-add <length>\n"
  "    vector-sqrt-ab <length>\n"
  "    vector-add-sub <length>\n"
  "    vector-set <length>\n"
  "\n"
  "    matrix-transpose <rows> <columns>\n"
  "    matrix-add <rows> <columns>\n"
  "    matrix-multiply-v1 <a_rows> <b_columns> <a_columns_b_rows>\n"
  "    matrix-multiply-v2 <a_rows> <b_columns> <a_columns_b_rows>\n"
  "\n"
  "    search-file <search_string> <file>\n"
  "\n"
  "    image-rotate <input_file> <output_file> <rotation_degrees>\n"
  "    image-filter <input_file> <output_file>\n"
  "\n"
  "    is-native\n"
  );
  exit(1);
}

/* Flags are packed in the following order.
 * 0000
 * _PTM */
unsigned char flags()
{
  unsigned char f = 0;
  f |= (parallel_flag << 2);
  f |= (test_flag << 1);
  f |= measure_flag;
  return f;
}
