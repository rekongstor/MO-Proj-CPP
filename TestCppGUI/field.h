#pragma once
#include "stdafx.h"

struct Zone
{
	xy point;
	float radius;
	float radius2;
	Zone(xy point_, float radius_) : point(point_), radius(radius_), radius2(radius_* radius_){};
	~Zone() {};
	coll Collision(const xy& start, xy& end);
	void Draw(void* graphics); // ������ ��� ����������� (����)
};

struct Border
{
	~Border() {};
	coll Collision(const xy& start, xy& end);
	void Draw(void* graphics) {}; // ������� �� ��������
};

struct Field
{
	Border_p border;
	std::vector<Zone_p> obstacles;
	Field();
	void Draw(void* graphics);
	coll Collision(const xy& start, xy& end); // ���������� ����� �������� � �������������
};

extern Field_p field;