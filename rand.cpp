#include "rand.h"
#include <random>

namespace rnd
{
	float range01()
	{
		return static_cast<float>(rand()) / RAND_MAX;
	}

	float range(float min, float max)
	{
		return range01() * (max - min) + min;
	}

	bool coinFlip()
	{
		return rand() % 2;
	}
}