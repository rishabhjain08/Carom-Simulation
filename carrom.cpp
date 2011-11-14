#include"ball.h"
#include<string>
#include<string.h>
#include<iostream>
#include"carrom.h"
#include<stdlib.h>
#include<sstream>
#include"carromgui.h"
#include"variables.h"
#include"physics.h"
#include<cmath>
#include<queue>
#include"client.h"
#include"server.h"
#include"ai.h"

int NBALL=20;
int HOLES=4;
queue<struct Ballinfo*> ballinfo_queue;
Server *server[3];
int players[4];
pthread_t clientReplyThread;
string playernames[4];
int score[4];
string winmsg="";
bool won=false;
int wait_for_client=15;
int level=0;
int levels[4];
int wait_for_connections_t=20;
bool got_connected[3]={false,false,false};
int server_no=0;
int start_port=2687;
Client *client;
int client_port=0;
bool clienthost=false;
bool connection_fine=false;
bool serverhost=false;
int playerno=0;
int salgo[4];
int localalgo=1;
pthread_mutex_t msgreceived=PTHREAD_MUTEX_INITIALIZER;

using namespace std;

void printPlayers(int *p){
    for(int i=0;i<4;i++){
        if(p[i]==PERSON){
            cout<<"PERSON";
        }
        else if(p[i]==COMPUTER){
            cout<<"COMPUTER";
        }
        else if(p[i]==NONE){
            cout<<"NONE";
        }
        else if(p[i]==CLIENTCOMP){
            cout<<"CLIENTCOMP";
        }
        if(i<3){
            cout<<",";
        }
    }
}


int countplayers(int *p,int k){
    int count=0;
    for(int i=0;i<4;i++){
        if(k!=ONLINE&&p[i]==k)
            count++;
        else if(k==ONLINE&&((p[i]==PERSON||p[i]==CLIENTCOMP)||(i==0&&p[0]==COMPUTER)))
            count++;
    }
    return count;
}

int getRelatedClient(int *p,int pos){
    int count=-2;
    for(int i=0;i<=pos;i++){
        if(i==0)
            count++;
        else if(p[i]==PERSON||p[i]==CLIENTCOMP)
            count++;
    }
    return count;
}

char*createMessage(struct Ballinfo binfo){
    double x,y,vx,vy,mass,radius;
    int color,index;
    string varAsString ;
    ostringstream sstream;
    for(int i=0;i<binfo.vec.size();i++){
        index=binfo.vec[i].getIndex();
        x=binfo.vec[i].getPosition()->getX();
        y=binfo.vec[i].getPosition()->getY();
        vx=binfo.vec[i].getVelocity()->getX();
        vy=binfo.vec[i].getVelocity()->getY();
        mass=binfo.vec[i].getMass();
        radius=binfo.vec[i].getRadius();
        color=binfo.vec[i].getColor();
        sstream << index;
        sstream << ",";
        sstream << x;
        sstream << ",";
        sstream << y;
        sstream << ",";
        sstream << vx;
        sstream << ",";
        sstream << vy;
        sstream << ",";
        sstream << mass;
        sstream << ",";
        sstream << radius;
        sstream << ",";
        sstream << color;
        sstream << ",";
    }
    sstream << "extra:";
    sstream << binfo.byplayer;
    sstream << ",";
    sstream << binfo.newgame;
    sstream << ",";
    sstream << binfo.gamenumber;
    sstream << ",";
    sstream << binfo.updatescores;
    sstream << ",";
    sstream << binfo.scores[0];
    sstream << ",";
    sstream << binfo.scores[1];
    sstream << ",";
    sstream << binfo.scores[2];
    sstream << ",";
    sstream << binfo.scores[3];
    sstream << ",";
    varAsString = sstream.str();
    char *a;
    a=(char *)calloc(varAsString.size()+1,sizeof(char));
    int yind=0;
    for(yind=0;yind<varAsString.size();yind++)
      a[yind]=varAsString[yind];
    a[yind]='\0';
    return a;
}

struct Ballinfo *reviveMessage(char *msg){
    struct Ballinfo *info;
    info=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
    string l=msg;
    string s=l.substr(0,l.find("extra")),g="";
    string extras=l.substr(l.find("extra:")+6);
    double double_value;
    vector<Ball> vec;
    double x,y,vx,vy,mass,radius;
    int color,index;
    int from=-1;
    int slen=s.size();
    int playernumber=0;
    int com1,com2,com3,com4,com5,com6,com7,com8;
    Ball *b=new Ball();
    while(from<slen){
        com1=s.find(",",from+1);
        com2=s.find(",",com1+1);
        com3=s.find(",",com2+1);
        com4=s.find(",",com3+1);
        com5=s.find(",",com4+1);
        com6=s.find(",",com5+1);
        com7=s.find(",",com6+1);
        com8=s.find(",",com7+1);
        if(com1==-1||com2==-1||com3==-1||com4==-1||com5==-1||com6==-1||com7==-1){
            info->vec=vec;
	    com1=extras.find(",",0);
	    com2=extras.find(",",com1+1);
	    com3=extras.find(",",com2+1);
	    com4=extras.find(",",com3+1);
	    com5=extras.find(",",com4+1);
	    com6=extras.find(",",com5+1);
	    com7=extras.find(",",com6+1);
	    com8=extras.find(",",com7+1);
	    g=extras.substr(0,com1);
            istringstream i(g);
            i>>info->byplayer;
            
	    g=extras.substr(com1+1,com2-com1-1);
            istringstream il1(g);
            il1>>info->newgame;
            
	    g=extras.substr(com2+1,com3-com2-1);
            istringstream il2(g);
            il2>>info->gamenumber;

	    g=extras.substr(com3+1,com4-com3-1);
            istringstream il3(g);
            il3>>info->updatescores;
	    
	    g=extras.substr(com4+1,com5-com4-1);
            istringstream il4(g);
            il4>>info->scores[0];
	    
	    g=extras.substr(com5+1,com6-com5-1);
            istringstream il5(g);
            il5>>info->scores[1];
	    
	    g=extras.substr(com6+1,com7-com6-1);
            istringstream il6(g);
            il6>>info->scores[2];
	    
	    g=extras.substr(com7+1,com8-com7-1);
            istringstream il7(g);
            il7>>info->scores[3];
		    info->simulation=1;
            info->stagnant=0;
            return info;
      }
        g=s.substr(from+1,com1-(from+1));
        istringstream i(g);
        i>>index;
        g=s.substr(com1+1,com2-(com1+1));
        istringstream i1(g);
        i1>>x;
        g=s.substr(com2+1,com3-(com2+1));
        istringstream i2(g);
        i2>>y;
        g=s.substr(com3+1,com4-(com3+1));
        istringstream i3(g);
        i3>>vx;
        g=s.substr(com4+1,com5-(com4+1));
        istringstream i4(g);
        i4>>vy;
        g=s.substr(com5+1,com6-(com5+1));
        istringstream i5(g);
        i5>>mass;
        g=s.substr(com6+1,com7-(com6+1));
        istringstream i6(g);
        i6>>radius;
        g=s.substr(com7+1,com8-(com7+1));
        istringstream i7(g);
        i7>>color;
        b->setIndex(index);
        b->setPosition(x,y);
        b->setVelocity(vx,vy);
        b->setMass(mass);
        b->setRadius(radius);
        b->setColor(color);
        vec.push_back(*b);
        from=com8;
    }
    info->simulation=1;
    info->stagnant=0;
    return NULL;
}

