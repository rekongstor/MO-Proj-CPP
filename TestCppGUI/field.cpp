#include <objidl.h>
#include <gdiplus.h>
#include "field.h"
using namespace Gdiplus;

#ifndef FIELD
Field::Field()
{
	border = (Border_p(new Border)); // создаём одну границу, чтобы можно было проверять её коллизию полиморфно
	for (int i = Random(small_zones[0], small_zones[1]); i > 0; --i)
	{
		obstacles.push_back(make_shared<Zone>(xy(Random(), Random()), Random() * (small_zone_size[1] - small_zone_size[0]) + small_zone_size[0]));
	}	
	for (int i = Random(big_zones[0], big_zones[1]); i > 0; --i)
	{
		obstacles.push_back(make_shared<Zone>(xy(Random(), Random()), Random() * (big_zone_size[1] - big_zone_size[0]) + big_zone_size[0]));
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
	for (auto& o : obstacles)
	{
		coll tmp = o->Collision(start, end);
		if (tmp.normal.len2() > 0.f)
			if (ret.point.dist2(start) > tmp.point.dist2(start)) // если новая найденная коллизия ближе к началу. Такое может быть
				ret = tmp;
	}
	if (ret.normal.len2() > 0.f)
		end = ret.point;
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
	if (end.x - point.x < radius) // axis aligned bounding box yay (aabb)
	if (end.x - point.x > -radius) // axis aligned bounding box yay (aabb)
	if (end.y - point.y < radius)
	if (end.y - point.y > -radius)
	if (end.dist2(point) < radius2)
	{
		coll rez(end, xy(end.x - point.x, end.y - point.y));
		rez.normal.x /= radius;
		rez.normal.y /= radius;
		return rez;
	}
	// TODO: запилить реализацию коллизии
	// На входе старая и новая точка
	// Если коллизия есть, то вернуть её и нормаль к поверхности
	// Если коллизии нет, то вернуть структуру с полем нормали n = 0 (n.x = 0; n.y = 0)
	return coll(end, xy00);
}
#endif // !ZONE

#ifndef BORDER
coll Border::Collision(const xy& start, xy& end)
{
	if (end.x < 0.f)
	{
		return coll(xy(0.f, end.y), xy(1.f, 0.f));
	}
	if (end.y < 0.f)
	{
		return coll(xy(end.x, 0.f), xy(0.f, 1.f));
	}
	if (end.x > 1.f)
	{
		return coll(xy(1.f, end.y), xy(-1.f, 0.f));
	}
	if (end.y > 1.f)
	{
		return coll(xy(end.x, 1.f), xy(0.f, -1.f));
	}
	// TODO: запилить реализацию коллизии
	// На входе старая и новая точка
	// Если коллизия есть, то вернуть её и нормаль к поверхности
	// Если коллизии нет, то вернуть структуру с полем нормали n = 0 (n.x = 0; n.y = 0)
	return coll(xy(10.0f, 10.0f), xy00);
}
#endif // !BORDER
