#include <stdio.h>
#include <stdlib.h>

#include "trampoline.h"

static unsigned long size;
static unsigned long iterations;

int main()
{
	size = 1024;
	iterations = 102400;

	fprintf(stderr, "Building CL trampoline... ");
	if (tramp_init()) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Done.\n");

	fprintf(stderr, "Loading kernel source from file... ");
	if (tramp_load_kernel("test.cl")) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Loaded.\n");

	fprintf(stderr, "Compiling kernel source... ");
	if (tramp_compile_kernel()) {
		fprintf(stderr, "Failed:\n%s\n", tramp_get_build_log());
		return 1;
	}
	fprintf(stderr, "Compiled.\n");

	fprintf(stderr, "Setting kernel arguments... ");
	if (tramp_set_kernel_args(size, iterations)) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Done.\n");

	fprintf(stderr, "Running kernel... ");
	if (tramp_run_kernel()) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Done.\n");

	char *buffer = malloc(size*size);
	if (!buffer) {
		perror("host data buffer malloc");
		return 1;
	}
	fprintf(stderr, "Reading data from device... ");
	if (tramp_copy_data(&buffer, size*size)) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Done.\n");

	fprintf(stderr, "Destroying CL trampoline... ");
	tramp_destroy();
	fprintf(stderr, "Blown to smitherines.\n");

	printf("P5\n%d\n%d\n255\n",size,size);
	fwrite(buffer, size*size, 1, stdout);

	return 0;
}
