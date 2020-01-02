#include "robot.h"
#include "field.h"
#include "push.h"

Robot::Robot(): coord(0.,0.), speed(0.,0.), life_time(0.), q(QT()), fin_dist2(10.), c(xyxx,xyxx)
{}

Robot::Robot(const Robot& r) : coord(r.coord), speed(r.speed), life_time(r.life_time), q(QT(r.q)), fin_dist2(r.fin_dist2), c(r.c)
{
	q.Split(0.f, 0.f);
}

Robot::~Robot()
{}

void Robot::Simulate(void* gr)
{
	coord.x = 0.f;
	coord.y = 0.f;
	speed.x = 0.f;
	speed.y = 0.f;
	life_time = 0.f;
	bool stopped = false;
	int n = 0;
	xy old_coord(coord);
	QT* leaf;
	while (!stopped)
	{
		if (++n > robot_steps)
			stopped = true;
		life_time += dt;
		coord.x += speed.x * dt;
		coord.y += speed.y * dt;
		leaf = q.Get(coord.x, coord.y);
		xy a = leaf->a; // узнаём ускорение
		// меняем ускорение с учётом отталкивания. может рили его кукошить?
		for (auto& m : mipmap)
		{
			xy push = m.GetA(coord);
			if (push != xy00)
			{
				a.x += push.x;
				a.y += push.y;
				break;
			}
		}

		speed.x += Fmax * leaf->a.x * dt;
		speed.y += Fmax * leaf->a.y * dt;
		leaf->time = life_time;

		c = field->Collision(old_coord, coord);
		if (c.point.x <9.f)
			stopped = true;

		if (gr)
		{
			Graphics& graphics = *(Graphics*)gr;
			SolidBrush br(Color(0, 0, 0));
			
			graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 1), (int)((1.f - coord.y) * reg_w + padding) - 1, 3, 3);

		}

		old_coord = coord;
	}
	fin_dist2 = coord.dist2(xy11);
}

void Robot::DrawQT(void* gr)
{
	q.Draw(gr, life_time);
}