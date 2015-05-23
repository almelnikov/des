#include "generators.hpp"

int Uniform(int bound)
{
	double x = static_cast<double>(std::rand()) / RAND_MAX;
	return static_cast<int> (x * bound);
}

int Uniform(int left_bound, int right_bound)
{
	double x = static_cast<double>(std::rand()) / RAND_MAX;
	return (right_bound - left_bound) * x + left_bound;
}	

double Expon(double lambda)
{
	double x = static_cast<double>(std::rand()) / RAND_MAX;
	return static_cast<double>(log(x) / -lambda);
}

