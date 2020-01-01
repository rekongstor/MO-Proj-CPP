#pragma once
#include <set>
#include <vector>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <random>
#include <cmath>
#include <thread>
#include <array>
#include <iostream>
#include <string>

const float dt = 0.01f;
const float Fmax = 1.;

const int max_depth = 7;
const int l1 = 256;
const int l2 = 128;
const int l3 = 64;
const int l4 = 32;

const int threads = 8;
const int gen_size = 4096 / threads;
const int max_tries = 1024;
const float finish_dist = 0.01f;
const float finish_dist2 = finish_dist * finish_dist;


#define threading

const float PI = 3.141592653589793238463f;

using namespace Gdiplus;
using namespace std;

struct Robot;
struct QT;
struct Field;
struct Container;
struct Mipmap;



struct xy
{
	float x, y;
	xy(float _x, float _y) : x(_x), y(_y) {};
	xy() : x(0.), y(0.) {};
	float len();
	float len2();
	float dist(xy dest);
	float dist2(xy dest);
	bool operator!=(const xy&);
};

const xy xy00(0., 0.);
const xy xy11(1., 1.);
const xy xyxx(10., 10.);

struct coll
{
	xy point;						// Точка на плоскости 
	xy normal;						// Вектор нормали
	coll(xy point_, xy normal_) : point(point_), normal(normal_) {};
};

const int reg_w = 512;			// размеры регионов
//const int small_zones[] =	{ 4, 8 };
const int small_zones[] =	{ 40, 80 };
const float small_zone_size[] = { 0.01, 0.03 };
//const int big_zones[] =		{ 1, 1 };
const int big_zones[] =		{ 8, 16 };
const float big_zone_size[] = { 0.05, 0.18 };


const int padding = reg_w / 20;	// отступ

const Rect r1(padding, padding, reg_w, reg_w);
const Rect r2(reg_w + 2 * padding, padding, reg_w, reg_w);

const WCHAR Tip[] = L"Press 'G' to regenerate field\nPress 'S' to simulate movement"; //\mPress 'Q' to stop simulation\0";


int Random(int min, int max);

float Random();

float clamp(float min, float x, float max);

void Simulate(Container&);
