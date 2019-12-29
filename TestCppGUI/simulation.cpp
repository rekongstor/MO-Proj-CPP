#include "field.h"
#include "quadtree.h"
#include "robot.h"



void Simulate()
{
	robot.q.Split(0.123, 0.123);
	robot.q.Randomize(0.);
}