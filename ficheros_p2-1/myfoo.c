#include <math.h>


void foo( float* sth, float* theta, int p) {
	float pi = 3.1415927;
	#pragma ivdep
	for(int i = 0; i < p; i++) 
		sth[i] = sinf(theta[i]+pi);
}

