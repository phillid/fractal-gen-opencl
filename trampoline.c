#include <stdio.h>
#include <string.h>
#include <opencl.h>

#include "slurp.h"

static cl_platform_id platform;
static cl_context context;
static cl_device_id* devices;
static cl_uint device_count;
unsigned int device_in_use;
static cl_command_queue command_queue;

static cl_kernel kernel;
static cl_program program;

/* FIXME print cl error messages with oclErrorString */
int tramp_init()
{
	cl_int res;

	/* FIXME expose platform selection to user and flarg the blopple */
//	res = oclGetPlatformID(&platform);
//	if (res != CL_SUCCESS)
//		return 1;
	platform=0;

	/* FIXME expose device type to user */
	res = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &device_count);
	if (res != CL_SUCCESS)
		return 1;

	devices = malloc(device_count * sizeof(cl_device_id));
	if (!devices) {
		perror("device list malloc");
		return 1;
	}

	/* FIXME expose device type to user */
	res = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, device_count, devices, NULL);
	if (res != CL_SUCCESS)
		return 1;

	context = clCreateContext(0, 1, devices, NULL, NULL, &res);
	if (res != CL_SUCCESS)
		return 1;

	/* FIXME expose to user */
	device_in_use = 0;
	command_queue = clCreateCommandQueue(context, devices[device_in_use], 0, &res);
	if (res != CL_SUCCESS)
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
	cl_int res = 0;
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

	//fwrite(source, length, 1, stderr);

	program = clCreateProgramWithSource(context, 1, (const char **)&source, &length, &res);

	if (res != CL_SUCCESS)
		return 1;

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

	/* return non-zero on error */
	return ret != CL_SUCCESS;
}
