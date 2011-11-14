#ifndef CARROMGUI_H
#define CARROMGUI_H
#include<queue>
#define UP 1
#define DOWN -1
void add(struct Ballinfo *);
void startGUI(void*);
void add_to_graphics_queue(struct Ballinfo*);
void *makeGUIThread(void *);
void myGlutIdle( void );
void drawCuboid(float,float,float);
void drawFace(float, float);
void drawCaromBoardBase(void);
void drawAdjacentFrames(void);
void update(int);
void drawCircle(float);
void drawCircle(float,float,float);
void stencilPockets(void);
void stencilPockets_sub1(float);
void drawBaseLinesandCircles(void);
void drawBaseLinesandCircles_sub2(float);
void drawBaseLinesandCircles_sub1(float);
void drawArrowhead(int);
void drawArrows(void);
void drawArrows_sub1(void);
void drawRing(float, float);
void drawRing(float , float ,float,float);
void drawArch(void);
void drawOuterCircle(void);
void drawInnerCircle(void);
void drawCarrommen(float,float,float,int);
void drawCylinderFace(float,float,float,float);
void drawCylinderFace(float,float);
void startGUI(void);
void add_to_graphics_queue(struct Ballinfo *);
void keyFunc(unsigned char,int ,int);
void keyUPFunc(unsigned char,int,int);
void specialKeyFunc(unsigned int,int,int);
void specialUPKeyFunc(unsigned int,int,int);
void keyOperations(void);
void passiveMouseMotion(int,int);
void triggered(int ,int);
void mouseButtonStateChanged(int,int,int,int);
void mouseInput(void);
void drawText(void);
void drawInfoBoard(void);
void creategame(int);
void mainmenu(int );
void creategame_done(int);
void myGlutIdle (void);
void joinagame(int);
void exitgame(int);
void startjoining(int);
extern queue<struct Ballinfo*> q;
extern bool myTurn;
extern vector<Ball> curr_coins;

void follow(void);
#endif // CARROMGUI_H
