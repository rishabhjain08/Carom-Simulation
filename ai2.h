#ifndef AI_PLAYER_2
#define AI_PLAYER_2
#include"ball.h"
#include<vector>
using namespace std;
void ai(int,int,int,vector<Ball> , vector<Ball>& );
Ball *ghostBall(Ball ,Ball ,Ball );
bool canReach(Ball ,Ball ,vector<Ball>);
bool isValidGhostBall(Ball,vector<Ball>);
bool positionStriker(int,Ball,vector<Ball>&);
void defaultPositionStriker(int ,vector<Ball>&);
bool checkIfValid(vector<Ball>, int, float, float, float, float);
void ai2(int,int ,int, vector<Ball>, vector<Ball> & ,int );
#endif
