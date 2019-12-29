#pragma once
#include <set>
#include <vector>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <random>
using namespace Gdiplus;
using namespace std;

struct Rnd
{
	Rnd();
};

const Rnd ra; // эта штука должна сто€ть тут, потому что она активирует конструктор, который задаЄт рандом

struct xy
{
	double x, y;
	xy(double _x, double _y) : x(_x), y(_y) {};
	double len();
	double dist(xy dest);
};

struct coll
{
	xy p;						// “очка на плоскости 
	xy n;						// ¬ектор нормали
	coll(xy point, xy normal) : p(point), n(normal) {};
};

const int reg_w = 400;			// размеры регионов
const int small_zones[] =	{ 40, 80 };
const double small_zone_size[] = { 0.01, 0.03 };
const int big_zones[] =		{ 8, 16 };
const double big_zone_size[] = { 0.05, 0.18 };


const int padding = reg_w / 20;	// отступ

const Rect r1(padding, padding, reg_w, reg_w);
const Rect r2(reg_w + 2 * padding, padding, reg_w, reg_w);

const WCHAR Tip[] = L"Press 'G' to regenerate field\nPress 'S' to simulate movement\nPress 'Q' to stop simulation\0";


int Random(int min, int max);

double Random();
