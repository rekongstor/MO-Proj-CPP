#pragma once
#include "stdafx.h"
#include "quadtree.h"

struct Robot
{
	float fin_dist2;
	float life_time;
	xy coord;
	xy speed;
	coll c;

	QT q;
	Robot();
	Robot(const Robot&);
	~Robot();
	Robot& operator=(const Robot&);
	void DrawQT(void* gr);
	void Simulate(void* gr, bool s);
};

extern Robot_p robot;