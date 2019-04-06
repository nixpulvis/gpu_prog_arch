#include "../commands.h"

int search_text(const char *search_string,
                const char *text)
{
  int occurrences = 0;
  const char *tmp = text;
  while((tmp = strstr(tmp, search_string))) {
    occurrences++;
    tmp++;
  }

  return occurrences;
}

int search_text_parallel(clut_runtime runtime,
                         const char *search_string,
                         const char *text)
{
  size_t search_string_length = strlen(search_string);
  size_t text_length = strlen(text);

  /* Copy data over to the device. */
  cl_mem _text = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                   text_length * sizeof(cl_char), NULL);
  cl_mem _search_string = clu_create_buffer(runtime.context, CL_MEM_READ_ONLY,
                                            search_string_length *
                                            sizeof(cl_char), NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _text, CL_TRUE, 0,
                           text_length * sizeof(cl_char), text, 0, NULL, NULL);
  clu_enqueue_write_buffer(runtime.command_queue, _search_string, CL_TRUE, 0,
                           search_string_length * sizeof(cl_char),
                           search_string, 0, NULL, NULL);

  /* Create device destination buffer. */
  size_t search_results_length = text_length - search_string_length + 1;
  cl_mem _search_results = clu_create_buffer(runtime.context, CL_MEM_WRITE_ONLY,
                                             search_results_length *
                                             sizeof(cl_int), NULL);

  /* Create the search_file kernel. */
  cl_kernel search_kernel = clu_create_kernel(runtime.program, "search_file");

  /* Get work group size limits. */
  size_t max_work_group_size;
  clu_get_kernel_work_group_info(search_kernel, runtime.device,
                                 CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t),
                                 &max_work_group_size, NULL);

  /* Set the size of our work groups. */
  size_t search_local_work_size = max_work_group_size;

  /* Guard for search_strings that are too long. */
  if (search_string_length > search_local_work_size) {
    printf("error: search string is too long.\n");
  }

  /* This is the number of work groups that will be needed
   * to complete this calculation, it makes as few as possible
   * to utilize work items inside the work groups optimally.
   */
  size_t search_work_group_count = 1 + (text_length - 1) / (search_local_work_size - (search_string_length - 1));

  /* This is the global work size. It is simply the local_size *
   * the number of work groups.
   */
  size_t search_global_work_size = search_local_work_size * search_work_group_count;

  /* Search string arguments. */
  clu_set_kernel_arg(search_kernel, 0, sizeof(cl_mem), &_search_string);
  clu_set_kernel_arg(search_kernel, 1, search_string_length * sizeof(char), NULL);
  clu_set_kernel_arg(search_kernel, 2, sizeof(cl_ulong), &search_string_length);

  /* Text arguments. */
  clu_set_kernel_arg(search_kernel, 3, sizeof(cl_mem), &_text);
  clu_set_kernel_arg(search_kernel, 4, search_local_work_size * sizeof(char), NULL);
  clu_set_kernel_arg(search_kernel, 5, sizeof(cl_ulong), &text_length);

  /* Result arguments. */
  clu_set_kernel_arg(search_kernel, 6, sizeof(cl_mem), &_search_results);

  /* Launch the kernel. */
  clu_enqueue_nd_range_kernel(runtime.command_queue, search_kernel, 1, NULL,
                              &search_global_work_size, &search_local_work_size,
                              0, NULL, NULL);

  /* Wait for it to finish. */
  clu_finish(runtime.command_queue);

  /* --- Count the matches --- */

  /* Create the sum kernel. */
  cl_kernel sum_kernel = clu_create_kernel(runtime.program, "sum");

  /* Calculate device sizes. */
  size_t sum_local_work_size = max_work_group_size;
  size_t sum_work_group_count = 1 + ((search_results_length - 1) / sum_local_work_size);
  size_t sum_global_work_size = sum_local_work_size * sum_work_group_count;

  cl_ulong gstride;
  for (gstride = 1; (size_t) gstride < search_results_length; gstride *= sum_local_work_size) {
    /* TODO: Decrease the global size each iteration. */
    clu_set_kernel_arg(sum_kernel, 0, sizeof(cl_mem), &_search_results);
    clu_set_kernel_arg(sum_kernel, 1, sizeof(cl_ulong), &gstride);
    clu_set_kernel_arg(sum_kernel, 2, sum_local_work_size * sizeof(cl_int), NULL);

    clu_enqueue_nd_range_kernel(runtime.command_queue, sum_kernel, 1, NULL,
                                &sum_global_work_size, &sum_local_work_size,
                                0, NULL, NULL);

    clu_finish(runtime.command_queue);
  }

  int occurrences;
  clu_enqueue_read_buffer(runtime.command_queue, _search_results,
                          CL_TRUE, 0, sizeof(cl_int), &occurrences,
                          0, NULL, NULL);

  return occurrences;
}

void search_file_command(clut_runtime runtime,
                         const char *search_string,
                         const char *file,
                         unsigned char flags)
{
  /* Setup the data. */
  size_t text_length;
  const char *text = clut_file_read(file, &text_length);
  int occurrences;

  int start, end;
  if (SHOULD_MEASURE(flags)) start = clock();

  if (SHOULD_PARALLEL(flags)) {
    occurrences = search_text_parallel(runtime, search_string, text);
  } else {
    occurrences = search_text(search_string, text);
  }

  if (SHOULD_MEASURE(flags)) end = clock();

  if (SHOULD_MEASURE(flags)) {
    printf("search file ran in: %f seconds\n",
      ((double) (end - start)) / CLOCKS_PER_SEC);
  } else if (SHOULD_TEST(flags)) {
    /* Run the other implementation. */
    int compare;
    if (SHOULD_PARALLEL(flags)) {
      compare = search_text(search_string, text);
    } else {
      compare = search_text_parallel(runtime, search_string, text);
    }

    if (occurrences == compare) {
      printf("SUCCCESS\n");
    } else {
      printf("FAILED\n");
    }
  } else {
    printf("The string \"%s\" occurred %d time(s).\n",
           search_string, occurrences);
  }
}
