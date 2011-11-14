#ifndef COMPONENTS_H
#define COMPONENTS_H

class Components{
    double x,y;
public:
    Components(double x1,double y1){
        x=x1;
        y=y1;
    }    double getX(){
        return x;
    }
    double getY(){
            return y;
    }
 };

class point{
public:
    double x,y;
    point(double a,double b){
        x=a;
        y=b;
    }
};

#endif // COMPONENTS_H
