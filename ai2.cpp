#include<vector>
#include"ball.h"
#include<iostream>
#include"variables.h"
#include"ai2.h"
#include<stdlib.h>
#include"carrom.h"
#include"variables.h"
#include <stdio.h>
#include <math.h>

#define debug 0
using namespace std;

void ai2(int player,int not_to_hit_color,int level, vector<Ball>initial_coins, vector<Ball> &final_coins ,int holes){
    //printf("Entering AI\n");
    final_coins = initial_coins;
    //final_coins[0].setVelocity(15.0, 15.0);
	float x, y;

	vector<float *> valid_paths;

    if (debug) printf("Entering from AI\n");

    
    int color1, color2;
    if (not_to_hit_color == BLACK){
			color1 = YELLOW;
			color2 = RED;
		}
		else if (not_to_hit_color == YELLOW){
			color1 = BLACK;
			color2 = RED;			
		}
		else if (not_to_hit_color == REDANDBLACK){
			color1 = YELLOW;
			color2 = YELLOW;
		}
		else if (not_to_hit_color == REDANDYELLOW){
			color1 = BLACK;
			color2 = BLACK;
		}
    
    int coin, hole;
    
    if (debug) printf("2\n");
    
    if (debug) printf("size is %d\n", (int) final_coins.size());
		
		bool got_path = false;   
									float str_rad = final_coins[0].getRadius(); 
    
    for (coin = 1; coin < initial_coins.size() - 4; coin++){
			if (debug) printf("coin is %d\n", coin);
			
			float coinx = final_coins[coin].getPosition()->getX();
			float coiny = final_coins[coin].getPosition()->getY();

			if (final_coins[coin].getColor() == color1 || final_coins[coin].getColor() == color2){
					//float max_ghostx = 100;
					//float max_ghosty = 100;
					
					float min_vx = 1000;
					float min_vy = 1000;
					float ghostx, ghosty, min_ghostx, min_ghosty;
					
					for (hole = final_coins.size() - 4; hole < final_coins.size(); hole++){
							if (debug) printf("hole is %d\n", hole);
							
							float holex = final_coins[hole].getPosition()->getX();
							float holey = final_coins[hole].getPosition()->getY();

							
							
							//printf("Considering %f %f %f %f\n", holex, holey, coinx, coiny);
							
							

							if (debug) {printf("str_rad is %lf\n", str_rad); printf("strikerDia is %f\n", strikerDia);}
							
							float coin_rad = final_coins[coin].getRadius();
							
							float distance = sqrt(pow(holex - coinx, 2) + pow(holey - coiny, 2));
							if (debug) printf("a is %lf, b is %lf, c is %lf\n", holex - coinx, holey - coiny, distance);

							ghostx = holex + ((coinx - holex) * (distance + str_rad + coin_rad)) / (distance);
							ghosty = holey + ((coiny - holey) * (distance + str_rad + coin_rad)) / (distance);
							
							
							if (checkIfValid(final_coins, coin, holex, holey, coinx, coiny)){
								//printf("Before calloc at line 95\n");
								float * fivetuple=(float *) calloc(5, sizeof(float));
								//printf("After calloc at line 95\n");
								fivetuple[0] = ghostx;
								fivetuple[1] = ghosty;
								fivetuple[2] = coinx;
								fivetuple[3] = coiny;
								fivetuple[4] = (float) coin;
								
								//printf("%f %f %f %f %dis valid\n", ghostx, ghosty, coinx, coiny, coin);
								valid_paths.push_back(fivetuple);
								
							}
						}
						}
						}
						
						//printf("valid_paths.size() is %d\n", (int) valid_paths.size());
						
						for (int k = 0; k < valid_paths.size(); k++){

							//printf ("path for player %d is %f %f %f %f %d\n", player, valid_paths[k][0], valid_paths[k][1], valid_paths[k][2], valid_paths[k][3], (int)valid_paths[k][4]);

						}
						
							vector<float *> valid_shots;
						
						for (float pos = 0; pos < baseLinelength; pos += 2 * str_rad){
								if(player==0){
				            y = -1 * (board_side / 2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth ) / 2.0);
				            x = baseLinelength / 2.0 - pos;
				        }
				        else if(player == 1){
				            x = board_side / 2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth) / 2.0;
				            y = baseLinelength / 2.0 - pos;
				        }
				        else if(player==2){
				            y = board_side /2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth) / 2.0;
				            x = baseLinelength / 2.0 - pos;
				        }
				        else{
				            x = -1 * (board_side / 2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth) / 2.0);
				            y = baseLinelength / 2.0 - pos;
				        }

							for (int i = 0; i < valid_paths.size(); i++){
								float * path = valid_paths[i];
								float ghostx = path[0], ghosty = path[1], coinx = path[2], coiny = path[3];
								int coin = path[4];
								
								
								
								
								if (checkIfValid(final_coins, -1, ghostx, ghosty, x, y)){
									
									float m1 = (ghosty - y) / (ghostx - x);
									float m2 = (ghosty - coiny) / (ghostx - coinx);
									float m = fabs((m2 - m1) / (1 + m1 * m2));
									//printf("Before calloc at line 151\n");
									float * fivetuple = (float *) calloc(5, sizeof(float));
									//printf("After calloc at line 151\n");
									fivetuple[0] = ghostx;
									fivetuple[1] = ghosty;
									fivetuple[2] = x;
									fivetuple[3] = y;
									fivetuple[4] = m;
									valid_shots.push_back(fivetuple);
								}
								
								 
							}
						}
						
						//printf("valid_shots.size() is %d\n", (int)valid_shots.size());
						
						float offset;
						if (level == 3){
							offset = 0.001;
						}
						else if (level == 2){
							offset = 0.1;
						}
						else if (level == 1){
							offset = 1;
						}
						else if (level == 0){
							offset = 2;
						}
						
						float minm = 1000;
						int mini = -1;
						for (int i = 0; i < valid_shots.size(); i++){
							//printf("valid shots are %f %f %f %f %f\n", valid_shots[i][0], valid_shots[i][1], valid_shots[i][2], valid_shots[i][3], valid_shots[i][4]);
							
							if (valid_shots[i][4] < minm && valid_shots[i][4] >= 0.1){
								minm = valid_shots[i][4];
								mini = i;
							}
						
						}
						
						if (mini == -1){
							if (valid_shots.size() != 0){
								mini = 0;
							}
							else{
								final_coins[0].setPosition(x, y);
								float x2y2 = sqrt(pow(x, 2) + pow(y, 2));
								final_coins[0].setVelocity( - 35 * x / x2y2, - 35 * y / x2y2);
								//printf("Leaving AI\n");
								return;
							}
						}
						
						
						
						//printf("Choosing shot %f %f %f %f %f\n", valid_shots[mini][0], valid_shots[mini][1], valid_shots[mini][2], valid_shots[mini][3], valid_shots[mini][4]);
						
						float vel = 35;
						
						float vx = valid_shots[mini][0] - valid_shots[mini][2] + offset * rand() / RAND_MAX;
						float vy = valid_shots[mini][1] - valid_shots[mini][3] + offset * rand() / RAND_MAX;
						float d = sqrt(pow(vx, 2) + pow(vy, 2));
						//final_coins[0].setPosition(valid_shots[mini][0], valid_shots[mini][1]);
						final_coins[0].setPosition(valid_shots[mini][2], valid_shots[mini][3]);
						
						final_coins[0].setVelocity(vel*vx/d, vel*vy/d);		
						    //printf("Leaving AI\n");
						return;
						}
							
						
						/*
						
						for (float pos = 0; pos < baseLinelength; pos += 2 * str_rad){
							
							if(player==0){
		              y = -1 * (board_side / 2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth ) / 2.0);
		              x = baseLinelength / 2.0 - pos;
		          }
		          else if(player == 1){
		              x = board_side / 2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth) / 2.0;
		              y = baseLinelength / 2.0 - pos;
		          }
		          else if(player==2){
		              y = board_side /2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth) / 2.0;
		              x = baseLinelength / 2.0 - pos;
		          }
		          else{
		              x = -1 * (board_side / 2.0 - baseLinedisplacement + (baseLineseperation - 2 * baseLinewidth) / 2.0);
		              y = baseLinelength / 2.0 - pos;
		          }

							if (checkIfValid(final_coins, coin, x, y, ghostx, ghosty)){
								got_path = true;
								break;

							}
						
						}
							
							if (got_path){
							final_coins[0].setPosition(x, y);
							
							float vx = ghostx - final_coins[0].getPosition()->getX();
							float vy = ghosty - final_coins[0].getPosition()->getY();
							
							if (pow(vx, 2) + pow(vy, 2) < pow(min_vx, 2) + pow(min_vy, 2)){
									min_vx = vx;
									min_vy = vy;
									min_ghostx = ghostx;
									min_ghosty = ghosty;
								}
								}
						}
						
						
						float d = sqrt(pow(min_vx, 2) + pow(min_vy, 2));
						if (debug) printf("a is %lf, b is %lf, c is %lf\n", min_vx, min_vy, d);
						float vel = 35;
						final_coins[0].setVelocity(vel*min_vx/d, vel*min_vy/d);
						//final_coins[0].setPosition(min_ghostx, min_ghosty);
						
						if (debug) printf("Returning from AI\n");
						
						return;
						
					
					
					
					
					
					//float vy = final_coins[coin].getPosition()->getY() - final_coins[0].getPosition()->getY();
					//float vx = final_coins[coin].getPosition()->getX() - final_coins[0].getPosition()->getX();
					//final_coins[0].setVelocity(vx, vy);
					//break;
				}
		}
		
		final_coins[0].setPosition(x, y);
		final_coins[0].setVelocity(-35 * final_coins[0].getPosition()->getX(), -35 * final_coins[0].getPosition()->getY());
		return;
		
		
		
}
*/


