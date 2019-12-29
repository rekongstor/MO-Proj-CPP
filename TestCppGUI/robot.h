#pragma once
#include "stdafx.h"
#include "quadtree.h"

struct Robot
{
	QT q;
	Robot();
	void DrawQT(void* gr);
};

extern Robot robot;