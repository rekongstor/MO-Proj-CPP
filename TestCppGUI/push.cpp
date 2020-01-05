#include "push.h"


Container::Container(): mip(nullptr), best(nullptr), bots(nullptr)
{}

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
		nx /= 0.25f;
		ny /= 0.25f;
		i /= 2;
		j /= 2;
		xyl2[i][j].x = ((xyl2[i][j].x * il2[i][j]) + nx) / (il2[i][j] + 1);
		xyl2[i][j].y = ((xyl2[i][j].y * il2[i][j]) + ny) / (il2[i][j] + 1);
		++il2[i][j];
		nx /= 0.25f;
		ny /= 0.25f;
		i /= 2;
		j /= 2;
		xyl3[i][j].x = ((xyl3[i][j].x * il3[i][j]) + nx) / (il3[i][j] + 1);
		xyl3[i][j].y = ((xyl3[i][j].y * il3[i][j]) + ny) / (il3[i][j] + 1);
		++il3[i][j];
		nx /= 0.25f;
		ny /= 0.25f;
		i /= 2;
		j /= 2;
		xyl4[i][j].x = ((xyl4[i][j].x * il4[i][j]) + nx) / (il4[i][j] + 1);
		xyl4[i][j].y = ((xyl4[i][j].y * il4[i][j]) + ny) / (il4[i][j] + 1);
		++il4[i][j];
	}
}

const xy& Mipmap::GetA(const xy& coord)
{
	int i = (int)(clamp(0.f, coord.x, 1.f) * (float)(l1 - 1));
	int j = (int)(clamp(0.f, coord.y, 1.f) * (float)(l1 - 1));

	if (xyl1[i][j].len2() > 0.f)
		return xyl1[i][j];
	i /= 2;
	j /= 2;
	if (xyl2[i][j].len2() > 0.f)
		return xyl2[i][j];
	i /= 2;
	j /= 2;
	if (xyl3[i][j].len2() > 0.f)
		return xyl3[i][j];
	i /= 2;
	j /= 2;
	if (xyl4[i][j].len2() > 0.f)
		return xyl4[i][j];
	return xy00;
}

void Mipmap::Draw(void* gr)
{
	Graphics& graphics = *(Graphics*)gr;
	SolidBrush br(Color(0, 0, 255));
	for (int i = 0; i < l1; ++i)
		for (int j = 0; j < l1; ++j)
		{
			if (xyl1[i][j].len2() > 0.f)
			{
				xy xy10(0.f, 1.f);
				xy xy2p3(-.86603f, -.5f);
				xy xy4p3(.86603f, -.5f);
				float rg = (1.f+cos(xyl1[i][j],xy10))/2.f;
				float gg = (1.f+cos(xyl1[i][j], xy2p3))/2.f;
				float bg = (1.f+cos(xyl1[i][j], xy4p3))/2.f;
				br.SetColor(Color(rg * 255.f, bg * 255.f, gg * 255.f));
				graphics.FillEllipse(&br, 2 * padding + reg_w + i * reg_w / l1 - 1, padding + (reg_w - j * reg_w / l1) - 3, 3, 3);
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

	Put(xy11, xyxx);
}