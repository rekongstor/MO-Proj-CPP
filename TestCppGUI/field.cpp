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
		if (key_vec.size() == 0) // лист был создан только что - нужно скопировать все элементы из предыдущего
		{
			auto it = aabb.find(key);
			if (it != aabb.begin()) // только если наш элемент не €вл€етс€ самым левым
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

	// а дл€ всех краЄв кроме левого нужно добавить сам элемент
	auto it_beg = aabb.find(point - z->radius);
	auto it_end = aabb.find(point + z->radius);
	for (auto it = it_beg; it != it_end; ++it)
	{
		it->second.insert(z.get());
	}
}

Field::Field()
{
	border = make_shared<Border>(); // создаЄм одну границу, чтобы можно было провер€ть еЄ коллизию полиморфно
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
	//if (ret.point.x == 10.f)
	//	return ret; // если бордер вернул коллизию, то не провер€ем остальное?

	// Get From Map
	auto it_x = aabb_x.upper_bound(end.x);
	if (it_x == aabb_x.begin())
		return ret; // если слева от точки не установлены aabb, то значит и пересечений там не будет - возвращаем что есть
	auto it_y = aabb_y.upper_bound(end.y);
	if (it_y == aabb_y.begin())
		return ret;
	--it_x; --it_y;
	set<Zone*>& x = it_x->second;
	set<Zone*>& y = it_y->second;

	if (x.size() < y.size()) // выбираем больший массив
	{
		for (auto& p : x) // проходим по x, ищем по y: O(x * lg(y))
		{
			if (y.find(p) != y.end()) // мы нашли тот же указатель
			{
				coll tmp = p->Collision(start, end);
				if (tmp.normal.len2() > 0.f)
					if (ret.point.dist2(start) > tmp.point.dist2(start)) // если нова€ найденна€ коллизи€ ближе к началу. “акое может быть
						ret = tmp;
			}
		}
	}
	else
	{
		for (auto& p : y)// проходим по y, ищем по x: O(y * lg(x))
		{
			if (x.find(p) != x.end()) // мы нашли тот же указатель
			{
				coll tmp = p->Collision(start, end);
				if (tmp.normal.len2() > 0.f)
					if (ret.point.dist2(start) > tmp.point.dist2(start)) // если нова€ найденна€ коллизи€ ближе к началу. “акое может быть
						ret = tmp;
			}
		}
	}
	// Get From Map

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
	if (end.dist2(point) < radius2)
	{
		coll rez(end, xy(end.x - point.x, end.y - point.y));
		rez.normal.x /= radius;
		rez.normal.y /= radius;
		return rez;
	}
	// TODO: запилить реализацию коллизии
	// Ќа входе стара€ и нова€ точка
	// ≈сли коллизи€ есть, то вернуть еЄ и нормаль к поверхности
	// ≈сли коллизии нет, то вернуть структуру с полем нормали n = 0 (n.x = 0; n.y = 0)
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
	// Ќа входе стара€ и нова€ точка
	// ≈сли коллизи€ есть, то вернуть еЄ и нормаль к поверхности
	// ≈сли коллизии нет, то вернуть структуру с полем нормали n = 0 (n.x = 0; n.y = 0)
	return coll(xy(10.0f, 10.0f), xy00);
}
#endif // !BORDER
