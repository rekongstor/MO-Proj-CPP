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
#include <memory>
#include <map>

const float dt = 0.01f;
const float Fmax = 1.;

const int max_depth = 6;
const int l1 = 1 << (max_depth + 1);
const int l2 = l1 >> 1;
const int l3 = l2 >> 1;
const int l4 = l3 >> 1;

const int robot_steps = 4096;

const int threads = 8;
const int gen_size = 1024 * 32 / threads;
const int max_tries = 512;
const float finish_dist = 0.01f;
const float finish_dist2 = finish_dist * finish_dist;


#define threading // многопоточка
#define true_random // рандом через mt19937. Не выключать при включенном threading, иначе нет смысла от нескольких ядер
#define alwaysdraw // Перерисовывать экран каждое поколение. Если убрать, то алгоритм будет работать быстрее

#ifndef true_random
struct Rnd
{
	Rnd();
};
#endif

const float PI = 3.141592741f;

using namespace Gdiplus;
using namespace std;

struct Robot;
struct QT;
struct Field;
struct Border;
struct Zone;
struct Container;
struct Mipmap;


using Robot_p = shared_ptr<Robot>;
using QT_p = shared_ptr<QT>;
using Field_p = shared_ptr<Field>;
using Border_p = shared_ptr<Border>;
using Zone_p = shared_ptr<Zone>;

struct xy
{
	float x, y;
	xy(float _x, float _y) : x(_x), y(_y) {};
	xy() : x(0.), y(0.) {};
	float len();
	float len2();
	float dist(xy dest);
	float dist2(xy dest);
	inline bool operator!=(const xy&);
	xy operator*(const float&);
	void operator+=(const xy&&);
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

const int reg_w = 256;			// размеры регионов
//const int small_zones[] =	{ 1, 1 };
//const int small_zones[] =	{ 4, 8 };
const int small_zones[] =	{ 50, 80 };
const float small_zone_size[] = { 0.03f, 0.05f };
//const int big_zones[] =		{ 1, 1 };
const int big_zones[] =		{ 4, 8 };
const float big_zone_size[] = { 0.10f, 0.20f };


const int padding = 20;	// отступ

const Rect r1(padding, padding, reg_w * 2, reg_w * 2);
const Rect r2(reg_w * 2 + 2 * padding, padding, reg_w, reg_w);
const Rect r3(reg_w * 2 + 2 * padding, reg_w + padding, reg_w, reg_w);

const WCHAR Tip[] = L"Press 'G' to regenerate field\nPress 'S' to simulate movement\nPress 'D' to download field\nPress 'L' to upload field"; //\mPress 'Q' to stop simulation\0";
extern Mipmap mipmap;

int Random(int min, int max);

float Random();

float clamp(float min, float x, float max);

void Simulate(Container&);

float cos(xy& a, xy& b);
