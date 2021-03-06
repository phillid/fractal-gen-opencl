# fractal-gen-opencl

	Syntax
	fractal-gen -p <platform> -s <size> -i <iterations>
	
	-p platform   - name of the preferred OpenCL platform to use. If no platform
	                with this name is available, the first available platform
	                is selected and the user is warned. If you do not know the
	                name of the platform, set to a nonsense string like 'any'.
	-s size       - size of the image to generate in pixels (square image)
	-i iterations - maximum number of iterations to use on each pixel to
	                determine its set membership
	

This is a work-in-progress port of my simple fractal-gen software to OpenCL.
That software was an experiment of mine to generate mandelbrot (and some of
its cousin) fractals on the CPU. This is my attempt at porting that software to
OpenCL so it can be used on a multitude of computation devices, chiefly GPUs.

## About

I had started to port it to CUDA in October 2016, but changed to CL because
of its portability and open nature.

The software is working with basic mandelbrot fractals, but needs more CL
kernels for such fractals as tricorn, burning ship, and julia sets to name a
few.

Below is a simple demo image produced with the software. It is a simple
mandelbrot fractal using only 75 iterations, at 768 square pixels. The
image is a negative of that which comes straight from the software.

![Demo mandelbrot image with few iterations](./demo-low-iterat.png)

Such a small, low-detail image will not provide a case for using GPU rather
than CPU, but once you start upping the image size and detail, a modern GPU
will provide endless benefit over a modern CPU.

For example, using the CPU-based fractal-gen, a 10240x10240 pixel image at a
10000 iteration cutout per pixel, the image will complete in about 2200 seconds
when running on all 32 threads of a dual-Xeon E5-2670 setup. Compare this to
the runtime of this software on a (much cheaper) NVIDIA GTX 1070; 1 second.
This is using single-precision floats on the GPU, but changing to `double` data
types only slows the 1070 to about 8 seconds.
