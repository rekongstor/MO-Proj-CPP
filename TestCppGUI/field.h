#pragma once
#include "stdafx.h"

struct Obstacle
{
	virtual ~Obstacle() {};
	virtual coll Collision(xy start, xy end) = 0;
	virtual void Draw(void* graphics) = 0;
};

struct Zone : Obstacle
{
	xy p;
	double r;
	Zone(xy point, double radius) : p(point), r(radius) {};
	~Zone() {};
	coll Collision(xy start, xy end);
	void Draw(void* graphics); // рисует все препятствия (зоны)
};

struct Border : Obstacle
{
	~Border() {};
	coll Collision(xy start, xy end);
	void Draw(void* graphics) {}; // границы не рисуются
};

struct Field
{
	std::vector<Obstacle*> obstacles;
	Field();
	void Draw(void* graphics);
	coll Collision(xy start, xy end); // возвращает точку коллизии с препятствиями
};

extern Field field;