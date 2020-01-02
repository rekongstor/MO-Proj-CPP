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
	xy point;
	float radius;
	float radius2;
	Zone(xy point_, float radius_) : point(point_), radius(radius_), radius2(radius_* radius_){};
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
	std::vector<Obstacle_p> obstacles;
	Field();
	void Draw(void* graphics);
	coll Collision(const xy& start, xy& end); // возвращает точку коллизии с препятствиями
};

extern Field* field;