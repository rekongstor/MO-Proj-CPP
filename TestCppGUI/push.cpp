#include "push.h"


Container::Container(): mip(nullptr), best(nullptr), bots(nullptr)
{
}

void Mipmap::Put(const xy& coord, const xy& normal)
{
	int i = (int)(clamp(0.f, coord.x * (float)(l1 - 1), (float)(l1 - 1)));
	int j = (int)(clamp(0.f, coord.y * (float)(l1 - 1), (float)(l1 - 1)));
	if (il1[i][j] == 0) // ускорению сюда ещё не задано
	{
		float nx = normal.x * 1.f;
		float ny = normal.y * 1.f;
		il1[i][j] = 1;
		xyl1[i][j].x = nx;
		xyl1[i][j].y = ny;
		nx *= 0.707f;
		ny *= 0.707f;
		i /= 2;
		j /= 2;
		xyl2[i][j].x = ((xyl2[i][j].x * il2[i][j]) + nx) / (il2[i][j] + 1);
		xyl2[i][j].y = ((xyl2[i][j].y * il2[i][j]) + ny) / (il2[i][j] + 1);
		++il2[i][j];
		nx *= 0.707f;
		ny *= 0.707f;
		i /= 2;
		j /= 2;
		xyl3[i][j].x = ((xyl3[i][j].x * il3[i][j]) + nx) / (il3[i][j] + 1);
		xyl3[i][j].y = ((xyl3[i][j].y * il3[i][j]) + ny) / (il3[i][j] + 1);
		++il3[i][j];
		nx *= 0.707f;
		ny *= 0.707f;
		i /= 2;
		j /= 2;
		xyl4[i][j].x = ((xyl4[i][j].x * il4[i][j]) + nx) / (il4[i][j] + 1);
		xyl4[i][j].y = ((xyl4[i][j].y * il4[i][j]) + ny) / (il4[i][j] + 1);
		++il4[i][j];
	}
}

