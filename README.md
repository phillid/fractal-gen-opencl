# fractal-gen-opencl

This is a work-in-progress port of my simple fractal-gen software to OpenCL.
That software was an experiment of mine to generate mandelbrot (and some of
its cousins) fractals on CPU. This is my attempt at porting that software to
OpenCL so it can be used on a multitude of computation devices, including GPUs.

I had started to port it to CUDA in October 2016, but changed to CL because
of its portability and open nature.

Software is still in early days and needs more CL kernels for such fractals as
tricorn, burning ship, and julia sets to name a few.
