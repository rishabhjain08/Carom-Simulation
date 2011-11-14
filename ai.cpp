#include<vector>
#include"ball.h"
#include<iostream>
#include"vectors.h"
#include"variables.h"
#include"ai.h"
#include"ai2.h"
#include<stdlib.h>
#include"carrom.h"
#include"variables.h"
using namespace std;

void ai(int playernum,int not_to_hit_color,int level,vector<Ball>initial_coins,vector<Ball>&final_coins,int holes,int algonum){
if(algonum<2)
	return ai( playernum, not_to_hit_color, level,initial_coins,final_coins,holes);
else 
	return ai2( playernum, not_to_hit_color, level,initial_coins,final_coins,holes);
}

void ai(int playernum,int not_to_hit_color,int level,vector<Ball>initial_coins,vector<Ball>&final_coins,int holes){
    final_coins = initial_coins;
    int color=0;
    if(not_to_hit_color==YELLOW)
    	color=BLACK;
	else
      color=YELLOW;
  	if(countColor(final_coins,RED)!=0&&countColor(final_coins,color)==1)
  		color=RED;
    if(initial_coins.size()==0||initial_coins[0].getIndex()!=0||final_coins.size()<5)
        return;
    //TODO HERE :
    Ball ghost;
    double svx=0.0,svy=0.0;
    bool ghostfound=false;
    int i,j;
    if(level<2){
        for(i=1;i<final_coins.size()-holes;i++){
            if(final_coins[i].getColor()!=color)
                continue;
            for(j=0;j<holes;j++){
                ghost=*ghostBall(final_coins[0],final_coins[i],final_coins[final_coins.size()-j-1]);
                if(!positionStriker(playernum,ghost,final_coins)){
                    continue;
                }
                svx=ghost.getPosition()->getX()-final_coins[0].getPosition()->getX();
                svy=ghost.getPosition()->getY()-final_coins[0].getPosition()->getY();
                if((playernum==0&&svy<0)||(playernum==1&&svx>0)||(playernum==2&&svy>0)||(playernum==3&&svx<0)){
                    continue;
                }
                if(!canReach(final_coins[0],ghost,final_coins)){
                    continue;
                }
                if(!positionStriker(playernum,ghost,final_coins)){
                    continue;
                }
                ghostfound=true;
                break;
            }
        }
    }
    else{
        vector<Ball> ghosts;
        	for(i=1;i<final_coins.size()-holes;i++){
            if(final_coins[i].getColor()!=color)
                continue;
            for(j=0;j<holes;j++){
                ghost=*ghostBall(final_coins[0],final_coins[i],final_coins[final_coins.size()-j-1]);
                if(!positionStriker(playernum,ghost,final_coins)){
                    continue;
                }
                svx=ghost.getPosition()->getX()-final_coins[0].getPosition()->getX();
                svy=ghost.getPosition()->getY()-final_coins[0].getPosition()->getY();
                if((playernum==0&&svy<0)||(playernum==1&&svx>0)||(playernum==2&&svy>0)||(playernum==3&&svx<0)){
                    continue;
                }
                if(!canReach(final_coins[0],ghost,final_coins)){
                    continue;
                }
                if(!canReach(final_coins[i],final_coins[final_coins.size()-j-1],final_coins)){
                    continue;
                }
                ghosts.push_back(ghost);
            }
        }
        double costheta=0.0;
	if(!ghosts.empty()){
            double remtheta=0.0;
            for(int k=0;k<ghosts.size();k++){
                positionStriker(playernum,ghosts[k],final_coins);
                vectors *dir=new vectors(ghosts[k].getPosition()->getX()-final_coins[0].getPosition()->getX(),ghosts[k].getPosition()->getY()-	final_coins[0].getPosition()->getY());
                vectors *ghostdir=new vectors(ghosts[k].getVelocity()->getX(),ghosts[k].getVelocity()->getY());
                remtheta=abs(dir->costheta(ghostdir));
                if(costheta<remtheta){
                    ghostfound=true;
                    costheta=remtheta;
                    ghost=ghosts[k];
                }
            }
            positionStriker(playernum,ghost,final_coins);
        }
      if(level>2){
	ghosts.clear();
	 //TODO Here: 
	 double ybyx,dy1,dy2,svx,svy;
	 defaultPositionStriker(playernum,final_coins);
	  for(i=1;i<final_coins.size()-holes;i++){
        if(final_coins[i].getColor()!=color)
            continue;
    	for(j=0;j<holes;j++){
		int dirx=1,diry=1;
		ghost=*ghostBall(final_coins[0],final_coins[i],final_coins[final_coins.size()-j-1]);
		double ghostx=ghost.getPosition()->getX();
		double ghosty=ghost.getPosition()->getY();
		svx=ghost.getPosition()->getX()-final_coins[0].getPosition()->getX();
		svy=ghost.getPosition()->getY()-final_coins[0].getPosition()->getY();
		if(!canReach(final_coins[i],final_coins[final_coins.size()-j-1],final_coins)){
                    continue;
                }
                if(svx!=0){//wall top
		  if(svx<0)
		    dirx=-1;
		  diry=1;
		  ybyx=0.0;
		  dy1=abs(board_side/2.0-final_coins[0].getRadius()-final_coins[0].getPosition()->getY());
		  dy2=abs(board_side/2.0-final_coins[0].getRadius()-ghosty);
		  ybyx=(dy1+dy2/COR)/abs(svx);
		  Ball *wallghost=new Ball();
		  vectors *vect=new vectors(dirx,diry*ybyx);
		  vect->mul(dy1/abs(vect->y));
		  wallghost->setPosition(final_coins[0].getPosition()->getX()+vect->x,final_coins[0].getPosition()->getY()+vect->y);
		  wallghost->setIndex(final_coins[0].getIndex());
		  wallghost->setRadius(final_coins[0].getRadius());
		    if(canReach(final_coins[0],*wallghost,final_coins)&&canReach(*wallghost,ghost,final_coins)){
		    ghostfound=true;
		    wallghost->setVelocity(ghost.getVelocity()->getX()*(ghost.getPosition()->getX()-wallghost->getPosition()->getX())+ghost.getVelocity()->getY()*(ghost.getPosition()->getY()-wallghost->getPosition()->getY()),0.0);
		    ghosts.push_back(*wallghost);
		  }
		}
		if(svx!=0){//wall bottom
		  if(svx<0)
		    dirx=-1;
		  diry=-1;
		  ybyx=0.0;
		  dy1=abs(-board_side/2.0+final_coins[0].getRadius()-final_coins[0].getPosition()->getY());
		  dy2=abs(-board_side/2.0+final_coins[0].getRadius()-ghosty);
		  ybyx=(dy1+dy2/COR)/abs(svx);
		  Ball *wallghost=new Ball();
		  vectors *vect=new vectors(dirx,diry*ybyx);
		  vect->mul(dy1/abs(vect->y));
		  wallghost->setPosition(final_coins[0].getPosition()->getX()+vect->x,final_coins[0].getPosition()->getY()+vect->y);
		  wallghost->setIndex(final_coins[0].getIndex());
		  wallghost->setRadius(final_coins[0].getRadius());
		  if(canReach(final_coins[0],*wallghost,final_coins)&&canReach(*wallghost,ghost,final_coins)){
		    ghostfound=true;
		    wallghost->setVelocity(ghost.getVelocity()->getX()*(ghost.getPosition()->getX()-wallghost->getPosition()->getX())+ghost.getVelocity()->getY()*(ghost.getPosition()->getY()-wallghost->getPosition()->getY()),0.0);
		    ghosts.push_back(*wallghost);
		  }
		}
		if(svy!=0){//wall left
		  dirx=-1;
		  if(svy<0)
		    diry=-1;
		  ybyx=0.0;
		  dy1=abs(-board_side/2.0+final_coins[0].getRadius()-final_coins[0].getPosition()->getX());
		  dy2=abs(-board_side/2.0+final_coins[0].getRadius()-ghostx);
		  ybyx=(dy1+dy2/COR)/abs(svy);
		  Ball *wallghost=new Ball();
		  vectors *vect=new vectors(dirx*ybyx,diry);
		  vect->mul(dy1/abs(vect->x));
		  wallghost->setPosition(final_coins[0].getPosition()->getX()+vect->x,final_coins[0].getPosition()->getY()+vect->y);
		  wallghost->setIndex(final_coins[0].getIndex());
		  wallghost->setRadius(final_coins[0].getRadius());
		   if(canReach(final_coins[0],*wallghost,final_coins)&&canReach(*wallghost,ghost,final_coins)){
		    ghostfound=true;
		    wallghost->setVelocity(ghost.getVelocity()->getX()*(ghost.getPosition()->getX()-wallghost->getPosition()->getX())+ghost.getVelocity()->getY()*(ghost.getPosition()->getY()-wallghost->getPosition()->getY()),0.0);
		    ghosts.push_back(*wallghost);
		  }
		}
		if(svy!=0){//wall right
		  dirx=1;
		  if(svy<0)
		    diry=-1;
		  ybyx=0.0;
		  dy1=abs(board_side/2.0-final_coins[0].getRadius()-final_coins[0].getPosition()->getX());
		    dy2=abs(board_side/2.0-final_coins[0].getRadius()-ghostx);
		  ybyx=(dy1+dy2/COR)/abs(svy);
		  Ball *wallghost=new Ball();
		  vectors *vect=new vectors(dirx*ybyx,diry);
		  vect->mul(dy1/abs(vect->x));
		  wallghost->setPosition(final_coins[0].getPosition()->getX()+vect->x,final_coins[0].getPosition()->getY()+vect->y);
		  wallghost->setIndex(final_coins[0].getIndex());
		  wallghost->setRadius(final_coins[0].getRadius());
		  if(canReach(final_coins[0],*wallghost,final_coins)&&canReach(*wallghost,ghost,final_coins)){
		    ghostfound=true;
		    wallghost->setVelocity(ghost.getVelocity()->getX()*(ghost.getPosition()->getX()-wallghost->getPosition()->getX())+ghost.getVelocity()->getY()*(ghost.getPosition()->getY()-wallghost->getPosition()->getY()),0.0);
		    ghosts.push_back(*wallghost);
		  }
		  }
            }
        }
	   if(!ghosts.empty()){
            double remtheta=0.0;
            for(int k=0;k<ghosts.size();k++){
                remtheta=abs(ghosts[k].getVelocity()->getX());
                if(costheta<remtheta){
                    ghostfound=true;
                    costheta=remtheta;
                    ghost=ghosts[k];
                }
            }
            positionStriker(playernum,ghost,final_coins);
        }
	}
}
   vectors *velo;
    double speed=50.0;
    if(!ghostfound){
        defaultPositionStriker(playernum,final_coins);
        if(final_coins.size()>holes+1){
          int u=0;
	  for(u=0;u<final_coins.size();u++)
	    if(final_coins[u].getColor()==color)
	      break;
	    if(u!=final_coins.size())
	      velo=new vectors(final_coins[u].getPosition()->getX()-final_coins[0].getPosition()->getX(),final_coins[u].getPosition()->getY()-final_coins[0].getPosition()->getY());
	    else
	      velo=new vectors(0.0,0.0);
	}
        else{
            final_coins[0].setPosition(0,0);
            velo=new vectors(0,0);
        }
        velo->normalize();
        velo->mul(speed);
        final_coins[0].setVelocity(velo->x,velo->y);
        return;
    }
    velo=new vectors(ghost.getPosition()->getX()-final_coins[0].getPosition()->getX(),
                              ghost.getPosition()->getY()-final_coins[0].getPosition()->getY());
    velo->normalize();
    velo->mul(speed);
    final_coins[0].setVelocity(velo->x,velo->y);
}

