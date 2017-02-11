int tramp_init();
void tramp_destroy();
int tramp_load_kernel(const char *filename);
char *tramp_get_build_log();
int tramp_compile_kernel();
int tramp_set_kernel_args();
int tramp_run_kernel();
int tramp_copy_data();
