#ifndef CARROM_H
#define CARROM_H

#include<vector>
#include"ball.h"
#include<string>
#include"variables.h"
#include<cmath>

#define NO_CLIENT -1
#define LOCAL_SERVER -2

#define NONE -1
#define PERSON 0
#define COMPUTER 1
#define CLIENTCOMP 3
#define ONLINE 4

using namespace std;
struct Ballinfo{
    vector<Ball> vec;
    double time;
    bool stagnant;
    int nball;
    int holes;
    int simulation;
    int byplayer;
    int gamenumber;
    bool newgame;
    bool updatescores;
    int scores[4];
};
char*createMessage(struct Ballinfo );
struct Ballinfo *reviveMessage(char *);
void display_vector(vector<Ball> &);
vector<Ball> &initialize();
void *hostAgame(void *);
void *clientConnected(void *);
void *readAndCreate(void *);
void setUpClient(char *,char *);
void *messageReceived(void *);
void add_to_ballinfo_queue(struct Ballinfo *,bool);
void add_to_ballinfo_queue(struct Ballinfo *);
Ball *newStriker(void);
int getRelatedClient(int *,int );
int getPlayerColor(int*,int );
bool isNextTurnTrigger(struct Ballinfo* );
bool nextTurn(int);
int startagame(int, char**);
bool isStrikerPresent(vector<Ball> );
int countColor(vector<Ball> ,int );
void broadcast(string ,int);
void *client_timeout(void *);

extern string playernames[4];
extern int current_player;
extern int score[4];
extern bool won;
extern string winmsg;
extern int playerno;

bool isSeperateBall(Ball ,vector<Ball>);
#endif // CARROM_H