void display_vector(vector<Ball> &vec){
    int i=0,j=0;
    Ball a;
    while(i<(vec).size()){
        a=vec[i];
        cout <<"("<<a.getPosition()->getX()<<","<<a.getPosition()->getY()<<","<<a.getVelocity()->getX()<<","<<a.getVelocity()->getY()<<")"<<"("<<a.getMass()<<","<<a.getRadius()<<","<<a.getColor()<<")\n";
        i++;
    }
}

Ball *newStriker(){
    Ball *b=new Ball();
    b->setPosition(-15.0,-15.0);
    b->setMass(strikerMass);
    b->setRadius(strikerDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(SILVER);
    b->setIndex(0);
    return b;
}

vector<Ball> &initialize(vector<Ball> &ballvec){
    float xerror=0.30f;
    float yerror=0.20f;
    ballvec.push_back(*newStriker());
    Ball *b=new Ball();
    //RED
    b->setPosition(0,0);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(RED);
    b->setIndex(1);
    ballvec.push_back(*b);
   //middle line
    b->setPosition(0,carrommenDia+yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(2);
    ballvec.push_back(*b);

    b->setPosition(0,carrommenDia*2.0f+2*yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(3);
    ballvec.push_back(*b);

    b->setPosition(0,-carrommenDia-yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(4);
    ballvec.push_back(*b);

    b->setPosition(0,-carrommenDia*2.0f-2*yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(5);
    ballvec.push_back(*b);
    //1 left
    b->setPosition(-sqrt(3)*carrommenDia/2.0f-xerror,(carrommenDia+yerror)/2.0f);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(6);
    ballvec.push_back(*b);

    b->setPosition(-sqrt(3)*carrommenDia/2.0f-xerror,1.50f*(carrommenDia+yerror));
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(7);
    ballvec.push_back(*b);

    b->setPosition(-sqrt(3)*carrommenDia/2.0f-xerror,-(carrommenDia+yerror)/2.0f);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(8);
    ballvec.push_back(*b);

    b->setPosition(-sqrt(3)*carrommenDia/2.0f-xerror,-1.50f*(carrommenDia+yerror));
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(9);
    ballvec.push_back(*b);

    //1 right
    b->setPosition(sqrt(3)*carrommenDia/2.0f+xerror,(carrommenDia+yerror)/2.0f);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(10);
    ballvec.push_back(*b);

    b->setPosition(sqrt(3)*carrommenDia/2.0f+xerror,1.50f*(carrommenDia+yerror));
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(11);
    ballvec.push_back(*b);

    b->setPosition(sqrt(3)*carrommenDia/2.0f+xerror,-(carrommenDia+yerror)/2.0f);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(12);
    ballvec.push_back(*b);

    b->setPosition(sqrt(3)*carrommenDia/2.0f+xerror,-1.50*(carrommenDia+yerror));
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(13);
    ballvec.push_back(*b);

    
    /*//2 left
    b->setPosition(-sqrt(3)*carrommenDia-2*xerror,0.0f);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(14);
    ballvec.push_back(*b);

    b->setPosition(-sqrt(3)*carrommenDia-2*xerror,carrommenDia+yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(15);
    ballvec.push_back(*b);

    b->setPosition(-sqrt(3)*carrommenDia-2*xerror,-carrommenDia-yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(16);
    ballvec.push_back(*b);
    //2 right
    b->setPosition(sqrt(3)*carrommenDia+2*xerror,0.0f);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(BLACK);
    b->setIndex(17);
    ballvec.push_back(*b);

    b->setPosition(sqrt(3)*carrommenDia+2*xerror,carrommenDia+yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(18);
    ballvec.push_back(*b);

    b->setPosition(sqrt(3)*carrommenDia+2*xerror,-carrommenDia-yerror);
    b->setMass(carrommenMass);
    b->setRadius(carrommenDia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(YELLOW);
    b->setIndex(19);
    ballvec.push_back(*b);
   
    *///holes
    b->setPosition(-board_side/2.0f+pocket_dia/2.0f,-board_side/2.0f+pocket_dia/2.0f);
    b->setMass(1);
    b->setRadius(pocket_dia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(TRANS);
    b->setIndex(14);
    ballvec.push_back(*b);

    b->setPosition(board_side/2.0f-pocket_dia/2.0f,-board_side/2.0f+pocket_dia/2.0f);
    b->setMass(1);
    b->setRadius(pocket_dia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(TRANS);
    b->setIndex(15);
    ballvec.push_back(*b);

    b->setPosition(board_side/2.0f-pocket_dia/2.0f,board_side/2.0f-pocket_dia/2.0f);
    b->setMass(1);
    b->setRadius(pocket_dia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(TRANS);
    b->setIndex(16);
    ballvec.push_back(*b);

    b->setPosition(-board_side/2.0f+pocket_dia/2.0f,board_side/2.0f-pocket_dia/2.0f);
    b->setMass(1);
    b->setRadius(pocket_dia/2.0f);
    b->setVelocity(0.0,0.0);
    b->setColor(TRANS);
    b->setIndex(17);
    ballvec.push_back(*b);
    return ballvec;
}

int current_player=0;

struct Ballinfo *nextTurnEvent(){
    struct Ballinfo *nte;
    nte=(struct Ballinfo *)calloc(1,sizeof(struct Ballinfo));
    nte->nball=-1;
    nte->holes=-1;
    return nte;
}

void* client_timeout(void* n){
    int o=*((int *)n);
    while(!q.empty()||!ballinfo_queue.empty()){
        pthread_yield();
    }
    sleep(wait_for_client);
    server[getRelatedClient(players,o)]->sendData((void *)"triggerOFF",strlen("triggerOFF"));
    nextTurn(o+1);
}

int startagame(int argc,char **argv){
    int i=0;
    while(i<3){
        players[i]=NONE;
        i++;
    }
    for(i=0;i<4;i++){
        score[i]=0;
         salgo[i]=1;
    }

    struct Ballinfo *init;
    init=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
    init->nball=0;
    init->holes=0;
    vector<Ball> ball_vector;
    init->vec=initialize(ball_vector);
    init->stagnant=0;
    init->simulation=1;
    NBALL=14;
    HOLES=4;

    playernames[0]="player1";
    playernames[1]="player2";
    playernames[2]="player3";
    playernames[3]="player4";
    string myName="UNKNOWN";

    ballinfo_queue.push(init);
     if(argc>4){
            int i=0;
            while(i<4){
                levels[i]=0;
                if(argv[i+1][0]=='p'){
                    players[i]=PERSON;
                    string ys=argv[i+1];
                    if(ys.find("-")!=string::npos){
                        playernames[i]=&ys.substr(ys.find("-")+1)[0];
                    }
                }
    else if(argv[i+1][0]=='c'){
                    players[i]=COMPUTER;
                    string g=argv[i+1];
                    string ys=argv[i+1];
                    if(ys.find("-")!=string::npos){
                        playernames[i]=&ys.substr(ys.find("-")+1)[0];
                        if(ys.find(":")!=string::npos){
                			istringstream y(g.substr(1,ys.find(":")-1));
	                    	 y>>levels[i];
							istringstream yy(g.substr(ys.find(":")+1,ys.find("-")-ys.find(":")-1));
	                    	 yy>>salgo[i];
                	 	}
                    	else{
                    		istringstream y(g.substr(1,ys.find("-")-1));
                    	    y>>levels[i];
                    	    salgo[i]=1;
                     	}
                       
                    }
                    else{
                        if(ys.find(":")!=string::npos){
                			istringstream y(g.substr(1,ys.find(":")-1));
	                    	 y>>levels[i];
							istringstream yy (g.substr(ys.find(":")+1));
	                    	 yy>>salgo[i];
                	 	}
                    	else{
                    		istringstream y(g.substr(1));
                    	    y>>levels[i];
                    	    salgo[i]=1;
                     	}
                       //exit(9);
                    }
                }            else if(argv[i+1][0]=='n'){
                    players[i]=NONE;
                    string ys=argv[i+1];
                    if(ys.find("-")!=string::npos){
                        playernames[i]=&ys.substr(ys.find("-")+1)[0];
                    }
                }
                else if(argv[i+1][0]=='o'){
                    players[i]=CLIENTCOMP;
                    string ys=argv[i+1];
                    if(ys.find("-")!=string::npos){
                        playernames[i]=&ys.substr(ys.find("-")+1)[0];
                    }
                }
                else{
                    cout<<"Invalid argumnets\n";
                    exit(9);
                }
                i++;
            }
    }
    else if(argc>=3&&argv[1][0]=='j'){
        myTurn=false;
        string ys=argv[1];
        level=0;
                      string g=argv[1];
                    if(ys.find("-")!=string::npos){
                        myName=&ys.substr(ys.find("-")+1)[0];
                        if(ys.find(":")!=string::npos){
                			istringstream y(g.substr(1,ys.find(":")-1));
	                    	 y>>level;
							istringstream yy(g.substr(ys.find(":")+1,ys.find("-")-ys.find(":")-1));
	                    	 yy>>localalgo;
                	 	}
                    	else{
                    		istringstream y(g.substr(1,ys.find("-")-1));
                    	    y>>level;
                    	    localalgo=1;
                     	}
                       
                    }
                    else{
                        if(ys.find(":")!=string::npos){
                			istringstream y(g.substr(1,ys.find(":")-1));
	                    	 y>>level;
							istringstream yy (g.substr(ys.find(":")+1));
	                    	 yy>>localalgo;
                	 	}
                    	else{
                    		istringstream y(g.substr(1));
                    	    y>>level;
                    	    localalgo=1;
                     	}
                       //exit(9);
                    }
                    
        setUpClient(argv[2],&myName[0]);
        return 0;
    }
    if(argc>4&&countplayers(players,NONE)!=4){//not a plain board
        //TODO HERE :
        if(countplayers(players,ONLINE)<2){
          if(players[0]==NONE){
                cout<<"Cannot create a game with no player";
                exit(9);//host should be a person or a computer;
            }
            else if(players[0]==CLIENTCOMP){
                cout<<"Host cannot be a client computer player.\n";
                exit(8);
            }
            else if(players[0]==COMPUTER){
                struct Ballinfo* ai_output;
                ai_output=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
                ai_output->stagnant=false;
                ai_output->simulation=true;
                int algonum=localalgo;
                if(!clienthost)
                	algonum=salgo[0];
				ai(0,getPlayerColor(players,0),levels[0],ball_vector,ai_output->vec,HOLES);
                ai_output->byplayer=0;
				add_to_ballinfo_queue(ai_output);
                add_to_ballinfo_queue(nextTurnEvent(),false);
	    }
            else{
                myTurn=true;
            }
        }
        else if(countplayers(players,ONLINE)>1){
            if(players[0]==NONE){
                cout<<"Cannot create a game with no player";
                exit(9);//host should be a person or a computer;
            }
            else if(players[0]==CLIENTCOMP){
                cout<<"Server cannot be a client computer.\n";
                exit(9);
            }
            myTurn=false;
		            
			hostAgame(NULL);
            myTurn=true;
            if(players[0]==COMPUTER){
                struct Ballinfo* ai_output;
                ai_output=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
                ai_output->stagnant=false;
                ai_output->simulation=true;
                int algonum=localalgo;
                if(!clienthost)
                	algonum=salgo[0];
                ai(0,getPlayerColor(players,0),levels[0],ball_vector,ai_output->vec,HOLES,algonum);
                ai_output->byplayer=0;
                add_to_ballinfo_queue(ai_output);
                add_to_ballinfo_queue(nextTurnEvent(),false);
            }
            else if(players[0]==PERSON){
                myTurn=true;
            }
        }
    }
}

void *messageReceived(void *node){
    pthread_mutex_lock(&msgreceived);//becase three server threads may be running at a time all trying to access the same function
    char *msg=(char *) ( ( ( struct data * ) node)->msg );
    void *args=( ( struct data * ) node)->args;
    int client_no=0;
    string str_msg=msg;
    if(serverhost&&args!=NULL){
        client_no=*((int *)args);
    }
    if(!clienthost&&str_msg.find("r8f#454gf45$$")!=string::npos){
        ostringstream sstream;
        int i=0;
        while(i<4){
            if(getRelatedClient(players,i)==client_no){
                sstream << i;
                break;
            }
            i++;
        }
        playernames[i]=str_msg.substr(str_msg.find("cname:")+6);
        server[client_no]->sendData((void*)(&("^$-5n7e5$%6(5index"+sstream.str())[0]),("^$-5n7e5$%6(5index"+sstream.str()).size());
        vector<Ball> vec;
        initialize(vec);
        ostringstream nball;
        nball << NBALL;
        ostringstream holes;
        holes << HOLES;
		struct Ballinfo temp;
		temp.vec=vec;
		temp.simulation=true;
		temp.stagnant=false;
		temp.byplayer=0;
		temp.newgame=true;
		temp.gamenumber=1;
		temp.updatescores=true;
		for(int rt=0;rt<4;rt++)
		  temp.scores[rt]=0;
        string y=createMessage(temp);
        string g="new_ballinfo_queue:"+nball.str()+","+holes.str()+","+y;
        server[client_no]->sendData(&g[0],g.size());
    }
    else if(clienthost&&str_msg.find("^$-5n7e5$%6(5")!=string::npos){
		istringstream i(str_msg.substr(str_msg.find("index")+5));
        i>>playerno;
        connection_fine=true;
    }
    else if(str_msg.find("scores")!=string::npos){
        int col1=str_msg.find_first_of(':');
        int col2=str_msg.find_first_of(':',col1+1);
        int col3=str_msg.find_first_of(':',col2+1);
        int col4=str_msg.find_first_of(':',col3+1);
        int col5=str_msg.find_first_of(':',col4+1);
        istringstream i1(str_msg.substr(col1+1,col2-col1-1));
        istringstream i2(str_msg.substr(col2+1,col3-col2-1));
        istringstream i3(str_msg.substr(col3+1,col4-col3-1));
        istringstream i4(str_msg.substr(col4+1,col5-col4-1));
        i1 >> score[0] ;
        i2 >> score[1] ;
        i3 >> score[2] ;
        i4 >> score[3] ;
    }
    else if(str_msg.find("pnames")!=string::npos){
		    int col1=str_msg.find_first_of(':');
            int col2=str_msg.find_first_of(':',col1+1);
            int col3=str_msg.find_first_of(':',col2+1);
            int col4=str_msg.find_first_of(':',col3+1);
            int col5=str_msg.find_first_of(':',col4+1);
            playernames[0]=str_msg.substr(col1+1,col2-col1-1);
            playernames[1]=str_msg.substr(col2+1,col3-col2-1);
            playernames[2]=str_msg.substr(col3+1,col4-col3-1);
            playernames[3]=str_msg.substr(col4+1,col5-col4-1);
        }
        else if(str_msg=="chanceover"){
            if(!clienthost&&getRelatedClient(players,current_player)==client_no){
                pthread_cancel(clientReplyThread);
            }
            while(!ballinfo_queue.empty()){
                pthread_yield();
            }
            add_to_ballinfo_queue(nextTurnEvent(),false);
        }
        else if(str_msg=="triggermyTurn"){
            while(!ballinfo_queue.empty()){
                pthread_yield();
            }
                  myTurn=true;
        }
        else if(str_msg=="triggerAIEvent"){
            myTurn=false;
            while(!ballinfo_queue.empty()){
                pthread_yield();
            }
            struct Ballinfo* ai_output;
            ai_output=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
            ai_output->stagnant=false;
            ai_output->simulation=true;
                int algonum=localalgo;
                if(!clienthost)
                	algonum=salgo[playerno];
            if(q.empty())
                ai(playerno,getPlayerColor(players,playerno), level,curr_coins,ai_output->vec,HOLES,algonum);
            else
                ai(playerno,getPlayerColor(players,playerno),level,(q.back())->vec,ai_output->vec,HOLES,algonum);
            ai_output->byplayer=playerno;
            add_to_ballinfo_queue(ai_output);
            add_to_ballinfo_queue(nextTurnEvent(),false);
        }
        else if(str_msg.find("insert_ballinfo_queue")!=string::npos){
            struct Ballinfo *temp;
            temp=reviveMessage(&str_msg.substr(str_msg.find("insert_ballinfo_queue:")+strlen("insert_ballinfo_queue:"))[0]);
            add_to_ballinfo_queue(temp,false);
            if(serverhost)
                broadcast(msg,(*((int *)(((struct data *)node)->args))));
        }
        else if(str_msg.find("new_ballinfo_queue")!=string::npos){
		    int colon=str_msg.find_first_of(':');
            int com1=str_msg.find_first_of(',');
            int com2=str_msg.find_first_of(',',com1+1);
            istringstream i(str_msg.substr(colon+1,com1-(colon+1)));
            i>>NBALL;
            istringstream i1(str_msg.substr(com1+1,com2-(com1+1)));
            i1>>HOLES;
            string binfo=str_msg.substr(com2+1);
            struct Ballinfo *temp;
            temp=reviveMessage(&binfo[0]);
            add_to_ballinfo_queue(temp,false);
        }
        else if(clienthost&&str_msg=="triggerOFF"){
            myTurn=false;
        }
        else if(clienthost&&str_msg.find("win")!=string::npos){
		  won=true;
		  winmsg=str_msg.substr(str_msg.find_first_of(':'));
		}
        else if(!clienthost){
            int i=0;
            while(i<4){
                if(getRelatedClient(players,i)==client_no){
                    break;
                }
                i++;
            }

            players[i]=NONE;
        }
        pthread_mutex_unlock(&msgreceived);
        return NULL;
}

bool isSeperateBall(Ball a,vector<Ball>vec){
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


void broadcast(string message,int from_client){
    if(!serverhost)
        return;
    int i=0;
    while(i<3){
        if(i==from_client||server[i]==NULL){
            i++;
            continue;
        }
        server[i]->sendData((void *)&((message)[0]),message.size());
        i++;
    }
}

void setUpClient(char *ip,char *clientname){
    clienthost=true;
    serverhost=false;
    int begin_port=start_port;
    client=new Client();
    int client_state=0;
    string curr_port="";
    while(begin_port-start_port<50){
        ostringstream sstream;
        sstream << begin_port;
        curr_port=sstream.str();
        connection_fine=false;
        client_state=client->start(ip,&curr_port[0],messageReceived,NULL);
        if(client_state!=0){
            client->shut();
            begin_port++;
            continue;
        }
        else{
            string cname=clientname;
            string tosend="r8f#454gf45$$";
            tosend+="cname:";
            tosend+=cname;
            if(client->sendData((void*)(&tosend[0]),tosend.size())!=SEND_SUCCESS)
                myTurn=true;
            client_port=begin_port;
            sleep(1);//time given by client to server to respond after which connection is closed
            if(!connection_fine){
                client->shut();
                begin_port++;
                continue;
            }
            break;
        }
    }
}

void *clientConnected(void *args){
    got_connected[*((int *)args)]=true;
}

bool terminate_lookForConnections=false;
void *lookForConnection(void *){
    int server_port_id[3];
	server_no=0;
    int return_code=0;
    while(server_no<3){
        server[server_no]=new Server();
        server_no++;
    }
    server_no=0;
    int begin_port=start_port;
    while(server_no<3){
        ostringstream sstream;
        sstream << begin_port;
        string curr_port=sstream.str();
        terminate_lookForConnections=true;
        server_port_id[server_no]=server_no;
        return_code=server[server_no]->start_unthreaded(&curr_port[0],clientConnected,&server_no,messageReceived,&server_port_id[server_no]);
        if(return_code!=SERVER_SUCCESS){//implies error
            begin_port++;
            continue;
        }
        terminate_lookForConnections=false;
        begin_port++;
        server_no++;
    }
}

void *hostAgame(void *n){
    serverhost=true;
    clienthost=false;
    pthread_t aConnectionThread;
    pthread_create(&aConnectionThread,NULL,lookForConnection,NULL);
    for(int i=0;i<wait_for_connections_t;i++){
      sleep(1);
    }
    pthread_cancel(aConnectionThread);
    int i=0;
    while(i<3){
        if(!got_connected[i]){
            server[i]->shut();
        }
        i++;
    }
    for(i=0;i<3;i++){
        string names="pnames:"+playernames[0]+":"+playernames[1]+":"+playernames[2]+":"+playernames[3]+":";
        server[i]->sendData(&names[0],names.size());
    }
}


void add_to_ballinfo_queue(struct Ballinfo *node,bool send){
    if(clienthost&&node->nball==-1&&node->holes==-1){
        client->sendData((void *)("chanceover"),strlen("chanceover"));
        return;
    }
	ballinfo_queue.push(node);
    if(clienthost&&send){
        string c="insert_ballinfo_queue:";
        string c1=createMessage(*node);
        client->sendData(&(c+c1)[0],c.size()+c1.size());
        return;
    }
    else if(serverhost&&send){
        string c="insert_ballinfo_queue:";
        string c1=createMessage(*node);
        broadcast(c+c1,LOCAL_SERVER);
    }
}

bool nextTurn(int playernumber){
    int i=playernumber;
    int k=0;
    for(k=0;k<4;k++){
        if(players[(i+k)%4]!=NONE)
            break;
    }
    i=(i+k)%4;
    if(players[i]==NONE)
        return false;
    if(players[i]==COMPUTER){
        current_player=i;
        while(!ballinfo_queue.empty()){
            pthread_yield();
        }
        struct Ballinfo s=(*(q.back()));
        struct Ballinfo *ai_output;
        ai_output=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
        ai_output->simulation=true;
        ai_output->stagnant=false;
        ai_output->byplayer=current_player;
        int algonum=localalgo;
        if(!clienthost)
        	algonum=salgo[i];
        ai(i,getPlayerColor(players,i),levels[i],s.vec,ai_output->vec,HOLES,algonum);//BLOCKING
	add_to_ballinfo_queue(ai_output);
        add_to_ballinfo_queue(nextTurnEvent(),false);
    }
    else if(players[i]==PERSON||players[i]==CLIENTCOMP){
        current_player=i;
        int serv_index=getRelatedClient(players,i);
        if(serv_index<0){
            //local machine
            //switch on the mouse and continue
            while(!ballinfo_queue.empty()){
                pthread_yield();
            }
            myTurn=true;
        }
        else if(serverhost){
            myTurn=false;
            current_player=i;
            int succ = 0;
            while(ballinfo_queue.size()){
                pthread_yield();
            }
            int *tellplayer=(int *)calloc(1,sizeof(int));
            *tellplayer=current_player;
            if(players[i]==PERSON){
                pthread_create(&clientReplyThread,NULL,client_timeout,(void *)tellplayer);
                succ = server[serv_index]->sendData((void *)("triggermyTurn"),strlen("triggermyTurn"));//means switch on the mouse in the client
            }
            else if(players[i]==CLIENTCOMP){
                pthread_create(&clientReplyThread,NULL,client_timeout,(void *)tellplayer);
                succ = server[serv_index]->sendData((void *)("triggerAIEvent"),strlen("triggerAIEvent"));//means switch on the ai in the client
            }
            if(succ!=SEND_SUCCESS){
                nextTurn(current_player+1);
            }
        }
    }
    return true;
}


void add_to_ballinfo_queue(struct Ballinfo *node){
    add_to_ballinfo_queue(node,true);
}

int getPlayerColor(int *pla,int playernumber){
    int yellow=0;
    for(int i=0;i<playernumber;i++){
        if(pla[i]!=NONE)
            yellow=1-yellow;
    }
    if(yellow)
        return YELLOW;
    else
        return BLACK;
}

bool isNextTurnTrigger(struct Ballinfo* info){
    return (info->nball==-1&&info->holes==-1);
}

int countColor(vector<Ball> vec,int color){
    int count=0;
    for(int y=0;y<vec.size();y++){
        if(vec[y].getColor()==color)
            count++;
    }
    return count;
}

bool isStrikerPresent(vector<Ball> vec){
    return (vec.size()>0&&vec[0].getIndex()==0);
}

void *readAndCreate(void *n){
    struct Ballinfo *info,*temp;
    struct Ballinfo prev;
    int ballinfoplayer=0;
    bool miss_chance[4];
    bool red_pocketed[4];
    bool replenishred=false;
    int gamewon[4];
    int gamenumber=1;
    int owe[4];
    int colors[4][3];
    
    for(int t=0;t<4;t++){
        miss_chance[t]=false;
        red_pocketed[t]=false;
	score[0]=score[1]=score[2]=score[3]=0;
        colors[t][0]=colors[t][1]=colors[t][2]=0;
	gamewon[t]=0;
	owe[t]=0;
    }
    int turn=0;
    vector<Ball> popped;
    vector<Ball> simulated;
    while(1){
            while(ballinfo_queue.empty()){
                pthread_yield();
           }
            info=ballinfo_queue.front();
	    turn++;
            
	    if(!clienthost&&isNextTurnTrigger(info)){
		turn--;
		ballinfo_queue.pop();
        int mcolor=getPlayerColor(players,ballinfoplayer);
		if(mcolor==YELLOW)
		  mcolor=BLACK;
		else
		  mcolor=YELLOW;
        bool repeatturn=false;
        if((mcolor==YELLOW&&countColor(popped, BLACK)!=countColor(simulated,BLACK))||//if i pocket opponents piece
                (mcolor==BLACK&&countColor(popped, YELLOW)!=countColor(simulated,YELLOW))){
            if((mcolor==YELLOW&&countColor(simulated,BLACK)==0)||(mcolor==BLACK&&countColor(simulated,YELLOW)==0)){
        score[ballinfoplayer]=-3;
			for(int r=0;r<4;r++){
			  if(getPlayerColor(players,r)==mcolor&&players[r]!=NONE){
			    gamewon[r]++;
			    score[r]+=countColor(simulated,mcolor);
			    if(colors[r][2]!=0)
			      score[r]+=3;
			  }
			}
			string s="";
			string y="";
		      for(int r=0;r<4;r++){
			  if(gamewon[r]>7){
			    s+=y;
			    s+=playernames[r];
			    y=" and ";
			  }
			}
			if(s!=""){
			  s+=" won the game";
			  winmsg=s;
			  won=true;
			  string temp1="win:";
			  broadcast(temp1+winmsg,LOCAL_SERVER);
			  continue;
			}
			int winplayer=-1;
			for(int r=0;r<4;r++){
			  if(score[r]>24){
			    winplayer=r;
			    break;
			  }
			}
			if(winplayer!=-1){
			  string s=playernames[winplayer];
			  for(int r=0;r<4;r++){
			      if(r!=winplayer&&getPlayerColor(players,winplayer)==getPlayerColor(players,r)){
				s+=" and ";
				s+=playernames[r];
				s+=" won the game";
				winmsg=s;
				break;
			      }
			  }
			  won=true;
			  string temp1="win:";
			  broadcast(temp1+winmsg,LOCAL_SERVER);
			  continue;
			}
			
			replenishred=false;
			int colors[4][3];
			for(int t=0;t<4;t++){
			    miss_chance[t]=false;
			    red_pocketed[t]=false;
			    owe[t]=0;
			    colors[t][0]=0;
			    colors[t][1]=0;
			    colors[t][2]=0;
			}
			gamenumber++;
			vector<Ball>newgame;
			initialize(newgame);
			struct Ballinfo *temp1;
			temp1=(struct Ballinfo *)calloc(1,sizeof(struct Ballinfo));
			temp1->vec=newgame;
			temp1->stagnant=0;
			temp1->simulation=1;
			temp1->nball=0;
			temp1->holes=0;
			temp1->byplayer=0;
			temp1->gamenumber=gamenumber;
			temp1->newgame=true;
			temp1->updatescores=true;
			for(int sui=0;sui<4;sui++)
			  temp1->scores[sui]=score[sui];
			add_to_graphics_queue(temp1);
			string c="insert_ballinfo_queue:";
			string c1=createMessage(*temp1);//use newgame and gamenumber
			broadcast(c+c1,LOCAL_SERVER);
			nextTurn(0);
			continue;
        }
                    miss_chance[ballinfoplayer]=true;
		}
		if(ballinfoplayer==0){
		  double ballx=popped[0].getPosition()->getX();
		  double bally=popped[0].getPosition()->getY();
		  if(ballx<-baseLinelength/2.0||ballx>baseLinelength/2.0||bally>-(board_side/2.0-baseLinedisplacement)||bally<-(board_side/2.0-baseLinedisplacement+baseLineseperation)){
		    miss_chance[ballinfoplayer]=true;
		  }
		}
		else if(ballinfoplayer==1){
		  double ballx=popped[0].getPosition()->getX();
		  double bally=popped[0].getPosition()->getY();
		  if(bally<-baseLinelength/2.0||bally>baseLinelength/2.0||ballx<(board_side/2.0-baseLinedisplacement)||ballx>(board_side/2.0-baseLinedisplacement+baseLineseperation)){
		    miss_chance[ballinfoplayer]=true;
		  }
		}
		else if(ballinfoplayer==2){
		  double ballx=popped[0].getPosition()->getX();
		  double bally=popped[0].getPosition()->getY();
		  if(ballx<-baseLinelength/2.0||ballx>baseLinelength/2.0||bally<(board_side/2.0-baseLinedisplacement)||bally>(board_side/2.0-baseLinedisplacement+baseLineseperation)){
		    miss_chance[ballinfoplayer]=true;
		  }
		}
		else if(ballinfoplayer==3){
		  double ballx=popped[0].getPosition()->getX();
		  double bally=popped[0].getPosition()->getY();
		  if(bally<-baseLinelength/2.0||bally>baseLinelength/2.0||ballx>-(board_side/2.0-baseLinedisplacement)||ballx<-(board_side/2.0-baseLinedisplacement+baseLineseperation)){
		    miss_chance[ballinfoplayer]=true;
		  }
		}
                if(!isStrikerPresent(simulated)&&countColor(popped,mcolor)==countColor(simulated,mcolor)){
                    miss_chance[ballinfoplayer]=true;
                    owe[ballinfoplayer]++;
                }
                if(countColor(popped,mcolor)!=countColor(simulated,mcolor)||countColor(popped,RED)!=countColor(simulated,RED)){
		  repeatturn=true;
                }
                if(countColor(simulated,mcolor)==0&&countColor(simulated,RED)==0){
 		for(int r=0;r<4;r++){
			  if(getPlayerColor(players,r)!=mcolor&&players[r]!=NONE){
			    gamewon[r]++;
			    if(mcolor==YELLOW)
			      score[r]+=countColor(simulated,BLACK);
			    if(mcolor==BLACK)
			      score[r]+=countColor(simulated,YELLOW);
			    if(colors[r][2]!=0)
			      score[r]+=3;
			  }
			}
			string s="";
			string y="";
		      for(int r=0;r<4;r++){
			  if(gamewon[r]>7){
			    s+=y;
			    s+=playernames[r];
			    y=" and ";
			  }
			}
			if(s!=""){
			  s+=" won the game";
			  winmsg=s;
			won=true;
			string temp1="win:";
			  broadcast(temp1+winmsg,LOCAL_SERVER);
			  continue;
			}
			int winplayer=-1;
			for(int r=0;r<4;r++){
			  if(score[r]>24){
			    winplayer=r;
			    break;
			  }
			}
			if(winplayer!=-1){
			  string s=playernames[winplayer];
			  for(int r=0;r<4;r++){
			      if(r!=winplayer&&getPlayerColor(players,winplayer)==getPlayerColor(players,r)){
				s+=" and ";
				s+=playernames[r];
				s+=" won the game";
				winmsg=s;
				break;
			      }
			  }
			  won=true;
			  string temp1="win:";
			  broadcast(temp1+winmsg,LOCAL_SERVER);
			  continue;
			}
			
			replenishred=false;
			for(int t=0;t<4;t++){
			    miss_chance[t]=false;
			    red_pocketed[t]=false;
			    owe[t]=0;
			    colors[t][0]=0;
			    colors[t][1]=0;
			    colors[t][2]=0;
			}
			gamenumber++;
			vector<Ball>newgame;
			initialize(newgame);
			struct Ballinfo *temp1;
			temp1=(struct Ballinfo *)calloc(1,sizeof(struct Ballinfo));
			temp1->vec=newgame;
			temp1->stagnant=0;
			temp1->simulation=1;
			temp1->nball=0;
			temp1->holes=0;
			temp1->byplayer=0;
			temp1->gamenumber=gamenumber;
			temp1->newgame=true;
			temp1->updatescores=true;
			for(int sui=0;sui<4;sui++)
			  temp1->scores[sui]=score[sui];
			add_to_graphics_queue(temp1);
			string c="insert_ballinfo_queue:";
			string c1=createMessage(*temp1);
			broadcast(c+c1,LOCAL_SERVER);
			nextTurn(0);
			continue;
                }
                if(red_pocketed[ballinfoplayer]&&countColor(popped,mcolor)!=countColor(simulated,mcolor)){
		      //I had pocketd a red in my prevoius chance and in this chance i pocketed one more coin therefore its my red   
		      replenishred=false;
		      red_pocketed[ballinfoplayer]=false;
		      colors[ballinfoplayer][2]=1;
                }
                if(red_pocketed[ballinfoplayer]&&countColor(popped,mcolor)==countColor(simulated,mcolor)){
		      //I had pocketd a red in my prevoius chance and in this chance i did not pocket any coin therefore i replace the red   
		      replenishred=true;
		      red_pocketed[ballinfoplayer]=false;
		      colors[ballinfoplayer][2]=0;
                }
                if(countColor(simulated,RED)!=0&&countColor(popped,mcolor)>0&&countColor(simulated,mcolor)==0){
                    if(mcolor==BLACK)
                        score[ballinfoplayer]-=(3+countColor(simulated,YELLOW));
                    else if(mcolor==YELLOW)
                        score[ballinfoplayer]-=(3+countColor(simulated,BLACK));
                    //i lose the game
                    //Since I pocketed my all of my coins before the red therefore I LOSE
			for(int r=0;r<4;r++){
			  if(getPlayerColor(players,r)==mcolor&&players[r]!=NONE){
			    gamewon[r]++;
			    score[r]+=countColor(simulated,mcolor);
			    if(colors[r][2]!=0)
			      score[r]+=3;
			  }
			}
			string s="";
			string y="";
		      for(int r=0;r<4;r++){
			  if(gamewon[r]>7){
			    s+=y;
			    s+=playernames[r];
			    y=" and ";
			  }
			}
			if(s!=""){
			  s+=" won the game";
			  winmsg=s;
			won=true;
			  string temp1="win:";
			  broadcast(temp1+winmsg,LOCAL_SERVER);
			  continue;
			}
			int winplayer=-1;
			for(int r=0;r<4;r++){
			  if(score[r]>24){
			    winplayer=r;
			    break;
			  }
			}
			if(winplayer!=-1){
			  string s=playernames[winplayer];
			  for(int r=0;r<4;r++){
			      if(r!=winplayer&&getPlayerColor(players,winplayer)==getPlayerColor(players,r)){
				s+=" and ";
				s+=playernames[r];
				s+=" won the game";
				winmsg=s;
				break;
			      }
			  }
			  won=true;
			  string temp1="win:";
			  broadcast(temp1+winmsg,LOCAL_SERVER);
			  continue;
			}
			
			replenishred=false;
			for(int t=0;t<4;t++){
			    miss_chance[t]=false;
			    red_pocketed[t]=false;
			    owe[t]=0;
			    colors[t][0]=0;
			    colors[t][1]=0;
			    colors[t][2]=0;
			}
			gamenumber++;
			vector<Ball>newgame;
			initialize(newgame);
			struct Ballinfo *temp1;
			temp1=(struct Ballinfo *)calloc(1,sizeof(struct Ballinfo));
			temp1->vec=newgame;
			temp1->stagnant=0;
			temp1->simulation=1;
			temp1->nball=0;
			temp1->holes=0;
			temp1->byplayer=0;
			temp1->gamenumber=gamenumber;
			temp1->newgame=true;
			temp1->updatescores=true;
			for(int sui=0;sui<4;sui++)
			  temp1->scores[sui]=score[sui];
			add_to_graphics_queue(temp1);
			string c="insert_ballinfo_queue:";
			string c1=createMessage(*temp1);
			broadcast(c+c1,LOCAL_SERVER);
			nextTurn(0);
		}
		
                if(countColor(popped,mcolor)!=countColor(simulated,mcolor)&&countColor(popped,RED)!=countColor(simulated,RED)){
                    colors[ballinfoplayer][2]=1;
				}
                if(countColor(popped,RED)!=countColor(simulated,RED)&&countColor(popped,mcolor)==countColor(simulated,mcolor)){
                    red_pocketed[ballinfoplayer]=true;
				}
                colors[ballinfoplayer][0]+=countColor(popped,YELLOW)-countColor(simulated,YELLOW);
                colors[ballinfoplayer][1]+=countColor(popped,BLACK)-countColor(simulated,BLACK);
				int remowe=0;
                while(colors[ballinfoplayer][mcolor==YELLOW?0:1]>0&&owe[ballinfoplayer]>0){
                    colors[ballinfoplayer][mcolor==YELLOW?0:1]--;
                    owe[ballinfoplayer]--;
		    remowe++;
                }
                int nextplayer=(ballinfoplayer+1)%4;
                while(players[nextplayer]==NONE){
                    nextplayer++;
                    nextplayer=nextplayer%4;
                }
                if(!isStrikerPresent(simulated))
		  simulated.insert(simulated.begin(),*newStriker());
                vector<Ball> initialvector;
		initialize(initialvector);
                for(int up=0;up<initialvector.size();up++){
                    if(initialvector[up].getColor()!=mcolor)
                        continue;
                    bool found=false;
                    int upindex=initialvector[up].getIndex();
                    for(int down=0;down<simulated.size();down++){
                        if(simulated[down].getColor()!=mcolor)
                            continue;
                        int downindex=simulated[down].getIndex();
                        if(upindex==downindex){
                            found=true;
                            break;
                        }
                    }
                    if(remowe<=0)
		      break;
                    if(found==true)
                        continue;
                    Ball *b=new Ball();
		    b->setRadius(carrommenDia/2.0);
		    b->setMass(carrommenMass);
                    b->setColor(mcolor);
                    b->setIndex(upindex);
                    b->setVelocity(0,0);
                    double displaced=0;
                    double  diff=b->getRadius()*2.1;
                    if(diff==0)
                        diff=1.0;
                    int niter=1,i=0;
                    double pos=0,x,y;
                    for(int iy=0;iy<niter;iy++){
                        pos=displaced;
                        while(pos<board_side/2.0){
                            if(nextplayer==0){
                                y=pos;
                                x=0;
                            }
                            else if(nextplayer==1){
                                x=-pos;
                                y=0;
                            }
                            else if(nextplayer==2){
                                y=-pos;
                                x=0;
                            }
                            else{
                                x=pos;
                                y=0;
                            }
                            b->setPosition(x,y);
                            pos+=diff;
                            if(!isSeperateBall(*b,simulated)){
                                continue;
                            }
                            for(int mk=0;mk<simulated.size();mk++){
			      if(simulated[mk].getIndex()>b->getIndex()){
				simulated.insert(simulated.begin()+mk,*b);
				break;
			      }
			    }
			    remowe--;
                            iy=niter;
                            break;
                        }
                        displaced=carrommenDia/(pow(2,iy+1));
                    }
		}

                if(replenishred&&countColor(simulated,RED)==0){
		    replenishred=false;
                    Ball *b=new Ball();
                    b->setRadius(carrommenDia/2.0);
                    b->setMass(carrommenMass);
                    b->setColor(RED);
                    b->setIndex(1);
                    b->setVelocity(0,0);
                    double displaced=0;
                    double  diff=b->getRadius()*2.1;
                    if(diff==0)
                        diff=1.0;
                    int niter=1,i=0;
                    double pos=0,x,y;
                    for(i=0;i<niter;i++){
                        pos=displaced;
                        while(pos<board_side/2.0){
                            if(nextplayer==0){
                                y=pos;
                                x=0;
                            }
                            else if(nextplayer==1){
                                x=-pos;
                                y=0;
                            }
                            else if(nextplayer==2){
                                y=-pos;
                                x=0;
                            }
                            else{
                                x=pos;
                                y=0;
                            }
                            b->setPosition(x,y);
                            pos+=diff;
                            if(!isSeperateBall(*b,simulated)){
                                continue;
                            }
                            simulated.insert(simulated.begin()+1,*b);
                            i=niter;
                            break;
                        }
                        displaced=carrommenDia/(pow(2,i+1));
                    }
                colors[ballinfoplayer][2]=0;
                }
                struct Ballinfo* replenished;
                replenished=(struct Ballinfo *)calloc(1,sizeof(struct Ballinfo));
                replenished->vec=simulated;
                replenished->holes=0;
                replenished->nball=0;
                replenished->simulation=1;
                replenished->stagnant=0;
                replenished->time=0;
		replenished->updatescores=true;
		for(int sui=0;sui<4;sui++)
		  replenished->scores[sui]=score[sui];
		
		
                if(repeatturn){
                    repeatturn=false;
                    //cout<<"repeating turn to "<<ballinfoplayer<<"\n";  
		    //cout<<"****************************************\n";
		    replenished->byplayer=ballinfoplayer;
		    add_to_graphics_queue(replenished);
		    string c="insert_ballinfo_queue:";
		    string c1=createMessage(*replenished);
		    broadcast(c+c1,LOCAL_SERVER);
		    nextTurn(ballinfoplayer);
                    continue;
                }
                
                int h=(ballinfoplayer+1)%4;
                while(1){
                    while(players[h]==NONE)
                        h=(h+1)%4;
                    if(miss_chance[h]){
                        miss_chance[h]=false;
                        h=(h+1)%4;
                        continue;
                    }
                    else{
                     //cout<<"giving turn to "<<h<<"\n";  
		    //cout<<"****************************************\n";
		    replenished->byplayer=h;
		    add_to_graphics_queue(replenished);
		    string c="insert_ballinfo_queue:";
		    string c1=createMessage(*replenished);
		    //cout<<"sending end of game scores : "<<score[0]<<" "<<score[1]<<" "<<score[2]<<" "<<score[3]<<" "<<c1<<" logo3\n";
		    broadcast(c+c1,LOCAL_SERVER);
		    nextTurn(h);
                        break;
                    }
                }
                continue;
            }
            popped=info->vec;
            ballinfoplayer=info->byplayer;
			while(info->simulation&&!info->stagnant){
	      	add_to_graphics_queue(info);
	        temp=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
            prev=*info;
	      nextEvent(info->vec,temp->vec,NBALL,HOLES);
	        temp->simulation=1;
	        temp->byplayer=info->byplayer;
			temp->newgame=info->newgame;
			temp->gamenumber=info->gamenumber;
			temp->updatescores=info->updatescores;
		for(int oi=0;oi<4;oi++)
		  temp->scores[oi]=info->scores[oi];
                if(temp->vec.empty()){//all the coins stopped and striker is still on the board
                   simulated=prev.vec;
	           		break;
                }
                else if(temp->vec.size()==1&&(temp->vec)[0].getIndex()==-1){//when the striker was pocketed no coin was moving or
		    		if(isStrikerPresent(prev.vec))
		    			prev.vec.erase(prev.vec.begin());//there was no striker on the board while the coins were moving and then the coins stopped
                    simulated=prev.vec;
                    prev.vec.insert(prev.vec.begin(),*newStriker());
                    add_to_graphics_queue(&prev);
                    break;
                }
                else{//coins are still moving
                    info=temp;
                }
            }
            info->stagnant=true;
            ballinfo_queue.pop();
    }
}
