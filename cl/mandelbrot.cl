__kernel void fractal_gen(
	__global unsigned char *buffer,
	const unsigned int size,
	const unsigned long iterations)
{
	unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);
	unsigned long i = 0;

	float a = -2.5+(((float)x)/size)*3.5;
	float b = -1.75+(((float)y)/size)*3.5;

	float2 z = (0.0, 0.0);

	for (i = 0; i < iterations; i++) {
		// if abs(z) > 2
		if (z.x*z.x + z.y*z.y > 4)
			break;

		float oldx = z.x;
		z.x = z.x*z.x - z.y*z.y + a;
		z.y = 2*oldx*z.y + b;
	}
	buffer[(size*y)+x] = (i*255)/iterations;
}
