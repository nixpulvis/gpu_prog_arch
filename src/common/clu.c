#include "clu.h"

/* OpenCL Wrapper Functions. */

void clu_get_platform_ids(cl_uint num_entries,
                          cl_platform_id *platforms,
                          cl_uint *num_platforms)
{
  cl_int ret = clGetPlatformIDs(num_entries, platforms, num_platforms);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_VALUE:
        printf("error: 'clu_get_platform_ids' failed with CL_INVALID_VALUE\n");
        break;
    }
    exit(1);
  }
}

void clu_get_device_ids(cl_platform_id platform,
                        cl_device_type device_type,
                        cl_uint num_entries,
                        cl_device_id *devices,
                        cl_uint *num_devices)
{
  cl_int ret = clGetDeviceIDs(platform, device_type, num_entries, devices,
                              num_devices);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_PLATFORM:
        printf("error: 'clu_get_device_ids' failed with CL_INVALID_PLATFORM\n");
        break;
      case CL_INVALID_DEVICE_TYPE:
        printf("error: 'clu_get_device_ids' failed with CL_INVALID_DEVICE_TYPE\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_get_device_ids' failed with CL_INVALID_VALUE\n");
        break;
      case CL_DEVICE_NOT_FOUND:
        printf("error: 'clu_get_device_ids' failed with CL_DEVICE_NOT_FOUND\n");
        break;
    }
    exit(1);
  }
}

