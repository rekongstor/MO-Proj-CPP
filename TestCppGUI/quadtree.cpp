#include "quadtree.h"

void QT::Draw(void* gr, float full_time)
{
	if (child00 != nullptr)
	{
		child00->Draw(gr, full_time);
		child01->Draw(gr, full_time);
		child10->Draw(gr, full_time);
		child11->Draw(gr, full_time);
	}
	else
	{
		Graphics& graphics = *(Graphics*)gr;
		SolidBrush br(Color((int)((a.x + 1.f) / 2.f * 255.f), (int)((a.y + 1.f) / 2.f * 255.f), 255));
		if (full_time > 0.f)
			br.SetColor(Color((int)((a.x + 1.f) / 2.f * 255.f), (int)((a.y + 1.f) / 2.f * 255.f), (int)(time / full_time * 255.f)));

		graphics.FillRectangle(&br, 2 * padding + reg_w + (int)(point.x * reg_w), padding + (int)(((1 - point.y - w) * reg_w)), (int)(w * reg_w), (int)(w * reg_w));
	}
}

QT::QT() :
	point(0.f, 0.f),
	w(1.f),
	a(sqrt(.5f), sqrt(.5f)),
	depth(0),
	child00(nullptr),
	child01(nullptr),
	child10(nullptr),
	child11(nullptr),
	time(0.f),
	fi(PI/4.f),
	power(1.f)
{}

QT::QT(xy point_, float w_, xy a_, int depth_, float time_, float fi_, float power_) :
	point(point_),
	w(w_),
	a(a_),
	depth(depth_),
	child00(nullptr),
	child01(nullptr),
	child10(nullptr),
	child11(nullptr),
	time(time_),
	fi(fi_),
	power(power_)
{}

QT::QT(const QT& q):
	point(q.point),
	w(q.w),
	a(q.a),
	depth(q.depth),
	child00(nullptr),
	child01(nullptr),
	child10(nullptr),
	child11(nullptr),
	time(q.time),
	fi(q.fi),
	power(q.power)
{
	if (q.child00)
		child00 = make_shared<QT>(*q.child00);
	if (q.child01)
		child01 = make_shared<QT>(*q.child01);
	if (q.child10)
		child10 = make_shared<QT>(*q.child10);
	if (q.child11)
		child11 = make_shared<QT>(*q.child11);
}

QT::~QT()
{
}

QT* QT::Get(float x_, float y_)
{
	if (child00 != nullptr)
	{
		if (x_ < point.x + w / 2.f)
			if (y_ < point.y + w / 2.f)
				return child00->Get(x_, y_);
			else
				return child01->Get(x_, y_);
		else
			if (y_ < point.y + w / 2.f)
				return child10->Get(x_, y_);
			else
				return child11->Get(x_, y_);
	}
	return (this);
}

void QT::Split(float x_, float y_)
{
	QT* l = Get(x_, y_);
	if (l->depth < max_depth)
	{
		l->child00 = make_shared<QT>(
			xy(l->point.x, l->point.y), 
			l->w / 2.f, 
			l->a, 
			l->depth + 1, 
			l->time, 
			l->fi,
			l->power);
		l->child01 = make_shared<QT>(
			xy(l->point.x, l->point.y + l->w / 2.f), 
			l->w / 2.f, 
			l->a, 
			l->depth + 1, 
			l->time, 
			l->fi,
			l->power);
		l->child10 = make_shared<QT>(
			xy(l->point.x + l->w / 2.f, l->point.y),
			l->w / 2.f, 
			l->a, 
			l->depth + 1, 
			l->time, 
			l->fi,
			l->power);
		l->child11 = make_shared<QT>(
			xy(l->point.x + l->w / 2.f, l->point.y + l->w / 2.f), 
			l->w / 2.f, 
			l->a, 
			l->depth + 1, 
			l->time, 
			l->fi,
			l->power);
	}
}

void QT::Randomize(float full_time)
{
	if (full_time > 0.)
	{
		if (child00 != nullptr)
		{
			child00->Randomize(full_time);
			child01->Randomize(full_time);
			child10->Randomize(full_time);
			child11->Randomize(full_time);
		}
		else
		{
			float time_coef = time / full_time; // pow(time / full_time, 1.f);

			//if (time_coef * Random() < 0.50f / depth)
			//{
			//	fi += (time_coef * (Random() - 0.5f) * PI) + (Random() - 0.5f) * 1.20f * (float)depth;

			//	if (fi > 2 * PI)
			//		fi -= 2 * PI;
			//	if (fi < 0.f)
			//		fi += 2 * PI;
			//	power = clamp(0.1f, power + time_coef * (Random() - 0.5f), 1.f);
			//	a.x = sin(fi) * power;
			//	a.y = cos(fi) * power;
			//}


			fi += (time_coef * time_coef * (Random() - 0.5f) * PI) + (Random() - 0.5f) * 0.01f * (float)depth;

			if (fi > 2 * PI)
				fi -= 2 * PI;
			if (fi < 0.f)
				fi += 2 * PI;
			power = clamp(0.1f, power + time_coef * time_coef * (Random() - 0.5f), 1.f);
			a.x = sin(fi) * power;
			a.y = cos(fi) * power;
		}
	}
}