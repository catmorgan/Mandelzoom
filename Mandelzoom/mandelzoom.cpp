/*-----------------------------------------------------------------------*/
/*  CS-378                  Computer Graphics            Cat Morgan      */
/*-----------------------------------------------------------------------*/
/*                          MandelZOoooooOm                              */
/*-----------------------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>
#include "Complex.h"
#include "Color.h"
#include "Set.h"
#include <vector>

using namespace std;

//width of window
int win_w  = 400;
//height of window
int win_h = 400;
//window x position
int initial_win_x  = 150;
//window y position
int initial_win_y = 50; 
//intial mandelbrot set
double mandel_x1 = -2.0;
double mandel_x2 = 0.5;
double mandel_y1 = -1.25;
double mandel_y2 = 1.25;
//max iterations over mandelbrot set
#define MAX_ITERATIONS 1000
//array of computed colors
int colors [1000][1000];
//if recompute colors boolean
bool reCompute = true;
//vector to hold all the previous mandelbrots
vector <Set> myVecStack;
//pointer for vector
int index = -1;
// Variables for use in rubberbanding.
int xAnchor, yAnchor, xStretch, yStretch;
bool rubberBanding = false;


//Takes a pixel coordinate and transforms it to the complex plane
Complex complexPoint (int u, int v) {
	  double x,y;
	  x = mandel_x1 + (u * ((mandel_x2 - mandel_x1) 
		  / (double) (win_w - 1)));
	  y = mandel_y1 + (v * ((mandel_y2 - mandel_y1)
		  / (double) (win_h - 1)));
	  return Complex (x,y);
  }


//iterates the mandelbrot equation over the complex number
  int countIterations (Complex s) {
	  Complex z = Complex (0,0);
	  int count = 0;
	  for (int i = 0; i < MAX_ITERATIONS; i++) {
		z = z * z + s;
		if ((z.x * z.x) + (z.y * z.y) > 4.0) {
		  break;
	    }
		count++;
	  }
	  return count;
  }
  
//if the complex number is in the mandelbrot set, it'll color it accordingly
 Color chooseColor (int i) {

	if (i==MAX_ITERATIONS) {
	  return Color (0.0, 0.0, 0.0);
	}
	//blue
	else if (10 < i && i < 400) {
	  return Color (0.1, 0.0, 1.0);
	}
	//pink
	else if ( 0 < i && i < 10) {
	  return Color (0.9, 0.9, 0.9);
	}
	else if (400 < i && i < 550) {
	  return Color (1.0, 0.00, 0.5);
	}
	else {
	  return Color (0.7, 1.0, 1.0);
	}
 }

 //takes the pixel coordinate and color to color the pixel
void displayPixel (int u, int v, Color c) {

	float r = c.red;
	float g = c.green;
	float b = c.blue;

	glColor3f(r, g, b);

	glBegin( GL_POINTS );
    glVertex2i(u, v);
    glEnd( );
  }

//computes and draws the mandelbrot set for every pixel in the window
void drawMandel() {
	  
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();

  for (int u =0; u < win_w; u++) {
	  for (int v = 0; v < win_h; v++) {
		  //will take a pixel and make it a complex number
		  Complex s = complexPoint (u, v);
		  //will iterate over the complex number using the mandelbrot equation
		  int i;
		  if (reCompute) {
		  i = countIterations (s);
		   //add the color at the pixel coordinate
		  colors[u][v] = i;
		  } else { 
			  i = colors [u] [v];
		  }
		  //if i < 1000; not in mandelbrot set
		  Color c = chooseColor (i);
		  //using openGL display the pixels 
		  displayPixel(u, v, c);
	  }
   }
  reCompute = false;
  glFlush();
}

//pushes a mandelbrot computation onto a vector stack
void Push () {
	//zoom in
	if (index == 0) {
   Set i(mandel_x1, mandel_x2, mandel_y1, mandel_y2);
   myVecStack.push_back(i);
	index++;
	} if (index > 0) {
	Set view = myVecStack.at(index++);
    mandel_x1 = view.mandel_x1;
	mandel_x2 = view.mandel_x2;
	mandel_y1 = view.mandel_y1;
    mandel_y2 = view.mandel_y2;
	index++;
	reCompute = true;
	}
	glutPostRedisplay();
}

//pops a mandelbrot computation onto a vector stack 
void Pop () {
	//zoom out
	if (index == 0) {
		return;
	} if (index > 0) {
	Set view = myVecStack.at(index-1);
    mandel_x1 = view.mandel_x1;
	mandel_x2 = view.mandel_x2;
	mandel_y1 = view.mandel_y1;
    mandel_y2 = view.mandel_y2;
	index--;
	reCompute = true;
	}
		glutPostRedisplay();
}

//draws a rectangle
  void drawRect(int xOld, int yOld, int xNew, int yNew )
{
  glColor3f(1.0, 1.0, 1.0);

  glBegin( GL_LINE_LOOP );
  glVertex2i( xOld, yOld ); glVertex2i (xNew, yOld);
  glVertex2i( xNew, yNew ); glVertex2i (xOld, yNew); 
  glEnd();
  glFlush();
}

  //draws a rubberband
  void drawRubberBand(int xA, int yA, int xS, int yS)
{
	glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glBegin( GL_LINE_LOOP );
    glVertex2i( xA, yA ); glVertex2i(xS, yA);
    glVertex2i( xS, yS ); glVertex2i(xA, yS);
    glEnd();
    glDisable(GL_COLOR_LOGIC_OP);
    glFlush();
}



//rubberbanding is called and drawn
 void rubberBand( int x, int y )
// Callback for processing mouse motion.
{
  if ( rubberBanding )
  {
    drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    y = win_h - y;
    xStretch = x;
    yStretch = y;
    drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    glFlush();
  }
}
 
 //reshapes the window
void reshape(int w, int h)
// Callback for processing reshape events.
{
  glViewport(0,0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, w, 0.0, h);
}

//exits the window
void escExit( GLubyte key, int, int )
// Callback for processing keyboard events.
{ if ( key == 27 /* ESC */ ) std::exit( 0 ); }

