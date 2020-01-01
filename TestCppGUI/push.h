#pragma once
#include "stdafx.h"

struct Container
{
	Robot* best;
	Mipmap* mip;
	Container();
	void setmm(Mipmap*);
};

struct Mipmap
{
	xy xyl1[l1][l1];
	xy xyl2[l2][l2];
	xy xyl3[l3][l3];
	xy xyl4[l4][l4];
	void Draw(void* gr);
	void Clear();
};
