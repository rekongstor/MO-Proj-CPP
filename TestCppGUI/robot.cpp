#include "robot.h"
#include "field.h"

Robot::Robot(): coord(0.,0.), speed(0.,0.), life_time(0.), q(QT()), fin_dist2(10.)
{}

Robot::Robot(const Robot& r) : coord(r.coord), speed(r.speed), life_time(r.life_time), q(QT(r.q)), fin_dist2(r.fin_dist2)
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
		if (++n > 5000)
			stopped = true;
		life_time += dt;
		coord.x += speed.x * dt;
		coord.y += speed.y * dt;
		leaf = q.Get(coord.x, coord.y);
		speed.x += Fmax * leaf->a.x * dt;
		speed.y += Fmax * leaf->a.y * dt;
		leaf->time = life_time;

		if (field->Collision(old_coord, coord).normal.len2() > 0.f)
			stopped = true;

		if (gr)
		{
			Graphics& graphics = *(Graphics*)gr;
			SolidBrush br(Color(0, 0, 0));
			
			graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 1), (int)((1.f - coord.y) * reg_w + padding) - 1, 3, 3);

		}

		old_coord = coord;
	}
	if (!gr)
	{
		q.Split(coord.x, coord.y);
	}
	fin_dist2 = coord.dist2(xy11);
}

void Robot::DrawQT(void* gr)
{
	q.Draw(gr, life_time);
}