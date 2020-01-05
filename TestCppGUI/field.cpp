#include <objidl.h>
#include <gdiplus.h>
#include "field.h"
using namespace Gdiplus;

#ifndef FIELD

void Field::AssignToMap(float point, Zone_p& z, map<float, set<Zone*>>& aabb)
{
	auto lambda_copy = [&](float key)
	{
		auto& key_vec = aabb[key];
		if (key_vec.size() == 0) // ���� ��� ������ ������ ��� - ����� ����������� ��� �������� �� �����������
		{
			auto it = aabb.find(key);
			if (it != aabb.begin()) // ������ ���� ��� ������� �� �������� ����� �����
			{
				--it;
				auto& copy_vec = it->second;
				for (auto& p : copy_vec)
					key_vec.insert(p);
			}
		}
	};

	lambda_copy(point - z->radius);
	lambda_copy(point + z->radius);

	// � ��� ���� ���� ����� ������ ����� �������� ��� �������
	auto it_beg = aabb.find(point - z->radius);
	auto it_end = aabb.find(point + z->radius);
	for (auto it = it_beg; it != it_end; ++it)
	{
		it->second.insert(z.get());
	}
}

Field::Field()
{
	border = make_shared<Border>(); // ������ ���� �������, ����� ����� ���� ��������� � �������� ����������
	for (int i = Random(small_zones[0], small_zones[1]); i > 0; --i)
	{
		obstacles.push_back(make_shared<Zone>(xy(Random(), Random()), Random() * (small_zone_size[1] - small_zone_size[0]) + small_zone_size[0]));
		auto& obst = obstacles.back();
		AssignToMap(obst->point.x, obst, aabb_x);
		AssignToMap(obst->point.y, obst, aabb_y);
	}	
	for (int i = Random(big_zones[0], big_zones[1]); i > 0; --i)
	{
		obstacles.push_back(make_shared<Zone>(xy(Random(), Random()), Random() * (big_zone_size[1] - big_zone_size[0]) + big_zone_size[0]));
		auto& obst = obstacles.back();
		AssignToMap(obst->point.x, obst, aabb_x);
		AssignToMap(obst->point.y, obst, aabb_y);
	}
}


void Field::Draw(void* gr)
{
	for (auto& o : obstacles)
		o->Draw(gr);
}

coll Field::Collision(const xy& start, xy& end)
{
	coll ret = border->Collision(start, end);
	//if (ret.point.x != 10.f)
	//	return ret; // ���� ������ ������ ��������, �� �� ��������� ���������?

	// Get From Map
	auto it_x = aabb_x.upper_bound(end.x);
	if (it_x == aabb_x.begin())
		return ret; // ���� ����� �� ����� �� ����������� aabb, �� ������ � ����������� ��� �� ����� - ���������� ��� ����
	auto it_y = aabb_y.upper_bound(end.y);
	if (it_y == aabb_y.begin())
		return ret;
	--it_x; --it_y;
	set<Zone*>& x = it_x->second;
	set<Zone*>& y = it_y->second;

	if (x.size() < y.size()) // �������� ������� ������
	{
		for (auto& p : x) // �������� �� x, ���� �� y: O(x * lg(y))
		{
			if (y.find(p) != y.end()) // �� ����� ��� �� ���������
			{
				coll tmp(p->Collision(start, end));
				if (tmp.point.dist2(start) < ret.point.dist2(start))
					ret = tmp;
				//if (ret.point.x != 10.f)
				//	return ret; // ���� ������ ������ ��������, �� �� ��������� ���������?
			}
		}
	}
	else
	{
		for (auto& p : y)// �������� �� y, ���� �� x: O(y * lg(x))
		{
			if (x.find(p) != x.end()) // �� ����� ��� �� ���������
			{
				coll tmp(p->Collision(start, end));
				if (tmp.point.dist2(start) < ret.point.dist2(start))
					ret = tmp;
				//if (ret.point.x != 10.f)
				//	return ret; // ���� ������ ������ ��������, �� �� ��������� ���������?
			}
		}
	}
	// Get From Map

	return ret;
}
#endif // !FIELD

