#ifndef __CLU_H__
#define __CLU_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#elif __linux
  #include <CL/cl.h>
#endif

/* CLUT - OpenCL Utility Toolkit
 * ----------------------------- */

#define CLUT_BUILD_WITH_SOURCE 0
#define CLUT_BUILD_WITH_BINARY 1

typedef struct {
  cl_platform_id platform;
  cl_context context;
  cl_program program;
  cl_device_id device;
  cl_command_queue command_queue;
} clut_runtime;

/* OpenCL Helper Functions */

void clut_setup(clut_runtime *runtime,
                cl_device_type device_type,
                cl_uint kernel_files_count,
                const char **kernel_files,
                int build_mode);

/* General Purpose Functions */

const char *clut_file_read(const char *file_name, size_t *size);
int clut_file_write(const char *file_name, char *buffer, size_t size);


/* CLU - OpenCL Utilities
 * ---------------------- */

/* OpenCL Wrapper Functions */

void clu_get_platform_ids(cl_uint num_entries,
                          cl_platform_id *platforms,
                          cl_uint *num_platforms);

void clu_get_device_ids(cl_platform_id platform,
                        cl_device_type device_type,
                        cl_uint num_entries,
                        cl_device_id *devices,
                        cl_uint *num_devices);

cl_context clu_create_context(cl_context_properties *properties,
                              cl_uint num_devices,
                              const cl_device_id *devices,
                              void pfn_notify(const char *errinfo,
                                              const void *private_info,
                                              size_t cb,
                                              void *user_data),
                              void *user_data);

cl_command_queue clu_create_command_queue(cl_context context,
                                          cl_device_id device,
                                          cl_command_queue_properties properties);

cl_program clu_create_program_with_source(cl_context context,
                                          cl_uint count,
                                          const char **strings,
                                          const size_t *lengths);

cl_program clu_create_program_with_binary(cl_context context,
                                          cl_uint num_devices,
                                          const cl_device_id *device_list,
                                          const size_t *lengths,
                                          const unsigned char **binaries,
                                          cl_int *binary_status);

void clu_build_program(cl_program program,
                       cl_uint num_devices,
                       const cl_device_id *device_list,
                       const char *options,
                       void (*pfn_notify) (cl_program, void *user_data),
                       void *user_data);

void clu_get_program_info(cl_program program,
                          cl_program_info param_name,
                          size_t param_value_size,
                          void *param_value,
                          size_t *param_value_size_ret);

void clu_get_program_build_info(cl_program program,
                                cl_device_id device,
                                cl_program_build_info param_name,
                                size_t param_value_size,
                                void *param_value,
                                size_t *param_value_size_ret);

cl_kernel clu_create_kernel(cl_program program,
                            const char *kernel_name);

void clu_set_kernel_arg(cl_kernel kernel,
                        cl_uint arg_index,
                        size_t arg_size,
                        const void *arg_value);

void clu_get_kernel_work_group_info(cl_kernel kernel,
                                    cl_device_id device,
                                    cl_kernel_work_group_info param_name,
                                    size_t param_value_size,
                                    void *param_value,
                                    size_t *param_value_size_ret);

cl_mem clu_create_buffer(cl_context context,
                         cl_mem_flags flags,
                         size_t size,
                         void *host_ptr);

void clu_enqueue_read_buffer(cl_command_queue command_queue,
                             cl_mem buffer,
                             cl_bool blocking_read,
                             size_t offset,
                             size_t cb,
                             void *ptr,
                             cl_uint num_events_in_wait_list,
                             const cl_event *event_wait_list,
                             cl_event *event);

void clu_enqueue_write_buffer(cl_command_queue command_queue,
                              cl_mem buffer,
                              cl_bool blocking_write,
                              size_t offset,
                              size_t cb,
                              const void *ptr,
                              cl_uint num_events_in_wait_list,
                              const cl_event *event_wait_list,
                              cl_event *event);

void clu_enqueue_nd_range_kernel(cl_command_queue command_queue,
                                 cl_kernel kernel,
                                 cl_uint work_dim,
                                 const size_t *global_work_offset,
                                 const size_t *global_work_size,
                                 const size_t *local_work_size,
                                 cl_uint num_events_in_wait_list,
                                 const cl_event *event_wait_list,
                                 cl_event *event);

void clu_finish(cl_command_queue command_queue);

#endif
