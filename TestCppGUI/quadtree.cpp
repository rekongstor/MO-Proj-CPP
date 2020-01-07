#include "quadtree.h"

void QT::Draw(void* gr, float& full_time)
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
		xy xy10(0.f, 1.f);
		xy xy2p3(-.86603f, -.5f);
		xy xy4p3(.86603f, -.5f);
		float rg = (1.f + cos(a, xy10)) / 2.f;
		float gg = (1.f + cos(a, xy2p3)) / 2.f;
		float bg = (1.f + cos(a, xy4p3)) / 2.f;
		float time_coef = 1.f;// time / full_time * time / full_time;
		Graphics& graphics = *(Graphics*)gr;
		SolidBrush br(Color((int)(rg * 255.f * time_coef), (int)(bg * 255.f * time_coef), (int)(gg * 255.f * time_coef)));
		//if (full_time > 0.f)
		//	br.SetColor(Color((int)((a.x + 1.f) / 2.f * 255.f), (int)((a.y + 1.f) / 2.f * 255.f), (int)(time / full_time * 255.f)));

		graphics.FillRectangle(&br, r2.X + (int)(point.x * r2.Width), r2.Y + (int)(((1 - point.y - w) * r2.Height)), (int)(w * r2.Width), (int)(w * r2.Height));
	}
}
void QT::CleanTimes(float best_distance)
{
	if (child00 != nullptr)
	{
		child00->CleanTimes(best_distance);
		child01->CleanTimes(best_distance);
		child10->CleanTimes(best_distance);
		child11->CleanTimes(best_distance);
	}
	else
	{
		if (!been)
			time = 0.f;
		else
			bestest_dist = best_distance;
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
	power(1.f),
	been(false),
	bestest_dist(10.f)
{}


QT::QT(xy point_, float w_, xy& a_, int& depth_, float& time_, float& fi_, float& power_, bool& been_, float& bd) :
	point(point_),
	w(w_ / 2.f),
	a(a_),
	depth(depth_ + 1),
	child00(nullptr),
	child01(nullptr),
	child10(nullptr),
	child11(nullptr),
	time(time_),
	fi(fi_),
	power(power_),
	been(been_),
	bestest_dist(bd)
{}

void QT::Copy(QT& q)
{
	// рекурсивно спуститься в листья
	if (child00 != nullptr)
	{
		child00->Copy(*q.child00);
		child01->Copy(*q.child01);
		child10->Copy(*q.child10);
		child11->Copy(*q.child11);
	}
	else
	{
		if (q.been && (bestest_dist > q.bestest_dist))
		{
			a = q.a;
			fi = q.fi;
			power = q.power;
			bestest_dist = q.bestest_dist;
		}
		been = false;
	}
}

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
	power(q.power),
	been(q.been),
	bestest_dist(q.bestest_dist)
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

QT* QT::Get(float& x_, float& y_)
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

void QT::Split(float& x_, float& y_)
{
	QT* l = Get(x_, y_);
	if (l->depth < max_depth)
	{
		l->child00 = make_shared<QT>(
			xy(l->point.x, l->point.y), 
			l->w, 
			l->a, 
			l->depth, 
			l->time, 
			l->fi,
			l->power,
			l->been,
			l->bestest_dist);
		l->child01 = make_shared<QT>(
			xy(l->point.x, l->point.y + l->w / 2.f), 
			l->w, 
			l->a, 
			l->depth, 
			l->time, 
			l->fi,
			l->power,
			l->been,
			l->bestest_dist);
		l->child10 = make_shared<QT>(
			xy(l->point.x + l->w / 2.f, l->point.y),
			l->w, 
			l->a, 
			l->depth, 
			l->time, 
			l->fi,
			l->power,
			l->been,
			l->bestest_dist);
		l->child11 = make_shared<QT>(
			xy(l->point.x + l->w / 2.f, l->point.y + l->w / 2.f), 
			l->w, 
			l->a, 
			l->depth, 
			l->time, 
			l->fi,
			l->power,
			l->been,
			l->bestest_dist);
	}
}

void QT::Randomize(float& full_time)
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

			fi += (time_coef * time_coef * time_coef * (Random() - 0.5f) * PI);

			// можно менять листы, где он не был, 
			// но где будут неудавшиеся роботы из новых поколений - 
			// это поможет расширить кругозор по оттакливаниям
			if (time_coef == 0.f)
				fi += ((Random() - 0.5f) * .25f * PI);
			
			// 20% листов просто рандомизируем вне зависимости от времени. Эта штука сильно зависит от количества потомков
			if (Random() < 0.2f) 
				fi = ((Random()) * 2.f * PI);

			if (fi > 2 * PI)
				fi -= 2 * PI;
			if (fi < 0.f)
				fi += 2 * PI;
			power = clamp(0.05f, power + time_coef * time_coef * (Random() - 0.5f), 5.f);
			a.x = sin(fi) * power;
			a.y = cos(fi) * power;
			//time = 0.f;
		}
	}
}