#include<cmath>
#include <GL/glut.h>
#include<queue>
#include<vector>
#include"components.h"
#include"carrom.h"
#include"variables.h"
#include<pthread.h>
#include"ball.h"
#include"text3d.h"
#include<sstream>
#include <GL/glui.h>
#include"carromgui.h"
using namespace std;

pthread_t nextPositionThread;
queue<struct Ballinfo*> q;

vector<Ball> curr_coins;
struct Ballinfo remlast_q;


int key_arr[255];
bool myTurn=false;
bool qatmin=true;
int pass_x=-1,pass_y=-1;
GLdouble c_mouse_x,c_mouse_y,c_mouse_z=board_thick;
bool hitting=false;
double x_angle=0.0,y_angle=0.0,z_angle=0.0;
int screen_width,screen_height;
int gmnum=1;
int displayscores[4]; 

void add_to_graphics_queue(struct Ballinfo *info){
    while(!qatmin||q.size()>10000){
        qatmin=false;
        if(q.size()<1000)
            qatmin=true;
        pthread_yield();
    }
    q.push(info);
}

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
        if(key_arr[key])
        	key_arr[key]=0;
        else
        	key_arr[key]++;
        switch (key) {
                case 27: //Escape key
                        exit(0);
        }
}

//Initializes 3D rendering
void initRendering() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING); //Enable lighting
        glEnable(GL_LIGHT0); //Enable light #0
        glEnable(GL_LIGHT1); //Enable light #1
        glEnable(GL_NORMALIZE); //Automatically normalize normals
        glEnable(GL_BLEND);
        glEnable(GL_LINE_SMOOTH);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glShadeModel(GL_SMOOTH); //Enable smooth shading
        glClearColor(0.3f,0.2f,0.8f,1.0f);
        t3dInit();
}

