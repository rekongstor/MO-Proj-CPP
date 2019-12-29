#pragma once
#include <set>
#include <vector>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
using namespace std;


struct xy
{
	double x, y;
	xy(double _x, double _y) : x(_x), y(_y) {};
};

struct coll
{
	xy p; // Point 
	xy n; // Normal
	coll(xy point, xy normal) : p(point), n(normal) {};
};

const int reg_w = 500; // размеры регионов

const int reg1_x = 20; // координаты первого региона рисования
const int reg1_y = 20;

const int reg2_x = reg_w + 40; // координаты второго региона рисования
const int reg2_y = 20;

const int text_x = 20;
const int text_y = reg_w + 40;

const Rect r1(reg1_x, reg1_y, reg_w, reg_w);
const Rect r2(reg2_x, reg2_y, reg_w, reg_w);

const WCHAR Tip[] = L"Press 'G' to regenerate field\nPress 'S' to simulate movement\nPress 'Q' to stop simulation\0";