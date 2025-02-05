
//
//modified by: Bilkeis Alrowhany 
//date: 1/24/2025
//
//original author: Gordon Griesel
//date:            2025
//purpose:         OpenGL sample program
//
//This program needs some refactoring.
//We will do this in class together.
//+hi
//to-do list
// 1/31/25 add some text
//
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>

using namespace std;
#include <stdio.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

//some structures

class Global {
public:
	int xres, yres;
	Global();
} g;




class X11_wrapper {//when mouse moves, event, check event @ func
private:
	Display *dpy;
	Window win;
	GLXContext glc;
public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent();
	void swapBuffers();
	void reshape_window(int width, int height);
	void check_resize(XEvent *e);
	void check_mouse(XEvent *e);
	int check_keys(XEvent *e);
} x11;

//Function prototypes
void init_opengl(void);
void physics(void);
void render(void);

//Global variables for the box movement since we moved from render to physics... 
static float w = 20.0f;
static float dir_x = 1.5f;  // x dir speed
static float dir_y = 1.5f;  // y dir speed
static float pos[2] = { 0.8f+w, g.yres/2.0f };
static int boxColor[3] = {100, 120, 220}; // Initial box color
static bool boxVisabile = true;






int main()

{
	init_opengl();
	int done = 0;
	//main game loop
	while (!done) {// event loop
		//look for external events such as keyboard, mouse.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
		usleep(200); // if not here pc will use 100% of cup.. here makes diff?
		

	} 
	cleanup_fonts();
	return 0;
}

Global::Global()
{
	xres = 400;
	yres = 200;
	
}

X11_wrapper::~X11_wrapper()
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w = g.xres, h = g.yres;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		cout << "\n\tcannot connect to X server\n" << endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		cout << "\n\tno appropriate visual found\n" << endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 Lab-2 Esc to Exit");
}

bool X11_wrapper::getXPending()
{
	//See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	//Get a pending event.
	XEvent e;
	XNextEvent(dpy, &e);
	return e;
}

void X11_wrapper::swapBuffers()
{
	glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
	//Window has been resized.
	g.xres = width;
	g.yres = height;
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != g.xres || xce.height != g.yres) {
		//Window size did change.
		reshape_window(xce.width, xce.height);
	}
}
//-----------------------------------------------------------------------------

void X11_wrapper::check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	//Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify) {
		//This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed.
			//int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify) {
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves.


		}
	}
}

int X11_wrapper::check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress) {
		switch (key) {
			case XK_a:
				//the 'a' key was pressed
				break;
			case XK_Escape:
				//Escape key was pressed
				return 1;
		}
	}
	return 0;
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	//Set the screen background color
	glClearColor(1.0, 0.65, 0.0, 1.0); // Orange background
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	
}

void physics()
{ //from render to physics > moved to global for access. 
	//static float w = 20.0f;
	//static float dir = 30.0f;
	//static float pos[2] = { 0.0f+w, g.yres/2.0f };

	//No physics yet.
    // Update x position

	if (g.xres < (2*w) || g.yres < (2*w)){
		boxVisabile = false;
		return;
	} else {
		boxVisabile = true; 
	}

    pos[0] += dir_x;
    if (pos[0] >= (g.xres-w)) {
        pos[0] = (g.xres-w);
        dir_x = -dir_x;  // Reverse x direction
		 boxColor[0] = 200;  
        boxColor[1] = 000;  
        boxColor[2] = 50;    
    }
    if (pos[0] <= w) {
        pos[0] = w;
        dir_x = -dir_x; 
		boxColor[0] = 0;    
        boxColor[1] = 050;  
        boxColor[2] = 200;   
    }
	pos[1] += dir_y;
	
	if (pos[1] >= (g.yres-w)) {
        pos[1] = (g.yres-w); //wall
        dir_y = -dir_y;  // Reverse y direction
		// Yellow for y wall
        boxColor[0] = 0;  
        boxColor[1] = 0;  
        boxColor[2] = 155;    

    }
    if (pos[1] <= w) {
        pos[1] = w;
        dir_y = -dir_y;  
		boxColor[0] = 255;  
        boxColor[1] = 0;  
        boxColor[2] = 050;   
    }

	




 
	 
   
}

void render()// always at bottom  //below should be at physics 
{ 
	
	//clear the window
	glClear(GL_COLOR_BUFFER_BIT); // here

	Rect r;
	
	r.bot = g.yres - 20; //chec removed from gl.yres
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "3350 - Lab-2");
	ggprint8b(&r, 16, 0x00ffff00, "n A Esc to Exit" );
	ggprint8b(&r, 16, 0x00ffff00, "n B Speed up");
	if (boxVisabile) {
		
			//draw the box
		glPushMatrix();
		glColor3ub(boxColor[0], boxColor[1], boxColor[2]); // A lighter orange box 
		//glColor3f(1.0f, 0.65f, 0.0f); // Orange box color


		glTranslatef(pos[0], pos[1], 0.0f);
		glBegin(GL_TRIANGLES);
		//float radius = w/1.2; 
		
		glVertex2f(-20.0f, 0.0f);
		glVertex2f(20.0f, 0.0f);
		glVertex2f(0.0f, 40.0f);

		glVertex2f(-20.0f,0.0f);
		glVertex2f(-60.0f,-20.0f);
		glVertex2f(-20.0f,-40.0f);

		glVertex2f(-20.0f,-40.0f);
		glVertex2f(0.0f, -80.0f);
		glVertex2f(20.0f, -40.0f);

		glVertex2f(20.0f, -40.0f);
		glVertex2f(60.0f, -20.0f);
		glVertex2f(20.0f, 0.0f);

		glVertex2f(-20.0f, 0.0f);
		glVertex2f(-20.0f,-40.0f);
		glVertex2f(20.0f, 0.0f);

		glVertex2f(-20.0f,-40.0f);
		glVertex2f(20.0f, -40.0f);
		glVertex2f(20.0f, 0.0f);
			
		glEnd();
		glPopMatrix();
			
	}
	
}