//Called when the window is resized
void handleResize(int w, int h) {
        screen_width=w;
        screen_height=h;
        glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void keyOperations(void){
	x_angle+=key_arr['a']*2;
	y_angle+=key_arr['s']*2;
	z_angle+=key_arr['d']*2;

	if(key_arr['r']){
		x_angle=y_angle=z_angle=0;
		key_arr['a']=key_arr['s']=key_arr['d']=0;
		key_arr['r']=0;
	}
}

void triggered(int x,int y){
        pass_x=x;
        pass_y=y;
}

void mouseButtonStateChanged(int button,int state,int x,int y){
    if(hitting&&state==GLUT_UP&&button==GLUT_LEFT_BUTTON){
        hitting=false;
        if(!myTurn||curr_coins.size()==0||curr_coins[0].getIndex()!=0||q.size()>1)
            return;
        vector<Ball>vec(curr_coins);
        double cordX1=curr_coins[0].getPosition()->getX();
        double cordY1=curr_coins[0].getPosition()->getY();
        double cordX2=c_mouse_x;
        double cordY2=c_mouse_y;
        double fac=0.0f;
        double max=baseLinedisplacement+baseLineseperation;
        double len=sqrt((cordX1-cordX2)*(cordX1-cordX2)+(cordY1-cordY2)*(cordY1-cordY2));
        if(len>max){
            fac=(max/len)*5;
        }
        else
            fac=5.0;
        vec[0].setVelocity((cordX1-cordX2)*fac,(cordY1-cordY2)*fac);
        struct Ballinfo* inf;
        inf=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
        inf->vec=vec;
        inf->holes=remlast_q.holes;
        inf->nball=remlast_q.nball;
        inf->simulation=1;
        inf->stagnant=0;
        inf->time=0;
        inf->byplayer=playerno;
        myTurn=false;
        add_to_ballinfo_queue(inf);
        struct Ballinfo* chanceOver;
        chanceOver=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
        chanceOver->holes=-1;
        chanceOver->nball=-1;
        add_to_ballinfo_queue(chanceOver,false);
    }
    else if(hitting){
        hitting=false;
    }
    else if(state==GLUT_DOWN&&button==GLUT_LEFT_BUTTON){
        hitting=true;
    }
}

void changeTheCord(){
        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
        glGetDoublev( GL_PROJECTION_MATRIX, projection );
        glGetIntegerv( GL_VIEWPORT, viewport );

        winX = (float)pass_x;
        winY = (float)viewport[3] - (float)pass_y;
        glReadPixels( pass_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

        gluUnProject( winX, winY, winZ, modelview, projection, viewport, &c_mouse_x, &c_mouse_y, &c_mouse_z);
}

void passiveMouseMotion(int x,int y){
    pass_x=x;
    pass_y=y;
}

void follow(void){
    if(!myTurn||hitting||curr_coins.size()==0||curr_coins[0].getIndex()!=0||q.size()>1||c_mouse_x>board_side/2.0f-curr_coins[0].getRadius()||c_mouse_x<-board_side/2.0f+curr_coins[0].getRadius()||c_mouse_y>board_side/2.0f-curr_coins[0].getRadius()||c_mouse_y<-board_side/2.0f+curr_coins[0].getRadius()||pass_x<0||pass_x>screen_width||pass_y<0||pass_y>screen_height||(c_mouse_x==curr_coins[0].getPosition()->getX()&&c_mouse_y==curr_coins[0].getPosition()->getY())||pass_x==-1||pass_x==-1)
        return;
    struct Ballinfo *abc;
    abc=(struct Ballinfo*)calloc(1,sizeof(struct Ballinfo));
    vector<Ball> vec(curr_coins);
    vec[0].setPosition(c_mouse_x,c_mouse_y);
    vec[0].setVelocity(0.0,0.0);
    abc->vec=vec;
    abc->holes=remlast_q.holes;
    abc->nball=remlast_q.nball;
    abc->simulation=true;
    abc->stagnant=false;
    abc->byplayer=playerno;
    add_to_ballinfo_queue(abc);
    curr_coins=vec;
}

void mouseInput(void){
    if(!myTurn||q.size()!=0||!hitting||curr_coins.size()==0||curr_coins[0].getIndex()!=0)
        return;
    glColor4f(0.7f , 0.4f , 0.4f , 0.7f);
    double cordX=curr_coins[0].getPosition()->getX();
    double cordY=curr_coins[0].getPosition()->getY();
    glBegin(GL_LINES);
    glVertex3f(cordX,cordY,board_thick/2.0+carrommenThick/2.0);
    glVertex3f(c_mouse_x,c_mouse_y,board_thick/2.0+carrommenThick/2.0);
    glEnd();
}

void drawInfoBoard(){
	 ostringstream sstream;
	 sstream <<"Round Number ";
	 sstream << gmnum;
	 string tdraw1=sstream.str();
	 
	 glPushMatrix();
     glTranslatef(board_side/2.0f+frame_width*3+4,board_side/2.0f+frame_width,board_thick/2.0f+0.05);
 	 float scalef=board_side/(2.5*t3dDrawWidth(&tdraw1[0]));
	 glScalef(scalef,scalef,scalef);
	 glColor3f(0.0f, 0.0f, 1.0f);
	 t3dDraw3D(&tdraw1[0], 0, 0, 0.2f);
	 glPopMatrix();
}

void drawText(){
	if(won&&q.size()==0){
		float scalef=board_side/(t3dDrawWidth(&winmsg[0]));
		glPushMatrix();
	  	glScalef(scalef,scalef,scalef);
	      	glColor3f(0.0f, 1.0f, 0.0f);
	  	t3dDraw3D(&winmsg[0], 0, 0, 0.2f);
		glPopMatrix();
		return;
	}
    float maxWidth = 0;
     for(int i = 0; i < 4; i++) {
         float width = t3dDrawWidth(playernames[i]);
         if (width > maxWidth) {
             maxWidth = width;
         }
     }
	float scalef=board_side/(maxWidth*6.0);
		glPushMatrix();
		for(int i = 0; i < 4; i++) {
		  glPushMatrix();
		  glRotatef(90 * i, 0, 0, 1);
		  glTranslatef(0,-board_side/2.0 -frame_width-6, board_thick/2.0+0.05);
		  glScalef(scalef,scalef,scalef);
		  if(i==remlast_q.byplayer)
			  glColor3f(0.0f, 1.0f, 0.0f);
		  else
			  glColor3f(1.0f, 0.0f, 0.0f);
		  ostringstream sstream;
		  sstream << displayscores[i];
		  string tdraw=playernames[i];
		  tdraw+="\n";
		  tdraw+=sstream.str();
		  t3dDraw3D(&tdraw[0], 0, 0, 0.2f);
		  glPopMatrix();
		}
	glPopMatrix();
}


//Draws the 3D scene
void drawScene() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0,0.0,-8.0f);
        glScalef(scale_fac,scale_fac,scale_fac);
        //Add ambient light
        GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

        //Add positioned light
        GLfloat lightColor0[] = {0.85f, 0.85f, 0.85f, 1.0f};
        GLfloat lightPos0[] = {0.0f, 0.0f, -1.0f, 0.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

        glRotatef(x_angle,1.0f,0.0f,0.0f);
      	glRotatef(y_angle,0.0f,1.0f,0.0f);
      	glRotatef(z_angle,0.0f,0.0f,1.0f);
      	
        if(!won||q.size()!=0){
			stencilPockets();
			drawAdjacentFrames();
			drawBaseLinesandCircles();
			drawArrows();
			drawInnerCircle();
			drawOuterCircle();
			int i=0;
			for(i=0;i<curr_coins.size();i++){
				drawCarrommen(((Ball)curr_coins[i]).getPosition()->getX(), ((Ball)curr_coins[i]).getPosition()->getY(),((Ball)curr_coins[i]).getRadius(),((Ball)curr_coins[i]).getColor());
		    }
		}

		drawInfoBoard();
		drawText();
        keyOperations();
        changeTheCord();
        mouseInput();
        follow();
        glutSwapBuffers();
}

void drawCarrommen(float x, float y,float radius,int color){
    float r=0.00f,g=0.00f,b=0.00f;
    float trans_base=0.8f,trans_surface=0.5f;
    float flickrred=0.01f;
    if(color==RED)
        r=0.70f;
    else if(color==YELLOW){
        r=1.00f;
        g=0.66;
    }
    else if(color==TRANS){
        trans_base=0.0f;
        trans_surface=0.0f;
    }
    else if(color==SILVER){
        r=0.7647f;
        g=0.7647f;
        b=0.7647f;
    }
    else if(color==HIGH_TRANS){
        r=0.7647f;
        g=0.7647f;
        b=0.7647f;
        trans_base=0.3f;
        trans_surface=0.3f;
    }
    glPushMatrix();
    glTranslatef(x,y,board_thick/2.00f+carrommenThick+flickrred);
    glColor4f(r,g,b,trans_surface);
    drawCylinderFace(carrommenDia/2.00f,carrommenThick);
    glColor4f(r,g,b,trans_base);
    drawCircle(radius);
    glPushMatrix();
    glTranslatef(0.0f,0.0f,-carrommenThick);
    glColor4f(r,g,b,trans_base);
    drawCircle(radius);
    glPopMatrix();
    glPopMatrix();
}

void drawCylinderFace(float r, float h){
    drawCylinderFace(r, h, 0.00f, 360.00f);
}

void drawCylinderFace(float radius, float h, float t1, float t2){
    while(t1<0.0f)
        t1+=360.0f;
    while(t1>360.0f)
        t1-=360.0f;
    while(t2<0.0f)
        t2+=360.0f;
    while(t2>360.0f)
        t2-=360.0f;

    t1=t1*pi/180.0f;
    t2=t2*pi/180.0f;

    float theta=0.01f;
    float pass=0.00f;
    float MAX_PASS=(2*pi)/theta;
    glBegin(GL_QUADS);
    while(pass<MAX_PASS){
        if(theta*pass>=t1&&theta*(pass)<=t2){
            glVertex3f(radius*cos(theta*pass),radius*sin(theta*pass),0.0f);
            glVertex3f(radius*cos(theta*(pass+1)),radius*sin(theta*(pass+1)),0.0f);
            glVertex3f(radius*cos(theta*(pass+1)),radius*sin(theta*(pass+1)),-h);
            glVertex3f(radius*cos(theta*pass),radius*sin(theta*pass),-h);
        }
        pass+=1.0f;
    }
    glEnd();
}

void drawRing(float r1, float r2,float t1,float t2){
    //Disable color and depth buffers
        glColorMask(false, false, false, false);             //Disable writting in color buffer
        glDepthMask(false);                                  //Disable writting in depth buffer
        glEnable(GL_STENCIL_TEST);                        //Enable Stencil test
        glStencilFunc(GL_ALWAYS, 1, 1);                   //Test always success, value written 1
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  //Stencil & Depth test passes => replace existing value
        drawCircle(r1,t1,t2);
        glDepthMask(true);
        glColorMask(true, true, true, true); //Enable drawing of r, g, b & a}
        glStencilFunc(GL_EQUAL, 0, 1);                   //Draw only where stencil buffer is 1
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);    //Stencil buffer read only
         drawCircle(r2,t1,t2);
        glDisable(GL_STENCIL_TEST);
}

