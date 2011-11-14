#ifndef BALL_H
#define BALL_H
#include"components.h"
class Ball{
    double x,y;
    double vx,vy;
    int dirofwall,index;
    double radius,mass;
    bool state;
    bool  highlited;
    int color,hcolor;
public:
    Ball();
    void deleteBall();
    void setHighlightColor(bool,int );
    int getHighlightedColor();
    void setColor(int );
    int getColor();
    void setRadius(double );
    double getRadius();
    bool exists();
    void setIndex(int );
    int getIndex();
    void setPosition(double ,double );
    void setVelocity(double ,double );
    Components *getPosition();
    Components *getVelocity();
    Components *getVelocityUnitVector();
    Components *VelocityAlongUnitVector(Components *);
    Components *VelocityPerpendicularTOUnitVector(Components *);
    Components* getUnitVector(Components *);
    void setDirectionofWall(int );
    int getDirectionofWall();
    void setMass(double );
    double getMass();
    double getAccelerationAlongUnitVector(Components *);
};
#endif // BALL_H