#pragma optimize( "", off )
const xy& Mipmap::GetA(const xy& coord)
{
	int i = (int)(clamp(0.f, coord.x * (float)(l1 - 1), (float)(l1 - 1)));
	int j = (int)(clamp(0.f, coord.y * (float)(l1 - 1), (float)(l1 - 1)));
	int m, n, c, l;
	// ниже вектора, которые надо усреднять
	xy rez;
	xy ret = xy00;

	// поиск в первом уровне
	if (il1[i][j] == 1)
	{
		ret.x = xyl1[i][j].x * 2.f;
		ret.y = xyl1[i][j].y * 2.f;
		l = 1;
		//return xyl1[i][j];
	}

	// поиск во втором уровне
	c = 0;
	m = (i % 2) * 2 - 1;
	n = (j % 2) * 2 - 1;
	i /= 2;
	j /= 2;
	if (il2[i][j] > 0)
	{
		rez = xyl2[i][j];
		++c;
	}
	if ((i + m >= 0))
	{
		if (il2[i + m][j] > 0)
		{
			rez.x = ((rez.x * c) + xyl2[i + m][j].x) / (c + 1.f) / 2.f;
			rez.y = ((rez.y * c) + xyl2[i + m][j].y) / (c + 1.f) / 2.f;
			++c;
		}
	}
	if ((j + n >= 0))
	{
		if (il2[i][j + n] > 0)
		{
			rez.x = ((rez.x * c) + xyl2[i][j + n].x) / (c + 1.f) / 2.f;
			rez.y = ((rez.y * c) + xyl2[i][j + n].y) / (c + 1.f) / 2.f;
			++c;
		}
	}
	if ((i + m >= 0) && (j + n >= 0))
	{
		if (il2[i + m][j + n] > 0)
		{
			rez.x = ((rez.x * c) + xyl2[i + m][j + n].x) / (c + 1.f) / 2.82842f;
			rez.y = ((rez.y * c) + xyl2[i + m][j + n].y) / (c + 1.f) / 2.82842f;
			++c;
		}
	}
	if (c > 0)
	{
		ret.x += rez.x;
		ret.y += rez.y;
		if (l == 1)
			return ret;
		l = 1;
	}

	// поиск в третьем уровне
	c = 0;
	m = (i % 2) * 2 - 1;
	n = (j % 2) * 2 - 1;
	i /= 2;
	j /= 2;
	if (il3[i][j] > 0)
	{
		rez = xyl3[i][j];
		++c;
	}
	if ((i + m >= 0))
	{
		if (il3[i + m][j] > 0)
		{
			rez.x = ((rez.x * c) + xyl3[i + m][j].x) / (c + 1.f) / 2.f;
			rez.y = ((rez.y * c) + xyl3[i + m][j].y) / (c + 1.f) / 2.f;
			++c;
		}
	}
	if ((j + n >= 0))
	{
		if (il3[i][j + n] > 0)
		{
			rez.x = ((rez.x * c) + xyl3[i][j + n].x) / (c + 1.f) / 2.f;
			rez.y = ((rez.y * c) + xyl3[i][j + n].y) / (c + 1.f) / 2.f;
			++c;
		}
	}
	if ((i + m >= 0) && (j + n >= 0))
	{
		if (il3[i + m][j + n] > 0)
		{
			rez.x = ((rez.x * c) + xyl3[i + m][j + n].x) / (c + 1.f) / 2.82842f;
			rez.y = ((rez.y * c) + xyl3[i + m][j + n].y) / (c + 1.f) / 2.82842f;
			++c;
		}
	}
	if (c > 0)
	{
		ret.x += rez.x;
		ret.y += rez.y;
		if (l == 1)
			return ret;
		l = 1;
	}


	// поиск в четвёртом уровне
	c = 0;
	m = (i % 2) * 2 - 1;
	n = (j % 2) * 2 - 1;
	i /= 2;
	j /= 2;
	if (il4[i][j] > 0)
	{
		rez = xyl4[i][j];
		++c;
	}
	if ((i + m >= 0))
	{
		if (il2[i + m][j] > 0)
		{
			rez.x = ((rez.x * c) + xyl4[i + m][j].x) / (c + 1.f) / 2.f;
			rez.y = ((rez.y * c) + xyl4[i + m][j].y) / (c + 1.f) / 2.f;
			++c;
		}
	}
	if ((j + n >= 0))
	{
		if (il4[i][j + n] > 0)
		{
			rez.x = ((rez.x * c) + xyl4[i][j + n].x) / (c + 1.f) / 2.f;
			rez.y = ((rez.y * c) + xyl4[i][j + n].y) / (c + 1.f) / 2.f;
			++c;
		}
	}
	if ((i + m >= 0) && (j + n >= 0))
	{
		if (il4[i + m][j + n] > 0)
		{
			rez.x = ((rez.x * c) + xyl4[i + m][j + n].x) / (c + 1.f) / 2.82842f;
			rez.y = ((rez.y * c) + xyl4[i + m][j + n].y) / (c + 1.f) / 2.82842f;
			++c;
		}
	}
	if (c > 0)
	{
		ret.x += rez.x;
		ret.y += rez.y;
		return ret;
	}
	return ret;
}
#pragma optimize( "", on )

void Mipmap::Draw(void* gr)
{
	Graphics& graphics = *(Graphics*)gr;
	SolidBrush br(Color(0, 0, 255));
	
	xy xy10(0.f, 1.f);
	xy xy2p3(-.86603f, -.5f);
	xy xy4p3(.86603f, -.5f);
	for (int i = 0; i < l1; ++i)
		for (int j = 0; j < l1; ++j)
		{
			if (il1[i][j] > 0)
			{
				float rg = (1.f + cos(xyl1[i][j], xy10)) / 2.f;
				float gg = (1.f + cos(xyl1[i][j], xy2p3)) / 2.f;
				float bg = (1.f + cos(xyl1[i][j], xy4p3)) / 2.f;
				br.SetColor(Color(rg * 255.f, bg * 255.f, gg * 255.f));
				graphics.FillEllipse(&br, r3.X + i * r3.Width / l1 - 1, r3.Y + (r3.Height - j * r3.Height / l1) - 3, 3, 3);
			}
		}
}

void Mipmap::Clear()
{
	for (auto& i : xyl1)
		for (auto& j : i)
			j = xy00;

	for (auto& i : xyl2)
		for (auto& j : i)
			j = xy00;

	for (auto& i : xyl3)
		for (auto& j : i)
			j = xy00;

	for (auto& i : xyl4)
		for (auto& j : i)
			j = xy00;

	for (auto& i : il1)
		for (auto& j : i)
			j = 0;

	for (auto& i : il2)
		for (auto& j : i)
			j = 0;

	for (auto& i : il3)
		for (auto& j : i)
			j = 0;

	for (auto& i : il4)
		for (auto& j : i)
			j = 0;

	Put(xy11, xy11);
	//Put(xy00, xy11);
}