void drawRing(float r1, float r2){
    drawRing(r1, r2,0.0f,360.0f);
}

void drawBaseLinesandCircles(){
glColor3f(0.3f,1.0f,0.9f);
drawBaseLinesandCircles_sub1(board_thick/2.0f+0.1f);
}

void drawBaseLinesandCircles_sub1(float z){
    glPushMatrix();
    drawBaseLinesandCircles_sub2(z);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f,0.0f,0.0f,-1.0f);
    drawBaseLinesandCircles_sub2(z);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180.0f,0.0f,0.0f,-1.0f);
    drawBaseLinesandCircles_sub2(z);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270.0f,0.0f,0.0f,-1.0f);
    drawBaseLinesandCircles_sub2(z);
    glPopMatrix();
}

void drawArrowhead(int dir){
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f,arrowHeadHeight*dir,0.0f);
    glVertex3f(-arrowHeadRatio*arrowHeadHeight/2.0f,0.0f,0.0f);
    glVertex3f(arrowHeadRatio*arrowHeadHeight/2.0f,0.0f,0.0f);
    glEnd();
}

void drawArrows(){
    float flickerred=0.01f;
    glColor3f(0.0f,0.0f,0.0f);

    glPushMatrix();
    glTranslatef(0.0f,0.0f,board_thick/2.00f+flickerred);
    glRotatef(arrowInclination,0.0f,0.0f,-1.0f);
    drawArrows_sub1();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f,0.0f,board_thick/2.00f+flickerred);
    glRotatef(arrowInclination+90.0,0.0f,0.0f,-1.0f);
    drawArrows_sub1();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f,0.0f,board_thick/2.00f+flickerred);
    glRotatef(arrowInclination+180.0,0.0f,0.0f,-1.0f);
    drawArrows_sub1();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f,0.0f,board_thick/2.00f+flickerred);
    glRotatef(arrowInclination+270.0,0.0f,0.0f,-1.0f);
    drawArrows_sub1();
    glPopMatrix();
}

