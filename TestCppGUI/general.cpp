#include "stdafx.h"
#include "robot.h"
#include "quadtree.h"
#include "push.h"



void Simulate(Container& dest)
{
	array<Robot*, gen_size> &bots = *dest.bots;
	for (int i = 0; i < gen_size; ++i)
		bots[i] = new Robot(*robot);


	bool fin = false;

	Robot* best = bots[0];

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
			if (best->fin_dist2 > b->fin_dist2)
				best = b;
		}
	}
	dest.best = new Robot(*best);
	dest.best->q.Split(dest.best->coord.x, dest.best->coord.y);

}

std::random_device rd;

std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0, 1.0);

int Random(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(mt);
}

float Random()
{
	return dist(mt);
}

bool xy::operator!=(const xy& r)
{
	return (x != r.x) && (y != r.y);
}

xy& xy::operator*(const float& r)
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
	return min(min, max(x, max));
}
