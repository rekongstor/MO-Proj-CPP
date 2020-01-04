#pragma once
#include "stdafx.h"

struct QT
{
    xy point;
    float w; // место и размеры листа на поле
    int depth;
    float time;
    QT_p    child00,
            child01,
            child10,
            child11;
    xy a; // ускорение в листе
    float fi; // направление ускорения
    float power; // сила ускорения
    bool been;
    QT* Get(float x, float y);
    void Draw(void* graphics, float full_time);
    QT();
    QT(xy point_, float w_, xy a_, int depth_, float time_, float fi_, float power_);
    QT(const QT&);
    ~QT();
    void Split(float x, float y);
    void Randomize(float full_time);
};

