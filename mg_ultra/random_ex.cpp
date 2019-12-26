#include <random>

#include "random_ex.h"

std::default_random_engine rGen;
std::normal_distribution<float> nDist(0.0f, 1.0f);


float rand_ex::next_norm(float mu, float sigma) {
	return nDist(rGen) * sigma + mu;
}