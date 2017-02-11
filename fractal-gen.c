#include <stdio.h>
#include <stdlib.h>

#include "trampoline.h"

int main() {
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
	if (tramp_set_kernel_args()) {
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


	unsigned int *buffer = calloc(sizeof(unsigned int), 1024*1024);
	if (!buffer) {
		perror("malloc");
		return 1;
	}
	fprintf(stderr, "Reading data from device... ");
	if (tramp_copy_data(&buffer)) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Done.\n");

	fprintf(stderr, "Destroying CL trampoline... ");
	tramp_destroy();
	fprintf(stderr, "Blown to smitherines.\n");

	printf("P5\n1024\n1024\n255\n");
	unsigned int i;
	for (i = 0; i < 1024*1024; i++)
		fputc(buffer[i], stdout);

	return 0;
}