void drawArrows_sub1(){
    float sqrt_2=sqrt(2);
    float finaly=(board_side/sqrt_2)-((1+sqrt_2)*pocket_dia/2.00f+arrowClearing);
    float initialy=finaly-arrowLength;
    glBegin(GL_QUADS);
    glVertex3f(-arrowWidth/2.00f,initialy,0.0f);
    glVertex3f(arrowWidth/2.00f,initialy,0.0f);
    glVertex3f(arrowWidth/2.00f,finaly,0.0f);
    glVertex3f(-arrowWidth/2.00f,finaly,0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f,finaly-arrowHeadHeight,0.0f);
    drawArrowhead(UP);
    glTranslatef(0.0f,-arrowLength+arrowHeadHeight,0.0f);
    drawArch();
    glPopMatrix();
}

void drawArch(){
    glPushMatrix();
    glRotatef(90.0f-archMouth/2.0f,0.0f,0.0f,1.0f);
    drawRing(archDia/2.0f,archDia/2.0f+archWidth,archMouth,360.0f);
    glTranslatef((archDia+archWidth)/2.0f,0.0f,0.0f);
    drawArrowhead(UP);
    glTranslatef(-(archDia+archWidth)/2.0f,0.0f,0.0f);
    glRotatef(archMouth,0.0f,0.0f,1.0f);
    glTranslatef((archDia+archWidth)/2.0f,0.0f,0.0f);
    drawArrowhead(DOWN);
    glPopMatrix();
}

void drawBaseLinesandCircles_sub2(float z){
    float innerRadius=innerDia/2.0f;
    float flicker_reduc=0.01f;
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-baseLinelength/2.0f,board_side/2.0-baseLinedisplacement,z);
    glVertex3f(baseLinelength/2.0f,board_side/2.0-baseLinedisplacement,z);
    glVertex3f(baseLinelength/2.0f,board_side/2.0-baseLinedisplacement-baseLinewidth,z);
    glVertex3f(-baseLinelength/2.0f,board_side/2.0-baseLinedisplacement-baseLinewidth,z);

    glVertex3f(-baseLinelength/2.0f,board_side/2.0-baseLinedisplacement+baseLineseperation-baseLinewidth,z);
    glVertex3f(baseLinelength/2.0f,board_side/2.0-baseLinedisplacement+baseLineseperation-baseLinewidth,z);
    glVertex3f(baseLinelength/2.0f,board_side/2.0-baseLinedisplacement+baseLineseperation-baseLinewidth-baseLinewidth,z);
    glVertex3f(-baseLinelength/2.0f,board_side/2.0-baseLinedisplacement+baseLineseperation-baseLinewidth-baseLinewidth,z);

    glEnd();

    glPushMatrix();
    glTranslatef(-baseLinelength/2.0f,board_side/2.0f-baseLinedisplacement-baseLinewidth+baseLineseperation/2.0f,z);
    glColor3f(0.0f,0.0f,0.0f);
    drawCircle(baseLineseperation/2.0f);
    glColor3f(1.0f,0.0f,0.0f);
    glTranslatef(0.0f,0.0f,flicker_reduc);
    drawCircle(innerRadius);
    glTranslatef(baseLinelength,0.0f,-flicker_reduc);
    glColor3f(0.0f,0.0f,0.0f);
    drawCircle(baseLineseperation/2.0f);
    glColor3f(1.0f,0.0f,0.0f);
    glTranslatef(0.0f,0.0f,flicker_reduc);
    drawCircle(innerRadius);
    glPopMatrix();
}

void drawAdjacentFrames(){
    glColor4f(0.759f,0.550f,0.250f,1.0f);
    glPushMatrix();
    glTranslatef(0.0f,(board_side+frame_width)/2.0f,frame_thick/2.0f);
    drawCuboid(board_side+frame_width*2,frame_thick+board_thick,frame_width);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f,0.0f,0.0f,-1.0f);
    glTranslatef(0.0f,(board_side+frame_width)/2.0f,frame_thick/2.0f);
    drawCuboid(board_side,frame_thick+board_thick,frame_width);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180.0f,0.0f,0.0f,-1.0f);
    glTranslatef(0.0f,(board_side+frame_width)/2.0f,frame_thick/2.0f);
    drawCuboid(board_side+frame_width*2,frame_thick+board_thick,frame_width);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270.0f,0.0f,0.0f,-1.0f);
    glTranslatef(0.0f,(board_side+frame_width)/2.0f,frame_thick/2.0f);
    drawCuboid(board_side,frame_thick+board_thick,frame_width);
    glPopMatrix();
}