bool isValidGhostBall(Ball a,vector<Ball>vec){
    if(a.getPosition()->getX()>board_side/2.0-a.getRadius()||a.getPosition()->getY()>board_side/2.0-a.getRadius()||
        a.getPosition()->getX()<-board_side/2.0+a.getRadius()||a.getPosition()->getY()<-board_side/2.0+a.getRadius())
        return false;
    for(int i=0;i<vec.size();i++){
            if(vec[i].getIndex()==a.getIndex())
                continue;
            double dx=a.getPosition()->getX()-vec[i].getPosition()->getX();
            double dy=a.getPosition()->getY()-vec[i].getPosition()->getY();
            if(sqrt(dx*dx+dy*dy)<a.getRadius()+vec[i].getRadius()){
                return false;
            }
        }
    return true;
}

bool checkIfValid(vector<Ball> coins, int ignore, float x1, float y1, float x2, float y2){
	for (int coin = 1; coin < coins.size() - 4; coin++){
		if (coin == ignore){continue;}
		float x3 = coins[coin].getPosition()->getX();
		float y3 = coins[coin].getPosition()->getY();
		
		
		// check if distance of (x3, y3) from the line joining (x1, y1) to (x2, y2) is less than 2 * carromMenRad i.e. carrommenDia
		
		float A = y2 - y1;
		float B = x1 - x2;
		float C = y1 * (x1 + x2) - x1 * (y1 + y2);
		
		float d = fabs(A * x3 + B * y3 + C) / sqrt(pow(A, 2) + pow(B, 2));
		
		float d1 = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
		float d2 = sqrt(pow(x3 - x1, 2) + pow(y3 - y1, 2));
		float d3 = sqrt(pow(x3 - x2, 2) + pow(y3 - y2, 2));

		
		
		
		if (d < carrommenDia && (d2 < d1 && d3 < d1)){
			//printf("coin %d at (%f, %f) is interfering with this path\n", coin, x3, y3);
			return false;
		}
	}
	
	return true;
}
