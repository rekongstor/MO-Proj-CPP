#include <objidl.h>
#include <gdiplus.h>
#include "field.h"
using namespace Gdiplus;

#ifndef FIELD
Field::Field()
{
	obstacles.push_back(new Border()); // ������ ���� �������, ����� ����� ���� ��������� � �������� ����������
	for (int i = Random(small_zones[0], small_zones[1]); i > 0; --i)
	{
		obstacles.push_back(new Zone(xy(Random(), Random()), Random() * (small_zone_size[1] - small_zone_size[0]) + small_zone_size[0]));
	}	
	for (int i = Random(big_zones[0], big_zones[1]); i > 0; --i)
	{
		obstacles.push_back(new Zone(xy(Random(), Random()), Random() * (big_zone_size[1] - big_zone_size[0]) + big_zone_size[0]));
	}
}

void Field::Draw(void* gr)
{
	for (auto o : obstacles)
		o->Draw(gr);
}

coll Field::Collision(xy start, xy end)
{
	coll ret(xy(10.0f, 10.0f), xy00);
	for (auto o : obstacles)
	{
		coll tmp = o->Collision(start, end);
		if (tmp.normal.len2() > 0.f)
			if (ret.point.dist2(start) > tmp.point.dist2(start)) // ���� ����� ��������� �������� ����� � ������. ����� ����� ����
				ret = tmp;
	}
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

coll Zone::Collision(xy start, xy end)
{
	coll rez(end, xy00);
	if (end.dist2(point) < radius2)
		rez = coll(end, xy11);

	// TODO: �������� ���������� ��������
	// �� ����� ������ � ����� �����
	// ���� �������� ����, �� ������� � � ������� � �����������
	// ���� �������� ���, �� ������� ��������� � ����� ������� n = 0 (n.x = 0; n.y = 0)
	return rez;
}
#endif // !ZONE

#ifndef BORDER
coll Border::Collision(xy start, xy end)
{
	coll rez(end, xy00);
	if (end.x < 0.f || end.x > 1.f || end.y < 0.f || end.y > 1.f)
		rez = coll(end, xy11);
	
	// TODO: �������� ���������� ��������
	// �� ����� ������ � ����� �����
	// ���� �������� ����, �� ������� � � ������� � �����������
	// ���� �������� ���, �� ������� ��������� � ����� ������� n = 0 (n.x = 0; n.y = 0)
	return rez;
}
#endif // !BORDER
