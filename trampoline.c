#include <stdio.h>
#include <string.h>
#include <opencl.h>

#include "cl_error.h"
#include "slurp.h"

static cl_platform_id platform;
static cl_context context;
static cl_device_id* devices;
static cl_uint device_count;
static unsigned int device_in_use;
static cl_command_queue command_queue;
static cl_mem device_buffer;

static cl_kernel kernel;
static cl_program program;

static unsigned int size;
static unsigned int iterations;


/* FIXME print cl error messages with oclErrorString */
int tramp_init()
{
	cl_int ret;

	/* FIXME expose platform selection to user and flarg the blopple */
//	ret = oclGetPlatformID(&platform);
//	if (ret != CL_SUCCESS)
//		return 1;
	platform=0;

	/* FIXME expose device type to user */
	ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &device_count);
	if (ret != CL_SUCCESS)
		return 1;

	devices = malloc(device_count * sizeof(cl_device_id));
	if (!devices) {
		perror("device list malloc");
		return 1;
	}

	/* FIXME expose device type to user */
	ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, device_count, devices, NULL);
	if (ret != CL_SUCCESS)
		return 1;

	context = clCreateContext(0, 1, devices, NULL, NULL, &ret);
	if (ret != CL_SUCCESS)
		return 1;

	/* FIXME expose to user */
	device_in_use = 0;
	command_queue = clCreateCommandQueue(context, devices[device_in_use], 0, &ret);
	if (ret != CL_SUCCESS)
		return 1;

	return 0;
}

void tramp_destroy()
{
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);
	
	if (devices) {
		free(devices);
		devices = NULL;
	}
}

int tramp_load_kernel(const char *filename)
{
	cl_int ret = 0;
	size_t length = 0;
	FILE *fin = fopen(filename, "r");
	char *source = NULL;
	if (!fin) {
		perror("fopen");
		return 1;
	}
	source = slurp(fin, &length);
	if (!source)
		return 1;

	fclose(fin);

	program = clCreateProgramWithSource(context, 1, (const char **)&source, &length, &ret);

	if (ret != CL_SUCCESS) {
		fprintf(stderr, "Failed to create program from source code: %s ", get_cl_error_string(ret));
		return 1;
	}

	free(source);
	source = NULL;

	return 0;
}

char *tramp_get_build_log()
{
	cl_int ret = 0;
	cl_build_status build_status;
	char *build_log = NULL;
	size_t log_size = 0;

	ret = clGetProgramBuildInfo(program, devices[device_in_use],
	                            CL_PROGRAM_BUILD_STATUS,
	                            sizeof(cl_build_status),
	                            &build_status, NULL);

	/* FIXME check */

	ret = clGetProgramBuildInfo(program, devices[device_in_use],
	                            CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	/* FIXME check */
	
	/* + 1 for null-terminator */
	build_log = malloc(log_size + 1);
	if (!build_log) {
		perror("malloc");
		return NULL;
	}
	
	ret = clGetProgramBuildInfo(program, devices[device_in_use],
	                            CL_PROGRAM_BUILD_LOG,
	                            log_size, build_log, NULL);
	
	/* null-terminate log */
	build_log[log_size] = '\0';

	return build_log;
}

int tramp_compile_kernel()
{
	cl_int ret = 0;

	ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (ret != CL_SUCCESS)
		return 1;

	kernel = clCreateKernel(program, "fractal_gen", &ret);

	/* return non-zero on error */
	return ret != CL_SUCCESS;
}

int tramp_set_kernel_args(unsigned int s, unsigned int it)
{
	cl_int ret = 0;

	size = s;
	iterations = it;

	device_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size*size, NULL, &ret);
	if (ret != CL_SUCCESS)
		return 1;

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_buffer);
	if (ret != CL_SUCCESS) {
		fprintf(stderr, "Error on buffer argument: %s ", get_cl_error_string(ret));
		return 1;
	}

	ret = clSetKernelArg(kernel, 1, sizeof(unsigned int), &size);
	if (ret != CL_SUCCESS) {
		fprintf(stderr, "Error on size argument: %s ", get_cl_error_string(ret));
		return 1;
	}

	ret = clSetKernelArg(kernel, 2, sizeof(unsigned int), &iterations);
	if (ret != CL_SUCCESS) {
		fprintf(stderr, "Error on iteration argument: %s ", get_cl_error_string(ret));
		return 1;
	}

	return 0;
}

int tramp_run_kernel()
{
	cl_event event;
	cl_int ret = 0;
	size_t workgroup_sizes[2];
	workgroup_sizes[0] = size;
	workgroup_sizes[1] = size;

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, workgroup_sizes, NULL, 0, NULL, &event);
	if (ret != CL_SUCCESS) {
		fprintf(stderr, "%d",ret);
		return ret;
	}

	clReleaseEvent(event);
	clFinish(command_queue);

	return ret;
}

int tramp_copy_data(void **buffer, size_t size)
{
	cl_event event;
	cl_int ret = 0;

	ret = clEnqueueReadBuffer(command_queue, device_buffer, CL_TRUE, 0, size, *buffer, 0, NULL, &event);
	clReleaseEvent(event);
}
