#pragma once
#include "stdafx.h"

struct QT
{
    xy point;
    double w; // ����� � ������� ����� �� ����
    int depth;
    double time;
    QT *child00,
       *child01,
       *child10,
       *child11;
    xy a; // ��������� � �����
    QT* Get(double x, double y);
    void Draw(void* graphics);
    QT();
    QT(xy point_, double w_, xy a_, int depth_, double time_);
    void Split(double x, double y);
    void Randomize(double full_time);
};

