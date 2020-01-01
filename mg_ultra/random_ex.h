/*extended random functionality*/
#ifndef __RANDOM_EX__
#define __RANDOM_EX__

namespace rand_ex {

	//gets next normal value
	float next_norm(float mu, float sigma);

	//fills an array with results of a normal distribution
	//centered on 0
	//index refers to offset, e.g. values 0-n
	//uniformly increased such that all values add to 1;
	void populate_half_norm(int n, float sigma, float* fill);
}

#endif 