cl_context clu_create_context(cl_context_properties *properties,
                              cl_uint num_devices,
                              const cl_device_id *devices,
                              void pfn_notify(const char *errinfo,
                                               const void *private_info,
                                               size_t cb,
                                               void *user_data),
                              void *user_data)
{
  cl_int ret;
  cl_context context = clCreateContext(properties, num_devices, devices,
                                       pfn_notify, user_data, &ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_PLATFORM:
        printf("error: 'clu_create_context' failed with CL_INVALID_PLATFORM\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_create_context' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_DEVICE:
        printf("error: 'clu_create_context' failed with CL_INVALID_DEVICE\n");
        break;
      case CL_DEVICE_NOT_AVAILABLE:
        printf("error: 'clu_create_context' failed with CL_DEVICE_NOT_AVAILABLE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_create_context' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }

  return context;
}

cl_command_queue clu_create_command_queue(cl_context context,
                                          cl_device_id device,
                                          cl_command_queue_properties properties)
{
  cl_int ret;
  cl_command_queue command_queue = clCreateCommandQueue(context, device,
                                                        properties, &ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_create_command_queue' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_DEVICE:
        printf("error: 'clu_create_command_queue' failed with CL_INVALID_DEVICE\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_create_command_queue' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_QUEUE_PROPERTIES:
        printf("error: 'clu_create_command_queue' failed with CL_INVALID_QUEUE_PROPERTIES\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_create_command_queue' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }

  return command_queue;
}

cl_program clu_create_program_with_source(cl_context context,
                                          cl_uint count,
                                          const char **strings,
                                          const size_t *lengths)
{
  cl_int ret;
  cl_program program = clCreateProgramWithSource(context, count, strings,
                                                 lengths, &ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_create_program_with_source' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_create_program_with_source' failed with CL_INVALID_VALUE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_create_program_with_source' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }

  return program;
}

cl_program clu_create_program_with_binary(cl_context context,
                                          cl_uint num_devices,
                                          const cl_device_id *device_list,
                                          const size_t *lengths,
                                          const unsigned char **binaries,
                                          cl_int *binary_status)
{
  cl_int ret;
  cl_program program = clCreateProgramWithBinary(context, num_devices,
                                                 device_list, lengths, binaries,
                                                 binary_status, &ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_create_program_with_binary' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_create_program_with_binary' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_DEVICE:
        printf("error: 'clu_create_program_with_binary' failed with CL_INVALID_DEVICE\n");
        break;
      case CL_INVALID_BINARY:
        printf("error: 'clu_create_program_with_binary' failed with CL_INVALID_BINARY\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_create_program_with_binary' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }

  return program;
}

void clu_build_program(cl_program program,
                       cl_uint num_devices,
                       const cl_device_id *device_list,
                       const char *options,
                       void (*pfn_notify) (cl_program, void *user_data),
                       void *user_data)
{
  cl_int ret = clBuildProgram(program, num_devices, device_list, options,
                              pfn_notify, user_data);
  for (cl_uint i = 0; i < num_devices; i++) {
    size_t size;
    clu_get_program_build_info(program, device_list[i], CL_PROGRAM_BUILD_LOG,
                               0, NULL, &size);
    char *build_log = malloc(size);
    clu_get_program_build_info(program, device_list[i], CL_PROGRAM_BUILD_LOG,
                               size, build_log, NULL);
    printf("%s", build_log);
    free(build_log);
  }

  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_PROGRAM:
        printf("error: 'clu_build_program' failed with CL_INVALID_PROGRAM\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_build_program' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_DEVICE:
        printf("error: 'clu_build_program' failed with CL_INVALID_DEVICE\n");
        break;
      case CL_INVALID_BINARY:
        printf("error: 'clu_build_program' failed with CL_INVALID_BINARY\n");
        break;
      case CL_INVALID_BUILD_OPTIONS:
        printf("error: 'clu_build_program' failed with CL_INVALID_BUILD_OPTIONS\n");
        break;
      case CL_INVALID_OPERATION:
        printf("error: 'clu_build_program' failed with CL_INVALID_OPERATION\n");
        break;
      case CL_COMPILER_NOT_AVAILABLE:
        printf("error: 'clu_build_program' failed with CL_COMPILER_NOT_AVAILABLE\n");
        break;
      case CL_BUILD_PROGRAM_FAILURE:
        printf("error: 'clu_build_program' failed with CL_BUILD_PROGRAM_FAILURE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_build_program' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }
}

void clu_get_program_info(cl_program program,
                          cl_program_info param_name,
                          size_t param_value_size,
                          void *param_value,
                          size_t *param_value_size_ret)
{
  cl_int ret = clGetProgramInfo(program, param_name, param_value_size,
                                param_value, param_value_size_ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_VALUE:
        printf("error: 'clu_get_program_info' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_PROGRAM:
        printf("error: 'clu_get_program_info' failed with CL_INVALID_PROGRAM\n");
        break;
    }
    exit(1);
  }
}

void clu_get_program_build_info(cl_program program,
                                cl_device_id device,
                                cl_program_build_info param_name,
                                size_t param_value_size,
                                void *param_value,
                                size_t *param_value_size_ret)
{
  cl_int ret = clGetProgramBuildInfo(program, device, param_name,
                                     param_value_size, param_value,
                                     param_value_size_ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_DEVICE:
        printf("error: 'clu_get_program_build_info' failed with CL_INVALID_DEVICE\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_get_program_build_info' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_PROGRAM:
        printf("error: 'clu_get_program_build_info' failed with CL_INVALID_PROGRAM\n");
        break;
    }
    exit(1);
  }
}

cl_kernel clu_create_kernel(cl_program program,
                            const char *kernel_name)
{
  cl_int ret;
  cl_kernel kernel = clCreateKernel(program, kernel_name, &ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_PROGRAM:
        printf("error: 'clu_create_kernel' failed with CL_INVALID_PROGRAM\n");
        break;
      case CL_INVALID_PROGRAM_EXECUTABLE:
        printf("error: 'clu_create_kernel' failed with CL_INVALID_PROGRAM_EXECUTABLE\n");
        break;
      case CL_INVALID_KERNEL_NAME:
        printf("error: 'clu_create_kernel' failed with CL_INVALID_KERNEL_NAME\n");
        break;
      case CL_INVALID_KERNEL_DEFINITION:
        printf("error: 'clu_create_kernel' failed with CL_INVALID_KERNEL_DEFINITION\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_create_kernel' failed with CL_INVALID_VALUE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_create_kernel' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }

  return kernel;
}

void clu_set_kernel_arg(cl_kernel kernel,
                        cl_uint arg_index,
                        size_t arg_size,
                        const void *arg_value)
{
  cl_int ret = clSetKernelArg(kernel, arg_index, arg_size, arg_value);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_KERNEL:
        printf("error: 'clu_set_kernel_arg' failed with CL_INVALID_KERNEL\n");
        break;
      case CL_INVALID_ARG_INDEX:
        printf("error: 'clu_set_kernel_arg' failed with CL_INVALID_ARG_INDEX\n");
        break;
      case CL_INVALID_ARG_VALUE:
        printf("error: 'clu_set_kernel_arg' failed with CL_INVALID_ARG_VALUE\n");
        break;
      case CL_INVALID_MEM_OBJECT:
        printf("error: 'clu_set_kernel_arg' failed with CL_INVALID_MEM_OBJECT\n");
        break;
      case CL_INVALID_SAMPLER:
        printf("error: 'clu_set_kernel_arg' failed with CL_INVALID_SAMPLER\n");
        break;
      case CL_INVALID_ARG_SIZE:
        printf("error: 'clu_set_kernel_arg' failed with CL_INVALID_ARG_SIZE\n");
        break;
    }
    exit(1);
  }
}

void clu_get_kernel_work_group_info(cl_kernel kernel,
                                    cl_device_id device,
                                    cl_kernel_work_group_info param_name,
                                    size_t param_value_size,
                                    void *param_value,
                                    size_t *param_value_size_ret)
{
  cl_int ret = clGetKernelWorkGroupInfo(kernel, device, param_name,
                                        param_value_size, param_value,
                                        param_value_size_ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_DEVICE:
        printf("error: 'clu_get_kernel_work_group_info' failed with CL_INVALID_DEVICE\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_get_kernel_work_group_info' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_KERNEL:
        printf("error: 'clu_get_kernel_work_group_info' failed with CL_INVALID_KERNEL\n");
        break;
    }
    exit(1);
  }
}

cl_mem clu_create_buffer(cl_context context,
                         cl_mem_flags flags,
                         size_t size,
                         void *host_ptr)
{
  cl_int ret;
  cl_mem buffer = clCreateBuffer(context, flags, size, host_ptr, &ret);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_create_buffer' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_create_buffer' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_BUFFER_SIZE:
        printf("error: 'clu_create_buffer' failed with CL_INVALID_BUFFER_SIZE\n");
        break;
      case CL_INVALID_HOST_PTR:
        printf("error: 'clu_create_buffer' failed with CL_INVALID_HOST_PTR\n");
        break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        printf("error: 'clu_create_buffer' failed with CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_create_buffer' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }

  return buffer;
}

void clu_enqueue_read_buffer(cl_command_queue command_queue,
                             cl_mem buffer,
                             cl_bool blocking_read,
                             size_t offset,
                             size_t cb,
                             void *ptr,
                             cl_uint num_events_in_wait_list,
                             const cl_event *event_wait_list,
                             cl_event *event)
{
  cl_int ret = clEnqueueReadBuffer(command_queue, buffer, blocking_read,
                                    offset, cb, ptr, num_events_in_wait_list,
                                    event_wait_list, event);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_COMMAND_QUEUE:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_INVALID_COMMAND_QUEUE\n");
        break;
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_MEM_OBJECT:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_INVALID_MEM_OBJECT\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_EVENT_WAIT_LIST:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_INVALID_EVENT_WAIT_LIST\n");
        break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_enqueue_read_buffer' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }
}

void clu_enqueue_write_buffer(cl_command_queue command_queue,
                              cl_mem buffer,
                              cl_bool blocking_write,
                              size_t offset,
                              size_t cb,
                              const void *ptr,
                              cl_uint num_events_in_wait_list,
                              const cl_event *event_wait_list,
                              cl_event *event)
{
  cl_int ret = clEnqueueWriteBuffer(command_queue, buffer, blocking_write,
                                    offset, cb, ptr, num_events_in_wait_list,
                                    event_wait_list, event);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_COMMAND_QUEUE:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_INVALID_COMMAND_QUEUE\n");
        break;
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_MEM_OBJECT:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_INVALID_MEM_OBJECT\n");
        break;
      case CL_INVALID_VALUE:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_INVALID_VALUE\n");
        break;
      case CL_INVALID_EVENT_WAIT_LIST:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_INVALID_EVENT_WAIT_LIST\n");
        break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_enqueue_write_buffer' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }
}

void clu_enqueue_nd_range_kernel(cl_command_queue command_queue,
                                 cl_kernel kernel,
                                 cl_uint work_dim,
                                 const size_t *global_work_offset,
                                 const size_t *global_work_size,
                                 const size_t *local_work_size,
                                 cl_uint num_events_in_wait_list,
                                 const cl_event *event_wait_list,
                                 cl_event *event)
{
  cl_int ret = clEnqueueNDRangeKernel(command_queue, kernel, work_dim,
                                      global_work_offset, global_work_size,
                                      local_work_size, num_events_in_wait_list,
                                      event_wait_list, event);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_PROGRAM_EXECUTABLE:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_PROGRAM_EXECUTABLE\n");
        break;
      case CL_INVALID_COMMAND_QUEUE:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_COMMAND_QUEUE\n");
        break;
      case CL_INVALID_KERNEL:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_KERNEL\n");
        break;
      case CL_INVALID_CONTEXT:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_CONTEXT\n");
        break;
      case CL_INVALID_KERNEL_ARGS:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_KERNEL_ARGS\n");
        break;
      case CL_INVALID_WORK_DIMENSION:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_WORK_DIMENSION\n");
        break;
      case CL_INVALID_WORK_GROUP_SIZE:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_WORK_GROUP_SIZE\n");
        break;
      case CL_INVALID_WORK_ITEM_SIZE:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_WORK_ITEM_SIZE\n");
        break;
      case CL_INVALID_GLOBAL_OFFSET:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_GLOBAL_OFFSET\n");
        break;
      case CL_OUT_OF_RESOURCES:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_OUT_OF_RESOURCES\n");
        break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
        break;
      case CL_INVALID_EVENT_WAIT_LIST:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_INVALID_EVENT_WAIT_LIST\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_enqueue_nd_range_kernel' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }
}

void clu_finish(cl_command_queue command_queue)
{
  cl_int ret = clFinish(command_queue);
  if (ret != CL_SUCCESS) {
    switch (ret) {
      case CL_INVALID_COMMAND_QUEUE:
        printf("error: 'clu_finish' failed with CL_INVALID_COMMAND_QUEUE\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error: 'clu_finish' failed with CL_OUT_OF_HOST_MEMORY\n");
        break;
    }
    exit(1);
  }
}
