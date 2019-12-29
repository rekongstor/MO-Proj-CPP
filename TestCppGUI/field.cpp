#include <objidl.h>
#include <gdiplus.h>
#include "field.h"
using namespace Gdiplus;

#ifndef FIELD
Field::Field()
{
	obstacles.push_back(new Border()); // создаём одну границу, чтобы можно было проверять её коллизию полиморфно
	obstacles.push_back(new Zone(xy(0.0,0.0),0.1));
}


void Field::Draw(void* gr)
{
	for (auto o : obstacles)
		o->Draw(gr);
}
#endif // !FIELD

#ifndef ZONE
void Zone::Draw(void* gr)
{
	Graphics& graphics = *(Graphics*)gr;
	SolidBrush br(Color(255, 0, 0));
	int pixel_r = r * reg_w;
	graphics.FillEllipse(&br, 
		reg1_x + p.x * reg_w - pixel_r, 
		reg1_y + (1. - p.y) * reg_w - pixel_r,
		pixel_r * 2,
		pixel_r * 2);
}

coll Zone::Collision(xy start, xy end)
{
	return coll(xy(0,0),xy(0,0));
}

Zone::~Zone()
{

}
#endif // !ZONE

#ifndef BORDER
coll Border::Collision(xy start, xy end)
{
	return coll(xy(0, 0), xy(0, 0));
}

#endif // !BORDER
