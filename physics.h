#ifndef PHYSICS_H
#define PHYSICS_H
#include<vector>
#include"ball.h"
bool simulate(Ball *,Ball *,int);
void nextEvent(vector<Ball>,vector<Ball>&,int,int);
void nextEvent(vector<Ball>,vector<Ball>&,bool,int,int);
#endif // PHYSICS_H
