#pragma once
#include "stdafx.h"
#include "quadtree.h"

struct Robot
{
	xy coord;
	xy speed;
	float life_time;
	float fin_dist2;

	QT q;
	Robot();
	Robot(const Robot&);
	Robot& operator=(const Robot&);
	~Robot();
	void DrawQT(void* gr);
	void Simulate(void* gr = nullptr);
};

extern Robot* robot;