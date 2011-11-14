#ifndef AI_PLAYER
#define AI_PLAYER
#include"ball.h"
#include<vector>
using namespace std;
void ai(int,int,int,vector<Ball> , vector<Ball>&,int );
Ball *ghostBall(Ball ,Ball ,Ball );
bool canReach(Ball ,Ball ,vector<Ball>);
bool positionStriker(int,Ball,vector<Ball>&);
void defaultPositionStriker(int ,vector<Ball>&);
void ai(int ,int ,int ,vector<Ball>,vector<Ball>&,int ,int );
#endif
