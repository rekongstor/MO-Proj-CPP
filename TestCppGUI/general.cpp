#include "stdafx.h"
#include "robot.h"
#include "quadtree.h"
#include "push.h"

float cos(xy& a, xy& b)
{
	return (a.x * b.x + a.y * b.y) / a.len() / b.len();
}

void Simulate(Container& dest)
{
	array<Robot_p, gen_size> &bots = *dest.bots;
	for (int i = 0; i < gen_size; ++i)
		bots[i] = make_shared<Robot>(*robot);


	bool fin = false;

	Robot_p best = bots[0];

	for (auto& b : bots)
	{
		b->q.Randomize(b->life_time);
		b->Simulate();
		if (b->fin_dist2 < finish_dist2)
		{
			fin = true;
			best = b;
		}
	}


	for (auto& b : bots)
	{
		if (fin) // ищем лучшего по времени
		{
			if ((best->fin_dist2 > b->fin_dist2) && (best->life_time > b->life_time))
				best = b;
		}
		else // ищем лучшего по расстоянию
		{
			if (best->fin_dist2 > b->fin_dist2 && b->c.point.x < 9.f)
				best = b;
		}
	}
	dest.best = make_shared<Robot>(*best);
}


#ifdef true_random
std::random_device rd;

std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist_r(0.0, 1.0);
int Random(int min, int max)
{
	std::uniform_int_distribution<int> dist_i(min, max);
	return dist_i(mt);
}

float Random()
{
	return dist_r(mt);
}
#else
SYSTEMTIME st;

Rnd::Rnd()
{
	GetLocalTime(&st);
	srand(st.wMilliseconds + st.wSecond * 1000 + st.wMinute * 60 * 1000);
}

int Random(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

float Random()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}
#endif // true_random

void xy::operator+=(const xy&& r)
{
	x += r.x;
	y += r.y;
}

bool xy::operator!=(const xy& r)
{
	return (x != r.x) && (y != r.y);
}

xy xy::operator*(const float& r)
{
	this->x *= r;
	this->y *= r;
	return *this;
}

float xy::len()
{
	return sqrt(x * x + y * y);
}

float xy::len2()
{
	return (x * x + y * y);
}

float xy::dist(xy dest)
{
	return sqrt((dest.x - x) * (dest.x - x) + (dest.y - y) * (dest.y - y));
}
float xy::dist2(xy dest)
{
	return (dest.x - x) * (dest.x - x) + (dest.y - y) * (dest.y - y);
}

float clamp(float min, float x, float max)
{
	return max(min, min(x, max));
}
