#include<vector>
#include"ball.h"
#include<cmath>
#include"variables.h"
#include"carrom.h"
#include"physics.h"

using namespace std;

void nextEvent(vector<Ball>v,vector<Ball>&fill,bool checkIfAtrest,int nball,int holes){
	if(v.size()<1)
	return;
    bool speedChanged=false;
    double x1,x2,y1,y2,r1,r2,vx1,vy1,vx2,vy2,ax1,ay1,remvx,remvy;
    double new_x,new_y,new_vx,new_vy;
    int i=0,j=0,index1,index2;
    bool event_occured=false;
    vector<Ball> ::iterator iter;
    iter=v.begin();
    bool simulate_;
    bool deleted=false;
        for(i=0;i<v.size()-holes;i++){
            x1=(*iter).getPosition()->getX();
            y1=(*iter).getPosition()->getY();
            r1=(*iter).getRadius();
            vx1=(*iter).getVelocity()->getX();
            vy1=(*iter).getVelocity()->getY();
            index1=(*iter).getIndex();
            deleted=false;
            for(j=i+1;j<v.size();j++){
                if(i==j)
                    continue;
                //exit(9);
                x2=((Ball)v[j]).getPosition()->getX();
                y2=((Ball)v[j]).getPosition()->getY();
                r2=((Ball)v[j]).getRadius();
                vx2=((Ball)v[j]).getVelocity()->getX();
                vy2=((Ball)v[j]).getVelocity()->getY();
                index2=((Ball)v[j]).getIndex();
                if((sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))<r1+r2+ERROR)&&((vx1-vx2)*(x1-x2)+(vy1-vy2)*(y1-y2))<0){
                    bool res=(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))<r2+ERROR);
                    if(index2>=nball&&res){
                        event_occured=true;
                        deleted=true;
                        iter=v.erase(iter);
                        i--;
                        break;
                    }
                    else if(index2<nball){
                        simulate_=simulate(&*iter,&v[j],nball);
                        vx1=(*iter).getVelocity()->getX();
                        vy1=(*iter).getVelocity()->getY();
                        vx2=v[j].getVelocity()->getX();
                        vy2=v[j].getVelocity()->getY();
                    }
                }
            }
            if(deleted)
                continue;
            if(((x1>WIDTH/2.0-r1-ERROR)&&vx1>0)||((x1<-WIDTH/2.0+ERROR+r1)&&vx1<0)){
                (*iter).setVelocity(COR*vx1*-1,vy1);
                vx1=vx1*-1*COR;
                event_occured=true;
            }
            if((vy1>0&&(y1>HEIGHT/2.0-r1-ERROR))||(vy1<0&&(y1<-HEIGHT/2+ERROR+r1))){
                (*iter).setVelocity(vx1,COR*vy1*-1);
                vy1=vy1*-1*COR;
                event_occured=true;
            }
            if(vx1!=0.0||vy1!=0.0)
                ax1=(*iter).getAccelerationAlongUnitVector(new Components(1.0,0.0));
            else
                ax1=0.0;
            if(vx1!=0||vy1!=0)
                ay1=(*iter).getAccelerationAlongUnitVector(new Components(0.0,1.0));
            else
                ay1=0.0;
            new_x=x1+(*iter).getVelocity()->getX()*MOVE_TIME+0.5*ax1*MOVE_TIME*MOVE_TIME;
            new_y=y1+(*iter).getVelocity()->getY()*MOVE_TIME+0.5*ay1*MOVE_TIME*MOVE_TIME;
            (*iter).setPosition(new_x,new_y);
            new_vx=((vx1)*(remvx=(vx1+ax1*MOVE_TIME))<=0)?0:remvx;
            new_vy=((vy1)*(remvy=(vy1+ay1*MOVE_TIME))<=0)?0.0:remvy;
            (*iter).setVelocity(new_vx,new_vy);
            iter++;
            if(!(vx1==0&&new_vx==0&&vy1==0&&new_vy==0))
                speedChanged=true;
        }
        if(!speedChanged&&checkIfAtrest&&v.size()>0&&v[0].getIndex()==0)
            v.clear();
        else if(!speedChanged&&checkIfAtrest){//if striker was pocketed
            v.clear();
            fill.clear();
            Ball *b=new Ball();
            b->setIndex(-1);
            fill.push_back(*b);
        }
    else{
        Ball *b=new Ball();
        for(int k=0;k<v.size();k++)
            fill.push_back(*b);
        b=NULL;
        vector<Ball> :: iterator iter1;
        vector<Ball> :: iterator iter2;
        iter1=v.begin();
        iter2=fill.begin();
        copy(iter1,iter1+v.size(),iter2);
    }
    return;

}

void nextEvent(vector<Ball>v,vector<Ball>&fill,int nball,int holes){
    nextEvent(v,fill,true,nball,holes);
    return ;
}

bool simulate(Ball *a,Ball *b,int nball){
        double xrel=a->getPosition()->getX()-b->getPosition()->getX();
        double yrel=a->getPosition()->getY()-b->getPosition()->getY();
        double dist=sqrt(xrel*xrel+yrel*yrel);
        double a_mass=a->getMass(),b_mass=b->getMass();
        Components* alongvector=new Components(xrel/dist,yrel/dist);
        Components* apara=(a->VelocityAlongUnitVector(alongvector));
        Components* aperp=(a->VelocityPerpendicularTOUnitVector(alongvector));
        Components* bpara=(b->VelocityAlongUnitVector(alongvector));
        Components* bperp=(b->VelocityPerpendicularTOUnitVector(alongvector));
        double term1x=a_mass*apara->getX()+b_mass*bpara->getX();
        double term1y=a_mass*apara->getY()+b_mass*bpara->getY();
        double term2x=COR*(bpara->getX()-apara->getX());
        double term2y=COR*(bpara->getY()-apara->getY());
        a->setVelocity((term1x+term2x*b_mass)/(a_mass+b_mass)+aperp->getX(),(term1y+term2y*b_mass)/(a_mass+b_mass)+aperp->getY());
        b->setVelocity((term1x-term2x*a_mass)/(a_mass+b_mass)+bperp->getX(),(term1y-term2y*a_mass)/(a_mass+b_mass)+bperp->getY());
        if(b->getIndex()>=nball){
                b->setVelocity(0,0);
                a->deleteBall();
                return false;
        }
        return true;
}
