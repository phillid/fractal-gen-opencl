#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "trampoline.h"

int run(unsigned int size, unsigned int iterations)
{
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
	if (tramp_copy_data((void*)&buffer, size*size)) {
		fprintf(stderr, "Failed.\n");
		return 1;
	}
	fprintf(stderr, "Done.\n");

	fprintf(stderr, "Destroying CL trampoline... ");
	tramp_destroy();
	fprintf(stderr, "Blown to smitherines.\n");

	printf("P5\n%d\n%d\n255\n",size,size);
	fwrite(buffer, size*size, 1, stdout);
}

void die_help()
{
	fprintf(stderr, "Syntax:\n%s [-s size] [-i max_iteratons]\n");
	exit(1);
}

int main(int argc, char **argv)
{
	unsigned int size = 0;
	unsigned int iterations = 0;
	char c = '\0';

	while ((c = getopt(argc, argv, "s:i:")) != -1) {
		switch (c) {
		case 's':
			size = atoi(optarg);
			break;
		case 'i':
			iterations = atoi(optarg);
			break;
		case '?':
			die_help();
			return 1; /* mostly unreachable */
			break; /* unreachable */
		}
	}

	if (size == 0 || iterations == 0) {
		die_help();
		return 1; /* mostly unreachable */
	}

	run(size, iterations);
	return 0;
}
