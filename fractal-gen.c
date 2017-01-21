#include <stdio.h>

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

	fprintf(stderr, "Destroying CL trampoline... ");
	tramp_destroy();
	fprintf(stderr, "Blown to smitherines.\n");
	return 0;
}
