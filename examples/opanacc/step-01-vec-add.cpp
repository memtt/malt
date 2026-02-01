#include <cstdio>
#include <cstdlib>

int main()
{
	//nb elements
	const size_t size = 1024*1024;

	//alloc
	float * a = new float[size];
	float * b = new float[size];
	float * c = new float[size];

	//init
	for(size_t i = 0 ; i < size ; i++)
		a[i] = i;
	for(size_t i = 0 ; i < size ; i++)
		b[i] = 2*i;
	for(size_t i = 0 ; i < size ; i++)
		c[i] = 0;

	//sum
	#pragma acc kernels
	for (size_t i = 0 ; i < size ; i++) {
		c[i] = a[i] + b[i];
	}

	//sum
	float tot = 0.0;
	for(size_t i = 0 ; i < size ; i++)
		tot += c[i];

	//print
	printf("tot=%f\n", tot);

	//free
	delete [] a;
	delete [] b;
	delete [] c;

	//ok
	return EXIT_SUCCESS;
}
