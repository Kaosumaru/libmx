#include "Random.h"
#include <math.h>
#include <ctime>
#include <cstdlib>

Random::MyRNG Random::_rgn;

void Random::Init()
{
    unsigned long t = (unsigned long)time(0);
    _rgn.seed(t);
    srand(t);
}

void Random::Seed(unsigned long s)
{
	_rgn.seed(s);
	srand(s);
}

bool Random::chance(float chance)
{
	return randomRange(1.0) <= chance;
}

float Random::randomRange(float range)
{
	return std::uniform_real_distribution<float>(0.0f,range)(_rgn);
}

unsigned Random::randomRange(const std::pair<unsigned, unsigned>& pair)
{
	if (pair.first == pair.second)
		return pair.first;
	return std::uniform_int_distribution<unsigned>(pair.first,pair.second)(_rgn);
}
int Random::randomRange(const std::pair<int, int>& pair)
{
	if (pair.first == pair.second)
		return pair.first;
	return std::uniform_int_distribution<int>(pair.first,pair.second)(_rgn);
}