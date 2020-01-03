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
		float na = a.len();
		float nn = speed.len();
		float push_power = 0.f;
		xy push(0.f,0.f);
		// меняем ускорение с учётом отталкивания. может рили его кукошить?
		if (!(coord.x > 0.97f && coord.y > 0.97f) && !(coord.x < 0.03f && coord.y < 0.03f))
		{
			auto& m = mipmap;
			push = m.GetA(coord);
			if (push.len2() > 0.f && speed.len2() > 0.f)
			{
				a.x /= na;
				a.y /= na;
				push_power = nn * push.len() * cos(speed, push) / dt;
				a.x -= push.x * push_power;
				a.y -= push.y * push_power;
				float nnn = a.len();
				a.x = a.x / nnn * na;
				a.y = a.y / nnn * na;
				break;
			}
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
			SolidBrush br(Color(static_cast<int>((push.x + 1.f) * 127.f), static_cast<int>((push.y + 1.f) * 127.f), static_cast<int>(push.len() * 255.f), 32));
			graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 3), (int)((1.f - coord.y) * reg_w + padding) - 3, 7, 7);
			br.SetColor(Color(static_cast<int>((speed.x + 1.f) * 127.f), static_cast<int>((speed.y + 1.f) * 127.f), static_cast<int>(speed.len() * 255.f), 64));
			graphics.FillEllipse(&br, (int)((coord.x * reg_w + padding) - 2), (int)((1.f - coord.y) * reg_w + padding) - 2, 5, 5);
			br.SetColor(Color(0, 0, 0));
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