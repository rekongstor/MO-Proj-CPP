#include "stdafx.h"

SYSTEMTIME st;
Rnd::Rnd()
{
	if (st.wYear == 0)
	{
		GetLocalTime(&st);
		srand(st.wMilliseconds + st.wSecond * 1000);
	}
}

int Random(int min, int max)
{
	uniform_int_distribution<int> distribution(min, max);
	default_random_engine re;
	return (distribution(re) + rand()) % (max - min + 1) + min;
}

double Random()
{
	uniform_real_distribution<double> distribution(0., 1.);
	default_random_engine re;
	double x = distribution(re) + (double)rand() / (double)RAND_MAX;
	return x > 1. ? x - 1. : x;
}

double xy::len()
{
	return sqrt(x * x + y * y);
}

double xy::dist(xy dest)
{
	return sqrt((dest.x - x) * (dest.x - x) + (dest.y - y) * (dest.y - y));
}