#ifndef ZONE
void Zone::Draw(void* gr)
{
	Graphics& graphics = *(Graphics*)gr;
	SolidBrush br(Color(255, 0, 0));
	int pixel_r = radius * reg_w;
	graphics.FillEllipse(&br, 
		padding + (int)(point.x * reg_w) - pixel_r,
		padding + (int)((1.f - point.y) * reg_w) - pixel_r,
		pixel_r * 2,
		pixel_r * 2);
}

coll Zone::Collision(const xy& start, xy& end)
{
	//starts:
	xy point_ = this->point;
	xy start_ = start;
	xy end_ = end;
	if (start.x - end.x < start.y - end.y) // ���� ������� �� x ������ ������� �� y, �� ���� ������ ��������� ���� ������������
	{ // �� ����� ������ ���������� �������
		swap(point_.x, point_.y);
		swap(start_.x, start_.y);
		swap(end_.x, end_.y);
	}
	double a = (start_.y - end_.y) / (start_.x - end_.x);
	double b = end_.y - a * end_.x;
	double ax2 = 1.f + a * a;
	double bx = (2.f * a * b) - (2.f * point_.x) - (2.f * a * point_.y);
	double c = point_.x * point_.x + point_.y * point_.y + b * b - radius * radius - 2.f * point_.y * b;
	double D = bx * bx - 4.f * ax2 * c;
	double x1, x2, y1, y2;
	xy p1, p2;
	if (D < 0.f) // ����������� ���
		return coll(xyxx, xy00);
	else
	if (D == 0.f) // ����������� ����
	{
		x1 = (-bx) / (2.f * ax2);
		y1 = a * x1 + b;
		p1 = xy(x1, y1);
		if (p1.dist2(start_) < end_.dist2(start_)) // ���� ��� ����������� ����� ��������� � �������� ������
		{
			end_ = p1;
			if (start.x - end.x < start.y - end.y) // ���� ������ �������
			{
				end.x = end_.y;
				end.y = end_.x;
			}
			else
			{
				end = end_;
			}
			coll rez(end, xy(end.x - point.x, end.y - point.y));
			rez.normal.x /= radius;
			rez.normal.y /= radius;
			return rez;
		}
	}
	else
	{
		x1 = (-bx + sqrt(D)) / (2.f * ax2);
		x2 = (-bx - sqrt(D)) / (2.f * ax2);
		y1 = a * x1 + b;
		y2 = a * x2 + b;
		p1 = xy(x1, y1);
		p2 = xy(x2, y2);
		// ���������, ����� ����� ����� � ������
		if (p1.dist2(start_) > p2.dist2(start_)) // ���� ����� ����� �����, �� ������� � ������
			p1 = p2;

		if (p1.dist2(start_) < end_.dist2(start_)) // ���� ��� ����������� ����� ��������� � �������� ������
		{
			end_ = p1;
			if (start.x - end.x < start.y - end.y)
			{
				end.x = end_.y;
				end.y = end_.x;
			}
			else
			{
				end = end_;
			}
			coll rez(end, xy(end.x - point.x, end.y - point.y));
			rez.normal.x /= radius;
			rez.normal.y /= radius;
			return rez;
		}
	}
	//if (end.dist2(point) <= radius2)
	//	goto starts;
	return coll(xyxx, xy00);
}
#endif // !ZONE

#ifndef BORDER
coll Border::Collision(const xy& start, xy& end)
{
	if (end.x < 0.f)
	{
		float a = (start.y - end.y) / (start.x - end.x);
		float b = end.y - a * end.x;
		end.x = 0.f;
		end.y = b;
		return coll(end, xy(1.f, 0.f));
	}
	if (end.y < 0.f)
	{
		float a = (start.y - end.y) / (start.x - end.x);
		float b = end.y - a * end.x;
		end.x = - b / a;
		end.y = 0.f;
		return coll(end, xy(0.f, 1.f));
	}
	if (end.x > 1.f)
	{
		float a = (start.y - end.y) / (start.x - end.x);
		float b = end.y - a * end.x;
		end.x = 1.f;
		end.y = a + b;
		return coll(end, xy(-1.f, 0.f));
	}
	if (end.y > 1.f)
	{
		float a = (start.y - end.y) / (start.x - end.x);
		float b = end.y - a * end.x;
		end.x = (1.f - b) / a;
		end.y = 1.f;
		return coll(end, xy(0.f, -1.f));
	}
	return coll(xyxx, xy00);
}
#endif // !BORDER
