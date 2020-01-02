#include "push.h"


Container::Container(): mip(nullptr), best(nullptr), bots(nullptr)
{}

const xy& Mipmap::GetA(const xy& coord)
{
	int i = (int)(coord.x * (float)(l1 - 1));
	int j = (int)(coord.y * (float)(l1 - 1));
	if (xyl1[i][j] != xy00)
		return xyl1[i][j];
	i /= 2;
	j /= 2;
	if (xyl2[i][j] != xy00)
		return xyl1[i][j];
	i /= 2;
	j /= 2;
	if (xyl3[i][j] != xy00)
		return xyl1[i][j];
	i /= 2;
	j /= 2;
	if (xyl4[i][j] != xy00)
		return xyl1[i][j];
	return xy00;
}

void Mipmap::Draw(void* gr)
{
	Graphics& graphics = *(Graphics*)gr;
	SolidBrush br(Color(0, 0, 255));
	for (int i = 0; i < l2; ++i)
		for (int j = 0; j < l2; ++j)
		{
			if (xyl2[i][j] != xy00)
			{
				br.SetColor(Color((xyl2[i][j].x + 1.f) * 127.f, ((xyl2[i][j].y + 1.f) * 127.f), 0));
				graphics.FillEllipse(&br, 2 * padding + reg_w + i * reg_w / l2 - max_depth / 2, padding + (reg_w - j * reg_w / l2) - max_depth / 2, max_depth + 2, max_depth + 2);
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