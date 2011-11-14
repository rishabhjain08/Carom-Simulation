#ifndef VECTORS_H
#define VECTORS_H

#include<cmath>
#include<iostream>
using namespace std;
#undef PI
#define PI 3.14

class vectors{
	public:
                double x,y;
		vectors();
		vectors(double, double);
		double dot(vectors *);
                double magnitude(void);
                void rotate(double angle);
                void mul(double );
                vectors* copy(void);
                void normalize();
                void add(vectors *);
                double costheta(vectors *);
            };
double vectors::dot(vectors *b){
        return (this->x*b->x+this->y*b->y);
}
void vectors::normalize(){
        double mag=this->magnitude();
        if(mag==0)
            return;
        this->x=this->x/mag;
        this->y=this->y/mag;
}
double vectors::magnitude(){
        return sqrt(this->x*this->x+this->y*this->y);
}
vectors::vectors(){
    this->x=0;
    this->y=0;
}
vectors::vectors(double x1,double y1){
    this->x=x1;
    this->y=y1;
}
void vectors::rotate(double angle_in_degrees){
    double angle=angle_in_degrees*PI/180.00;
    double cos1=cos(angle);
    double sin1=sin(angle);
    double rem_x=this->x;
    this->x=this->x*cos1-this->y*sin1;
    this->y=this->y*cos1+rem_x*sin1;
}
void vectors::mul(double constant){
    this->x=this->x*constant;
    this->y=this->y*constant;
}

vectors* vectors::copy(){
    vectors *c=new vectors(this->x,this->y);
    return c;
}
void vectors::add(vectors *vector2){
    this->x=this->x+vector2->x;
    this->y=this->y+vector2->y;
}

double vectors::costheta(vectors *vector2){
    return (this->dot(vector2)/(this->magnitude()*vector2->magnitude()));
}

#endif //VECTORS_H

