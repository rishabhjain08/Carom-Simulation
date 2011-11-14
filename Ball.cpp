#include"ball.h"
#include"components.h"
#include"variables.h"
#include<cmath>
#include<stdlib.h>

    Ball::Ball(){
        Ball::highlited=false;
        Ball::state=true;
        Ball::hcolor=-1;
    }

    void Ball::deleteBall(){
        state=false;
    }
    void Ball::setHighlightColor(bool b,int color1){
        highlited=b;
        hcolor=color1;
    }
    int Ball::getHighlightedColor(){
        if(hcolor!=-1)
            return hcolor;
        return this->getColor();
    }
    void Ball::setColor(int c){
        color=c;
    }
    int Ball::getColor(){
        return color;
    }
    void Ball::setRadius(double rad){
        radius=rad;
    }
    double Ball::getRadius(){
        return radius;
    }
    bool Ball::exists(){
        return state;
    }
    void Ball::setIndex(int i){
        index=i;
    }
    int Ball::getIndex(){
        return index;
    }

    void Ball::setPosition(double x1,double y1){
        x=x1;
        y=y1;
    }
    void Ball::setVelocity(double vx1,double vy1){
        vx=vx1;
        vy=vy1;
    }
    Components *Ball::getPosition(){
        return new Components(x,y);
    }
    Components *Ball::getVelocity(){
        return new Components(vx,vy);
    }
    Components *Ball::getVelocityUnitVector(){
        return this->getUnitVector(this->getVelocity());
        //double theta=atan(vy/vx);
        //return new Components(vx*cos(theta)/(sqrt(vx*vx+vy*vy)),vy*sin(theta)/(sqrt(vx*vx+vy*vy)));
    }
    Components* Ball::getUnitVector(Components *vec){
        double x_=vec->getX();
        double y_=vec->getY();
        double magnitude=sqrt(x_*x_+y_*y_);
        if(magnitude==0.0)
            return vec;
        else
            return new Components(x_/magnitude,y_/magnitude);
    }

    Components *Ball::VelocityAlongUnitVector(Components *comp){
        double dotproduct=vx*comp->getX()+vy*comp->getY();
        return new Components(comp->getX()*dotproduct,comp->getY()*dotproduct);
    }
    Components *Ball::VelocityPerpendicularTOUnitVector(Components *comp){
        Components *along=this->VelocityAlongUnitVector(comp);
        return new Components(vx-along->getX(),vy-along->getY());
    }
    void Ball::setDirectionofWall(int i){
        dirofwall=i;
    }
    int Ball::getDirectionofWall(){
        return dirofwall;
    }
    void Ball::setMass(double d){
        mass=d;
    }
    double Ball::getMass(){
        return mass;
    }
    double Ball::getAccelerationAlongUnitVector(Components *comp){
        comp=this->getUnitVector(comp);
        Components* unit_velo=this->getVelocityUnitVector();
        return (-1*MU*G*(unit_velo->getX()*comp->getX()+unit_velo->getY()*comp->getY()));
    }