//callsback the menu
 void mainMenu( int item )
  {
    switch ( item )
    {
      case 1: Push(); break;
      case 2: Pop(); break;
      case 3: std::exit( 0 );
    }
  }


 //Sets the menu items
  void setMenus()
  {
    glutCreateMenu( mainMenu );
    glutAddMenuEntry( "Push", 1 );
    glutAddMenuEntry( "Pop", 2 );
	glutAddMenuEntry( "Exit", 3 );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
  }

  //if the left mouse button is released
  void processLeftUp( int x, int y )
  {
    if (rubberBanding)
    {
	  reCompute = true;
      int xNew, yNew;
      drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
      rubberBanding = false;
      xNew = x;
      yNew = win_h - y;
      drawRubberBand(xAnchor, yAnchor, xNew, yNew);
	  Complex newX1Y1 = complexPoint(xAnchor, yAnchor);
	  Complex newX2Y2 = complexPoint(xNew, yNew);
	  //used to prevent distortion of the image when zooming in/out
	  double xd = newX2Y2.x - newX1Y1.x;
	  double yd = newX2Y2.y - newX1Y1.y;
	  double Ar = yd / xd;
	  double xs = mandel_x2 - mandel_x1;
	  double ys = mandel_y2 - mandel_y1;
	  double Aw = ys / xs;
	  
	  if (Ar > Aw) {
	  double i = (((Ar / Aw) - 1) / 2) * xd;
	  mandel_x1 = newX1Y1.x - i;
	  mandel_x2 = newX2Y2.x + i;
	  mandel_y1 = newX1Y1.y;
	  mandel_y2 = newX2Y2.y;
      }

	  if (Ar < Aw) {
	  double i = (((Aw / Ar) - 1) / 2) * yd;
	  mandel_x1 = newX1Y1.x;
	  mandel_x2 = newX2Y2.x;
	  mandel_y1 = newX1Y1.y - i;
	  mandel_y2 = newX2Y2.y + i;
      }

	 if (Ar == Aw) {
	  mandel_x1 = newX1Y1.x;
	  mandel_x2 = newX2Y2.x;
	  mandel_y1 = newX1Y1.y;
	  mandel_y2 = newX2Y2.y;
	  }

	  //make a struct out of the new mandbrot set 
	  Set i(mandel_x1, mandel_x2, mandel_y1, mandel_y2);
	  //add element to end
	  index++;
	  myVecStack.push_back(i);
	  glutPostRedisplay();
      glFlush();
    }
  }

  //if the left mouse button is pressed down
 void processLeftDown( int x, int y )
  {
    if (!rubberBanding)
    {
      int xNew = x;
      int yNew = win_h - y;
      xAnchor = xNew;
      yAnchor = yNew;
      xStretch = xNew;
      yStretch = yNew;
      drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
      rubberBanding = true;
    }
  }


 //mouse function that takes in the state of the mouse and coordinate
  void mouse( int button, int state, int x, int y )
  {
    if ( button == GLUT_LEFT_BUTTON )
      switch ( state )
      {
        case GLUT_DOWN: processLeftDown( x, y ); break;
        case GLUT_UP: processLeftUp( x, y ); break;
      }
  }


int main(int argc, char* argv[])
{
	mandel_x1 = atof(argv[1]);
	mandel_x2 = atof(argv[2]);
    mandel_y1 = atof(argv[3]);
	mandel_y2 = atof(argv[4]);
    win_w = atoi(argv[5]);
    win_h = atoi(argv[6]);
	Set initial(mandel_x1, mandel_x2, mandel_y1, mandel_y2);
	myVecStack.push_back(initial);
	index = 0;

   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

  // Choose RGB display mode for normal screen window.
  glutInitDisplayMode(GLUT_RGB);

  // Set initial window size, position, and title.
  glutInitWindowSize(win_w, win_h);
  glutInitWindowPosition(initial_win_x, initial_win_y);
  glutCreateWindow("MandelZooooooom");

  // You don't (yet) want to know what this does.
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluOrtho2D( 0.0, (double) win_w, 0.0, (double) win_h);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

  // This is a hack.
  glTranslatef( 0.375, 0.375, 0.0 );

  // Set the color for clearing the normal screen window.
  glClearColor( 1.0, 1.0, 1.0, 0.0 );

  // Set the callbacks for the normal screen window.
  glutDisplayFunc(drawMandel);
  glutMouseFunc( mouse );
  glutMotionFunc( rubberBand );
  glutPassiveMotionFunc( rubberBand );


  // Set the callback for reshape events.
  glutReshapeFunc(reshape);

  // Set the callback for keyboard events.
  glutKeyboardFunc( escExit );

  // Set up the menus.
  setMenus();

  // Start the GLUT main loop.
  glutMainLoop( );

}