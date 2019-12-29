#include "quadtree.h"

void QT::Draw(void* gr)
{
	if (child00 != nullptr)
	{
		child00->Draw(gr);
		child01->Draw(gr);
		child10->Draw(gr);
		child11->Draw(gr);
	}
	else
	{
		Graphics& graphics = *(Graphics*)gr;
		SolidBrush br(Color((int)((a.x + 1.) / 2. * 255.), (int)((a.y + 1.) / 2. * 255.), 255));
		graphics.FillRectangle(&br, 2 * padding + reg_w + (int)(point.x * reg_w), padding + (int)(((1 - point.y - w) * reg_w) ), (int)(w * reg_w), (int)(w * reg_w));
	}
}

QT::QT() :
	point(0., 0.),
	w(1.),
	a(sqrt(.5), sqrt(.5)),
	depth(1),
	child00(nullptr),
	child01(nullptr),
	child10(nullptr),
	child11(nullptr),
	time(0.),
	fi(PI/4.)
{
}

QT::QT(xy point_, double w_, xy a_, int depth_, double time_, double fi_) : 
	point(point_),
	w(w_),
	a(a_),
	depth(depth_),
	child00(nullptr),
	child01(nullptr),
	child10(nullptr),
	child11(nullptr),
	time(time_),
	fi(fi_)
{
}

QT* QT::Get(double x_, double y_)
{
	if (child00 != nullptr)
	{
		if (x_ < point.x + w / 2.)
			if (y_ < point.y + w / 2.)
				return child00->Get(x_, y_);
			else
				return child01->Get(x_, y_);
		else
			if (y_ < point.y + w / 2.)
				return child10->Get(x_, y_);
			else
				return child11->Get(x_, y_);
	}
	return this;
}

void QT::Split(double x_, double y_)
{
	QT* l = Get(x_, y_);
	if (l->depth < max_depth)
	{
		l->child00 = new QT(xy(l->point.x, l->point.y), l->w / 2., l->a, l->depth + 1, l->time, fi);
		l->child01 = new QT(xy(l->point.x, l->point.y + l->w / 2.), l->w / 2., l->a, l->depth + 1, l->time, fi);
		l->child10 = new QT(xy(l->point.x + l->w / 2., l->point.y), w / 2., l->a, l->depth + 1, l->time, fi);
		l->child11 = new QT(xy(l->point.x + l->w / 2., l->point.y + l->w / 2.), l->w / 2., l->a, l->depth + 1, l->time, fi);
	}
}

void QT::Randomize(double full_time)
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
		// Random fi +- 15'
		// ax = cos(fi)
		// ay = sin(fi)
		// TODO: Полярные координаты
		a.x = (.5 - Random()) * 2.;
		a.y = (.5 - Random()) * 2.;
	}
}