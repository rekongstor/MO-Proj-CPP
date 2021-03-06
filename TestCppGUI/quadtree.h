#pragma once
#include "stdafx.h"

struct QT
{
    xy point;
    float w; // ����� � ������� ����� �� ����
    int depth;
    float time;
    float bestest_dist;
    QT_p    child00,
            child01,
            child10,
            child11;
    xy a; // ��������� � �����
    float fi; // ����������� ���������
    float power; // ���� ���������
    bool been;
    QT* Get(float& x, float& y);
    void Draw(void* graphics, float& full_time);
    QT();
    QT(xy point_, float w_, xy& a_, int& depth_, float& time_, float& fi_, float& power_, bool& been, float& bd);
    QT(const QT&);
    ~QT();
    void Copy(QT&);
    void Split(float& x, float& y);
    void Randomize(float& full_time);
    void Randomize2(float& full_time);
    void CleanTimes(float best_distance);
};