Ball *ghostBall(Ball balltobeshot,Ball coin,Ball hole){
    double error=-0.01;
    vectors *a=new vectors(coin.getPosition()->getX()-hole.getPosition()->getX(),coin.getPosition()->getY()-hole.getPosition()->getY());
    int dist=a->magnitude();
    a->normalize();
    a->mul(coin.getRadius()+balltobeshot.getRadius()+error);
    Ball *newball=new Ball();
    newball->setPosition(coin.getPosition()->getX()+a->x,coin.getPosition()->getY()+a->y);
    newball->setRadius(balltobeshot.getRadius());
    newball->deleteBall();
    newball->setIndex(balltobeshot.getIndex());
    newball->setVelocity(hole.getPosition()->getX()-coin.getPosition()->getX(),hole.getPosition()->getY()-coin.getPosition()->getY());
    return newball;
}

bool canReach(Ball a,Ball b,vector<Ball>initial_coins){
    if(initial_coins.size()==0)
        return false;
    vectors *along=new vectors(b.getPosition()->getX()-a.getPosition()->getX(),b.getPosition()->getY()-a.getPosition()->getY());
    double dist=along->magnitude();
    along->normalize();
    vectors *perp=along->copy();
    perp->rotate(90.00);
    perp->normalize();
    int baseradius=a.getRadius();
    for(int i=0;i<initial_coins.size();i++){
        if(initial_coins[i].getIndex()==a.getIndex()||initial_coins[i].getIndex()==b.getIndex()){
            continue;
        }
        vectors *p=perp->copy();
        p->mul(initial_coins[i].getRadius()+baseradius);
        vectors *para=along->copy();
        para->mul(dist+baseradius);
        point *p1=new point(a.getPosition()->getX()+p->x,a.getPosition()->getY()+p->y);
        point *p4=new point(a.getPosition()->getX()-p->x,a.getPosition()->getY()-p->y);
        point *p2=new point(a.getPosition()->getX()+para->x+p->x,a.getPosition()->getY()+para->y+p->y);
        point *p3=new point(a.getPosition()->getX()+para->x-p->x,a.getPosition()->getY()+para->y-p->y);
        double cx=initial_coins[i].getPosition()->getX();
        double cy=initial_coins[i].getPosition()->getY();
        point *checkp=new point(cx,cy);
        double upsign1=-(p4->y-p1->y)+(p2->y-p1->y)*(p4->x-p1->x)/(p2->x-p1->x);
        double upsign2=-(checkp->y-p1->y)+(p2->y-p1->y)*(checkp->x-p1->x)/(p2->x-p1->x);

        double downsign1=-(p1->y-p4->y)+(p3->y-p4->y)*(p1->x-p4->x)/(p3->x-p4->x);
        double downsign2=-(checkp->y-p4->y)+(p3->y-p4->y)*(checkp->x-p4->x)/(p3->x-p4->x);

        double leftsign1=-(p1->y-p2->y)+(p3->y-p2->y)*(p1->x-p2->x)/(p3->x-p2->x);
        double leftsign2=-(checkp->y-p2->y)+(p3->y-p2->y)*(checkp->x-p2->x)/(p3->x-p2->x);

        double rightsign1=-(p2->y-p1->y)+(p4->y-p1->y)*(p2->x-p1->x)/(p4->x-p1->x);
        double rightsign2=-(checkp->y-p1->y)+(p4->y-p1->y)*(checkp->x-p1->x)/(p4->x-p1->x);
        if(upsign1*upsign2>0&&downsign1*downsign2>0&&leftsign1*leftsign2>0&&rightsign1*rightsign2>0)
            return false;

    }
    return true;
}


