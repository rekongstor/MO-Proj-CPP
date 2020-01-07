#include "robot.h"
#include "field.h"
#include "push.h"

Robot::Robot(): coord(0.,0.), speed(0.,0.), life_time(0.), q(QT()), fin_dist2(10.), c(xyxx,xyxx)
{
	// ����� ������ ������ ������ �����
	float c1 = .5f;
	float c21 = .25f, c22 = .75f;
	q.Split(c1, c1); // � ��������
	q.Split(c21, c21); // �� ����� ���������
	q.Split(c21, c22);
	q.Split(c22, c21);
	q.Split(c22, c22);
	// ����� �������� �������!
	float c31 = .25f - .01f;
	float c32 = .25f + .01f;
	float c33 = .75f - .01f;
	float c34 = .75f + .01f;
	q.Split(c31, c31); 
	q.Split(c31, c32); 
	q.Split(c32, c31); 
	q.Split(c32, c32); 

	q.Split(c31, c33);
	q.Split(c31, c34);
	q.Split(c32, c33);
	q.Split(c32, c34);

	q.Split(c33, c31);
	q.Split(c33, c32);
	q.Split(c34, c31);
	q.Split(c34, c32);

	q.Split(c33, c33);
	q.Split(c33, c34);
	q.Split(c34, c33);
	q.Split(c34, c34);
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
		if (!leaf->been && !gr)
		{
			leaf->Randomize(rand_time);
			leaf->time = 0.f;
			leaf->been = true;
		}
		xy a = leaf->a; // ����� ���������
		
		float nn = speed.len();
		xy push(0.f,0.f);
		// ������ ��������� � ������ ������������. ����� ���� ��� ��������?
		push = mipmap.GetA(coord);
		float push_len = push.len();
		float push_power = 0.f;
		if (push_len > 0.f && nn > 0.f)
		{
			push_power = nn * push_len * cos(speed, push);
			if (push_power < 0.f)
			{
				a.x /= leaf->power;
				a.y /= leaf->power;
				push.x /= push_len;
				push.y /= push_len;
				a.x -= push.x * push_power / dt * 0.5f;
				a.y -= push.y * push_power / dt * 0.5f;
				float nnn = a.len();
				float push_factor = clamp(0.f, push_len, 1.f) * 2.f;
				a.x = a.x / nnn * leaf->power * push_factor;
				a.y = a.y / nnn * leaf->power * push_factor;
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
			xy xy10(0.f, 1.f);
			xy xy2p3(-.86603f, -.5f);
			xy xy4p3(.86603f, -.5f);
			float rg = (1.f + cos(push, xy10)) / 2.f;
			float gg = (1.f + cos(push, xy2p3)) / 2.f;
			float bg = (1.f + cos(push, xy4p3)) / 2.f;
			Graphics& graphics = *(Graphics*)gr;
			//push_len = clamp(0.f, abs(push_power / dt),1.f);
			SolidBrush br(Color(
				(rg * push_len * 255.f), //clamp(0.f, abs(push_power), 1.f) * 
				(gg * push_len * 255.f),
				(bg * push_len * 255.f)));
			//br.SetColor(Color(rg * 255.f, bg * 255.f, gg * 255.f));
			graphics.FillEllipse(&br, (int)((coord.x * r1.Width + r1.X)) - 2, (int)((1.f - coord.y) * r1.Height + r1.Y) - 2, 5, 5);
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