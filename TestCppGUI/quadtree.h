#pragma once

struct QT
{
    double x, y, w;
    virtual ~QT() {};
    virtual QT* Get(double, double) = 0;
    virtual void Draw() = 0;
};

struct QT_Parent : QT
{
    QT_Parent();
    ~QT_Parent();

};

struct QT_Leaf : QT
{
    QT_Leaf();
    ~QT_Leaf();
};