void defaultPositionStriker(int playernum,vector<Ball>&coins){
    double displaced=0;
    double  diff=coins[0].getRadius()*2;
    if(diff==0)
        diff=1.0;
    int niter=1,i=0;
    double pos=0,x,y;
    for(i=0;i<niter;i++){
        pos=displaced;
        while(pos<baseLinelength){
            if(playernum==0){
                y=-1*(board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0);
                x=baseLinelength/2.0-pos;
            }
            else if(playernum==1){
                x=board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0;
                y=baseLinelength/2.0-pos;
            }
            else if(playernum==2){
                y=board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0;
                x=baseLinelength/2.0-pos;
            }
            else{
                x=-1*(board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0);
                y=baseLinelength/2.0-pos;
            }
            coins[0].setPosition(x,y);
            pos+=diff;
            if(!isSeperateBall(coins[0],coins)){
                continue;
            }
            return;
        }
        displaced=coins[0].getRadius()/(pow(2,i));
    }
    return;
}

bool positionStriker(int playernum,Ball ghost,vector<Ball>&coins){
    double displaced=0;
    double  diff=coins[0].getRadius()*2;
    if(diff==0)
        diff=1.0;
    int niter=1,i=0;
    double pos=0,x,y;
    for(i=0;i<niter;i++){
        pos=displaced;
        while(pos<baseLinelength){
            if(playernum==0){
                y=-1*(board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0);
                x=baseLinelength/2.0-pos;
            }
            else if(playernum==1){
                x=board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0;
                y=baseLinelength/2.0-pos;
            }
            else if(playernum==2){
                y=board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0;
                x=baseLinelength/2.0-pos;
            }
            else{
                x=-1*(board_side/2.0-baseLinedisplacement+(baseLineseperation-2*baseLinewidth)/2.0);
                y=baseLinelength/2.0-pos;
            }
            coins[0].setPosition(x,y);
            pos+=diff;
            if(!isSeperateBall(coins[0],coins)){
                continue;
            }
            if(!canReach(coins[0],ghost,coins)){
                continue;
            }
            return true;
        }
        displaced=coins[0].getRadius()/(pow(2,i));
    }
    return false;
}
