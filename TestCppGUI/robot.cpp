#include "robot.h"
#include "field.h"
#include "push.h"

Robot::Robot(): coord(0.,0.), speed(0.,0.), life_time(0.), q(QT()), fin_dist2(10.), c(xyxx,xyxx)
{
	q.Split(0.f, 0.f);
	q.Split(0.f, 0.f);
}

Robot::Robot(const Robot& r) : coord(r.coord), speed(r.speed), life_time(r.life_time), q(QT(r.q)), fin_dist2(r.fin_dist2), c(r.c)
{
	//q.Split(0.f, 0.f);
}

Robot::~Robot()
{}

Robot& Robot::operator=(const Robot& r)
{
	coord = r.coord;
	speed = r.speed;
	life_time = r.life_time;
	return *this;
}

void Robot::Simulate(void* gr)
{
	coord.x = 0.f;
	coord.y = 0.f;
	speed.x = 0.f;
	speed.y = 0.f;
	float rand_time = life_time;
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
		if (!leaf->been)
		{
			leaf->Randomize(rand_time);
			leaf->been = true;
		}
		xy a = leaf->a; // узнаём ускорение
		
		float nn = speed.len();
		xy push(0.f,0.f);
		// меняем ускорение с учётом отталкивания. может рили его кукошить?
		push = mipmap.GetA(coord);
		if (push.len2() > 0.f && speed.len2() > 0.f)
		{
			float push_power;
			a.x /= leaf->power;
			a.y /= leaf->power;
			push_power = clamp(-.01f, nn * push.len() * cos(speed, push), .01f);
			a.x -= push.x * push_power;
			a.y -= push.y * push_power;
			float nnn = a.len();
			a.x = a.x / nnn * leaf->power;
			a.y = a.y / nnn * leaf->power;
		}

		speed.x += Fmax * a.x * dt;
		speed.y += Fmax * a.y * dt;
		leaf->time = life_time;

		c = field->Collision(old_coord, coord);
		if (c.point.x < 9.f)
			stopped = true;

		if (gr)
		{
			Graphics& graphics = *(Graphics*)gr;
			SolidBrush br(Color(0, static_cast<int>(push.len() * 255.f), static_cast<int>(push.len() * 255.f)));
			graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 2), (int)((1.f - coord.y) * reg_w + padding) - 2, 5, 5);
			//br.SetColor(Color(static_cast<int>((speed.x + 1.f) * 127.f), static_cast<int>((speed.y + 1.f) * 127.f), static_cast<int>(speed.len() * 255.f), 64));
			//graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 2), (int)((1.f - coord.y) * reg_w + padding) - 2, 5, 5);
			//br.SetColor(Color(0, 0, 0));
			//graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 1), (int)((1.f - coord.y) * reg_w + padding) - 1, 3, 3);
		}

		old_coord = coord;
	}
	fin_dist2 = coord.dist2(xy11);
}

void Robot::DrawQT(void* gr)
{
	q.Draw(gr, life_time);
}