#include "push.h"


Container::Container(): mip(nullptr), best(nullptr), bots(nullptr)
{}

void Mipmap::Put(const xy& coord, const xy& normal)
{
	int i = (int)(clamp(0.f, coord.x * (float)(l1 - 1), (float)(l1 - 1)));
	int j = (int)(clamp(0.f, coord.y * (float)(l1 - 1), (float)(l1 - 1)));
	if (xyl1[i][j].len2() == 0.f) // ускорению сюда ещё не задано
	{
		xyl1[i][j].x = normal.x * 1.f;
		xyl1[i][j].y = normal.x * 1.f;
		i /= 2;
		j /= 2;
		xyl2[i][j].x += normal.x * .5f;
		xyl2[i][j].y += normal.x * .5f;
		i /= 2;
		j /= 2;
		xyl3[i][j].x += normal.x * .25f;
		xyl3[i][j].y += normal.x * .25f;
		i /= 2;
		j /= 2;
		xyl4[i][j].x += normal.x * .125f;
		xyl4[i][j].y += normal.x * .125f;
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
	for (int i = 0; i < l2; ++i)
		for (int j = 0; j < l2; ++j)
		{
			if (xyl2[i][j].len2() > 0.f)
			{
				if (j == l2 - 1)
					j = l2 - 1;
				br.SetColor(Color((xyl2[i][j].x + 1.f) * (float)(l1 - 1), ((xyl2[i][j].y + 1.f) * (float)(l1 - 1)), 0));
				graphics.FillEllipse(&br, 2 * padding + reg_w + i * reg_w / l2 - max_depth / 2 + 2, padding + (reg_w - j * reg_w / l2) - max_depth - 2, max_depth + 2, max_depth + 2);
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
}