void drawOuterCircle(){
    float flickerred=0.01f;
    glColor3f(0.0f,0.0f,0.0f);
    glPushMatrix();
    glTranslatef(0.0f,0.0f,board_thick/2.0f+flickerred);
    drawRing(outerCircleDia/2.0f,(outerCircleDia+outerCircleThick)/2.0f);
    glPopMatrix();
}

void drawInnerCircle(){
    float flickerred=0.01f;
    glColor3f(1.0f,0.0f,0.0f);
    glPushMatrix();
    glTranslatef(0.0f,0.0f,board_thick/2.0f+flickerred);
    drawRing(innerCircleDia/2.0f,(innerCircleDia+innerCircleThick)/2.0f);
    glPopMatrix();
}

void drawCaromBoardBase(){
    glColor3f(0.872f,0.754f,0.394f);
    drawCuboid(board_side,board_thick,board_side);
}

void update(int value){
        if(!q.empty()){
                int j=10;
                while(j>0){
                    if(!q.empty()){
                        remlast_q=*(q.front());
                        curr_coins=((struct Ballinfo *)(q.front()))->vec;
                        if(remlast_q.newgame)
                        	gmnum=remlast_q.gamenumber;
                    	if(remlast_q.updatescores){
                    		for(int yui=0;yui<4;yui++)
                    		displayscores[yui]=remlast_q.scores[yui];
                		}
                        q.pop();
                    }
                    j--;
                }
            }
    glutPostRedisplay();
    glutTimerFunc(1, update, 0);
}

void drawCircle(float radius,float t1, float t2){
    while(t1<0.0f)
        t1+=360.0f;
    while(t1>360.0f)
        t1-=360.0f;
    while(t2<0.0f)
        t2+=360.0f;
    while(t2>360.0f)
        t2-=360.0f;

    t1=t1*pi/180.0f;
    t2=t2*pi/180.0f;

    float theta=0.01f;
    float pass=0.0f;
    float MAX_PASS=(2*pi)/theta;
    glBegin(GL_TRIANGLES);
    while(pass<MAX_PASS){
        if(theta*pass>=t1&&theta*(pass)<=t2){
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(radius*cos(theta*pass),radius*sin(theta*pass),0.0f);
            glVertex3f(radius*cos(theta*(pass+1)),radius*sin(theta*(pass+1)),0.0f);
        }
        pass+=1.0f;
    }
    glEnd();
}

void drawCircle(float radius){
    drawCircle(radius,0.0f,360.0f);
}

void stencilPockets(){
        glEnable(GL_STENCIL_TEST); //Enable using the stencil buffer
//       glColorMask(0, 0, 0, 0); //Disable drawing colors to the screen
//       glDisable(GL_DEPTH_TEST); //Disable depth testing
       glStencilFunc(GL_ALWAYS, 1, 1); //Make the stencil test always pass
       //Make pixels in the stencil buffer be set to 1 when the stencil test passes
//       glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//       glClearStencil(0);
      //Set all of the pixels covered by the floor to be 1 in the stencil buffer
    stencilPockets_sub1(board_thick/2.0f+0.001f);
    stencilPockets_sub1(-board_thick/2.0f-.001f);
//        glColorMask(1, 1, 1, 1); //Enable drawing colors to the screen
//       glEnable(GL_DEPTH_TEST); //Enable depth testing
       //Make the stencil test pass only when the pixel is 1 in the stencil buffer
       glStencilFunc(GL_EQUAL, 0, 1);
//       glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Make the stencil buffer not change*/
       glClear(GL_COLOR_BUFFER_BIT);


       drawCaromBoardBase();
       glDisable(GL_STENCIL_TEST); //Disable using the stencil buffer
}

