__kernel void test(__global float *a, __global float *b)
{
	unsigned int i = get_global_id(0);
	b[i] = a[i] * 2.f;
}
