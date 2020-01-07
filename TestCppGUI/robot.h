#pragma once
#include "stdafx.h"
#include "quadtree.h"

struct Robot
{
	float fin_dist2;
	xy coord;
	xy speed;
	float life_time;
	coll c;

	QT q;
	Robot();
	Robot(const Robot&);
	~Robot();
	Robot& operator=(const Robot&);
	void DrawQT(void* gr);
	void Simulate(void* gr = nullptr, bool s = false);
};

extern Robot_p robot;