void stencilPockets_sub1(float z){
    float l=board_side,h=board_side,b=board_thick,radius=pocket_dia/2.0;
    glPushMatrix();
    glTranslatef(-l/2.0f+radius,-h/2.0f+radius,z);
    drawCircle(radius);
    glBegin(GL_QUADS);
    glVertex3f(-radius,-radius,0.0f);
    glVertex3f(0.0f,-radius,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(-radius,0.0f,0.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(l/2.0f-radius,-h/2.0f+radius,z);
    drawCircle(radius);
    glBegin(GL_QUADS);
    glVertex3f(radius,-radius,0.0f);
    glVertex3f(0.0f,-radius,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(radius,0.0f,0.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-l/2.0f+radius,h/2.0f-radius,z);
    drawCircle(radius);
    glBegin(GL_QUADS);
    glVertex3f(-radius,radius,0.0f);
    glVertex3f(0.0f,radius,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(-radius,0.0f,0.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(l/2.0f-radius,h/2.0f-radius,z);
    drawCircle(radius);
    glBegin(GL_QUADS);
    glVertex3f(radius,radius,0.0f);
    glVertex3f(0.0f,radius,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(radius,0.0f,0.0f);
    glEnd();
    glPopMatrix();
}

void drawCuboid(float l, float b, float h){
    glPushMatrix();

    glBegin(GL_QUADS);
    float f1=(float)l/2.0f,f2=(float)h/2.0f;
    glNormal3f(-1.0f,0.0f,-1.0f);
    glVertex3f(-l/2.0f,-h/2.0f,-b/2.0f);
    glNormal3f(-1.0f,0.0f,-1.0f);
    glVertex3f(-l/2.0f,h/2.0f,-b/2.0f);
    glNormal3f(1.0f,0.0f,-1.0f);
    glVertex3f(l/2.0f,h/2.0f,-b/2.0f);
    glNormal3f(1.0f,0.0f,-1.0f);
    glVertex3f(l/2.0f,-h/2.0f,-b/2.0f);

    glNormal3f(-1.0f,0.0f,-1.0f);
    glVertex3f(-l/2.0f,-h/2.0f,b/2.0f);
    glNormal3f(-1.0f,0.0f,-1.0f);
    glVertex3f(-l/2.0f,h/2.0f,b/2.0f);
    glNormal3f(1.0f,0.0f,-1.0f);
    glVertex3f(l/2.0f,h/2.0f,b/2.0f);
    glNormal3f(1.0f,0.0f,-1.0f);
    glVertex3f(l/2.0f,-h/2.0f,b/2.0f);

    glNormal3f(-1.0f,0.0f,-1.0f);
    glVertex3f(-l/2.0f,-h/2.0f,-b/2.0f);
    glNormal3f(-1.0f,0.0f,-1.0f);
    glVertex3f(-l/2.0f,h/2.0f,-b/2.0f);
    glNormal3f(-1.0f,0.0f,1.0f);
    glVertex3f(-l/2.0f,h/2.0f,b/2.0f);
    glNormal3f(-1.0f,0.0f,1.0f);
    glVertex3f(-l/2.0f,-h/2.0f,b/2.0f);

    glNormal3f(1.0f,0.0f,-1.0f);
    glVertex3f(l/2.0f,-h/2.0f,-b/2.0f);
    glNormal3f(1.0f,0.0f,-1.0f);
    glVertex3f(l/2.0f,h/2.0f,-b/2.0f);
    glNormal3f(1.0f,0.0f,1.0f);
    glVertex3f(l/2.0f,h/2.0f,b/2.0f);
    glNormal3f(1.0f,0.0f,1.0f);
    glVertex3f(l/2.0f,-h/2.0f,b/2.0f);

    glVertex3f(-l/2.0f,-h/2.0f,-b/2.0f);
    glVertex3f(-l/2.0f,-h/2.0f,b/2.0f);
    glVertex3f(l/2.0f,-h/2.0f,b/2.0f);
    glVertex3f(l/2.0f,-h/2.0f,-b/2.0f);

    glVertex3f(-l/2.0f,h/2.0f,-b/2.0f);
    glVertex3f(-l/2.0f,h/2.0f,b/2.0f);
    glVertex3f(l/2.0f,h/2.0f,b/2.0f);
    glVertex3f(l/2.0f,h/2.0f,-b/2.0f);

    glEnd();
    glPopMatrix();
    //glPushMatrix();
}

void drawFace(float l, float b){
    glBegin(GL_QUADS);
    glVertex3f(-l/2.0f, -b/2.0f, 0.0f);
    glVertex3f(-l/2.0f, b/2.0f, 0.0f);
    glVertex3f(l/2.0f, -b/2.0f, 0.0f);
    glVertex3f(l/2.0f, b/2.0f, 0.0f);
    glEnd();
}


char pname1[sizeof(GLUI_String)],pname2[sizeof(GLUI_String)],pname3[sizeof(GLUI_String)],pname4[sizeof(GLUI_String)],clientname[sizeof(GLUI_String)],ipname[sizeof(GLUI_String)];
int p[4];
int window_id=0;
int plevel[4],clientlevel;
int algo[4];

int main(int argc,char **argv){
        for(int i=0;i<255;i++){
            key_arr[i]=0;
        }
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
        glutInitWindowSize(screen_width=800, screen_height=500);

        //Create the window
        window_id=glutCreateWindow("International Carom-Board");
        initRendering();

		GLUI_Master.set_glutDisplayFunc(drawScene);
        GLUI_Master.set_glutKeyboardFunc(handleKeypress);
        GLUI_Master.set_glutReshapeFunc(handleResize);
        glutMotionFunc(triggered);
        GLUI_Master.set_glutMouseFunc(mouseButtonStateChanged);
        glutPassiveMotionFunc(passiveMouseMotion);
        GLUI_Master.set_glutTimerFunc(25, update, 0); //Add a timer
		GLUI_Master.set_glutIdleFunc (myGlutIdle );
		if(argc==1){
			mainmenu(0);
		}
		else{
			pthread_create(&nextPositionThread,NULL,readAndCreate,NULL);//to keep an eye on the queue of ballinfos to be processed
			startagame(argc,argv);
		  }
        glutMainLoop();
        return 0;
}
void joinagame(int no_need){

  	GLUI_Master.close_all();
	GLUI *glui_subwin = GLUI_Master.create_glui_subwindow (window_id,GLUI_SUBWINDOW_LEFT );
	glui_subwin->set_main_gfx_window( window_id );
	
	clientname[0]='\0';
	GLUI_EditText *pname=glui_subwin->add_edittext( "Player Name " ,GLUI_EDITTEXT_TEXT,clientname);
	pname->set_w(200);

	
	ipname[0]='\0';
	GLUI_EditText *pip=glui_subwin->add_edittext( "Server IP      " ,GLUI_EDITTEXT_TEXT,ipname);
	pip->set_w(200);
	clientlevel=1;
	GLUI_Listbox *p2level=glui_subwin->add_listbox( "Level           " ,(&clientlevel));
	p2level->add_item(1,"Novice");
	p2level->add_item(2,"Beginner");
	p2level->add_item(3,"Expert");
	
	glui_subwin ->add_button( "Join",0,startjoining);
	glui_subwin ->add_button( "Cancel",0,mainmenu);
}

void startjoining(int){
  char *arr[3];
  arr[0]=(char *)calloc(1,sizeof(char));
  arr[1]=(char *)calloc(sizeof(GLUI_String)+20,sizeof(char));
  arr[2]=(char *)calloc(sizeof(GLUI_String),sizeof(char));
  string cname="";
  cname="j";
  ostringstream sstream;
  sstream<<clientlevel;
  cname+=sstream.str();
  cname+="-";
  string cliname=clientname;
  if(cliname=="")
    cname+="CLIENT";
  else
    cname+=cliname;
  arr[1]=&cname[0];
  string ipconn=ipname;
  arr[2]=&ipconn[0];
  pthread_create(&nextPositionThread,NULL,readAndCreate,NULL);//to keep an eye on the queue of ballinfos to be processed
  startagame(3,arr);
  GLUI_Master.close_all();
}
void exitgame(int no_need){
  exit(9);
}

void myGlutIdle (void){
  glutSetWindow(window_id);
  glutPostRedisplay();
}
void mainmenu(int no_need=0){
	
	GLUI_Master.close_all();
	GLUI *glui_subwin = GLUI_Master.create_glui_subwindow (window_id,GLUI_SUBWINDOW_LEFT );
	glui_subwin->set_main_gfx_window( window_id );


	glui_subwin ->add_button( "Create a Game",0,creategame);
	glui_subwin ->add_button( "Join a Game",0,joinagame);
	glui_subwin ->add_button( "Exit",0,exitgame);
}

void creategame(int no_need){
	
	GLUI_Master.close_all();
	GLUI *glui_subwin = GLUI_Master.create_glui_subwindow (window_id,GLUI_SUBWINDOW_LEFT );
	glui_subwin->set_main_gfx_window( window_id );

	for(int i=0;i<4;i++){
	  p[i]=0;
	  plevel[i]=1;
	  algo[i]=1;
	}

	pname1[0]='\0';
	GLUI_EditText *pe1=glui_subwin->add_edittext( "Player 1" ,GLUI_EDITTEXT_TEXT,pname1);

	pname2[0]='\0';
	GLUI_EditText *pe2=glui_subwin->add_edittext( "Player 2" ,GLUI_EDITTEXT_TEXT,pname2);

	pname3[0]='\0';
	GLUI_EditText *pe3=glui_subwin->add_edittext( "Player 3" ,GLUI_EDITTEXT_TEXT,pname3);

	pname4[0]='\0';
	GLUI_EditText *pe4=glui_subwin->add_edittext( "Player 4" ,GLUI_EDITTEXT_TEXT,pname4);

	glui_subwin ->add_button( "Done ",0,creategame_done);
	
	glui_subwin->add_column(false);
	
	GLUI_Listbox *p1=glui_subwin->add_listbox( " " ,(&p[0]));
	p1->add_item(0,"Human          ");
	p1->add_item(1,"Computer         ");
	
	GLUI_Listbox *p2=glui_subwin->add_listbox( " " ,(&p[1]));
	p2->add_item(0,"None");
	p2->add_item(1,"Human");
	p2->add_item(2,"Local Computer");
	p2->add_item(3,"Client Computer");
	GLUI_Listbox *p3=glui_subwin->add_listbox( " " ,(&p[2]));
	p3->add_item(0,"None");
	p3->add_item(1,"Human");
	p3->add_item(2,"Local Computer");
	p3->add_item(3,"Client Computer");
	GLUI_Listbox *p4=glui_subwin->add_listbox( " " ,(&p[3]));
	p4->add_item(0,"None");
	p4->add_item(1,"Human");
	p4->add_item(2,"Local Computer");
	p4->add_item(3,"Client Computer");

	glui_subwin->add_column(false);
	
	GLUI_Listbox *p1level=glui_subwin->add_listbox( " " ,(&plevel[0]));
	p1level->add_item(1,"Newbie");
	p1level->add_item(2,"Amature");
	p1level->add_item(3,"Expert");
	
	GLUI_Listbox *p2level=glui_subwin->add_listbox( " " ,(&plevel[1]));
	p2level->add_item(1,"Novice");
	p2level->add_item(2,"Beginner");
	p2level->add_item(3,"Expert");
	
	GLUI_Listbox *p3level=glui_subwin->add_listbox( " " ,(&plevel[2]));
	p3level->add_item(1,"Novice");
	p3level->add_item(2,"Beginner");
	p3level->add_item(3,"Expert");
	
	GLUI_Listbox *p4level=glui_subwin->add_listbox( " " ,(&plevel[3]));
	p4level->add_item(1,"Novice");
	p4level->add_item(2,"Beginner");
	p4level->add_item(3,"Expert");
	
	glui_subwin->add_column(false);
	
	GLUI_Listbox *p1leve=glui_subwin->add_listbox( " " ,(&algo[0]));
	p1leve->add_item(1,"Algo1");
	p1leve->add_item(2,"Algo2");
	
	GLUI_Listbox *p2leve=glui_subwin->add_listbox( " " ,(&algo[1]));
	p2leve->add_item(1,"Algo1");
	p2leve->add_item(2,"Algo2");
	
	GLUI_Listbox *p3leve=glui_subwin->add_listbox( " " ,(&algo[2]));
	p3leve->add_item(1,"Algo1");
	p3leve->add_item(2,"Algo2");
	
	GLUI_Listbox *p4leve=glui_subwin->add_listbox( " " ,(&algo[3]));
	p4leve->add_item(1,"Algo1");
	p4leve->add_item(2,"Algo2");
	
	glui_subwin ->add_button( "Cancel",0,mainmenu);
  }

void creategame_done(int no_need){
  char *arr[5];
  arr[0]=(char *)calloc(1,sizeof(char));
  arr[1]=(char *)calloc(sizeof(GLUI_String)+20,sizeof(char));
  arr[2]=(char *)calloc(sizeof(GLUI_String)+20,sizeof(char));
  arr[3]=(char *)calloc(sizeof(GLUI_String)+20,sizeof(char));
  arr[4]=(char *)calloc(sizeof(GLUI_String)+20,sizeof(char));
  string p1="",p2="",p3="",p4="";
  if(p[0]==0)
    p1+="p";
  else if(p[0]==1){
    p1+="c";
    ostringstream sstream;
    sstream << plevel[0];
    p1+=sstream.str();
    
    ostringstream sstream1;
    sstream1 << algo[0];
    p1+=":";
    p1+=sstream.str();
  }
  string name1=pname1;
  if(name1=="")
    p1+="-player1";
  else{
    p1+="-"+name1;
  }

  if(p[1]==0)
    p2+="n";
  else if(p[1]==1){
    p2+="p";
  }
  else if(p[1]==2){
    p2+="c";
    ostringstream sstream;
    sstream << plevel[1];
    p2+=sstream.str();

    ostringstream sstream1;
    sstream1 << algo[1];
    p2+=":";
    p2+=sstream.str();
  }
  else{
    p2+="o";
  }
  
  name1=pname2;
  if(name1=="")
    p2+="-player2";
  else{
    p2+="-"+name1;
  }

  if(p[2]==0)
    p3+="n";
  else if(p[2]==1){
    p3+="p";
  }
  else if(p[2]==2){
    p3+="c";
    ostringstream sstream;
    sstream << plevel[2];
    p3+=sstream.str();

    ostringstream sstream1;
    sstream1 << algo[2];
    p3+=":";
    p3+=sstream.str();

  }
  else{
    p3+="o";
  }
  name1=pname3;
  if(name1=="")
    p3+="-player3";
  else{
    p3+="-"+name1;
  }
  
  if(p[3]==0)
    p4+="n";
  else if(p[3]==1){
    p4+="p";
  }
  else if(p[3]==2){
    p4+="c";
    ostringstream sstream;
    sstream << plevel[3];
    p4+=sstream.str();

    ostringstream sstream1;
    sstream1 << algo[3];
    p4+=":";
    p4+=sstream.str();

  }
  else{
    p4+="o";
  }
  name1=pname4;
  if(name1=="")
    p4+="-player4";
  else{
    p4+="-"+name1;
  }

  arr[1]=&p1[0];
  arr[2]=&p2[0];
  arr[3]=&p3[0];
  arr[4]=&p4[0];
  pthread_create(&nextPositionThread,NULL,readAndCreate,NULL);//to keep an eye on the queue of ballinfos to be processed
  startagame(5,arr);
  GLUI_Master.close_all();
  }
