#pragma once
#include "stdafx.h"

struct Container
{
	Robot_p best;
	Mipmap* mip;
	array<Robot_p, gen_size>* bots;
	Container();
};

struct Mipmap
{
	xy xyl1[l1][l1];
	xy xyl2[l2][l2];
	xy xyl3[l3][l3];
	xy xyl4[l4][l4];
	int il1[l1][l1];
	int il2[l2][l2];
	int il3[l3][l3];
	int il4[l4][l4];
	void Draw(void* gr);
	void Clear();
	const xy& GetA(const xy& coord);
	void Put(const xy& coord, const xy& normal);
};
