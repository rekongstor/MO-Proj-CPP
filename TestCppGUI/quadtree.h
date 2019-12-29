#pragma once
#include "stdafx.h"

struct QT
{
    xy point;
    double w; // место и размеры листа на поле
    int depth;
    double time;
    QT *child00,
       *child01,
       *child10,
       *child11;
    xy a; // ускорение в листе
    double fi; // направление ускорения
    QT* Get(double x, double y);
    void Draw(void* graphics);
    QT();
    QT(xy point_, double w_, xy a_, int depth_, double time_, double fi_);
    void Split(double x, double y);
    void Randomize(double full_time);
};

