// Timothy Luciani
// Assignment 3
// Transformer

#ifndef __CS1566_GLOBALS
#define __CS1566_GLOBALS

#include <stdlib.h>
#define _USE_MATH_DEFINES //cause vs is dumb :-(
#include <math.h>
#include <glut/glut.h>
#include "Transformation.h"

#ifndef min //In VC++ 2008, Ryan got a warning about min redefinition, so let's not redefine it if something else already did; sigh
#define min(a,b) ((a) < (b)? a:b)
#endif

#define FALSE 0 
#define TRUE  1

/* define index constants into the colors array */
#define BLACK   0
#define RED     1
#define YELLOW  2
#define MAGENTA 3
#define GREEN   4
#define CYAN    5
#define BLUE    6
#define GREY    7
#define WHITE   8

#define TRANSLATION_MODE 0
#define SCALING_MODE 1
#define ROTATION_MODE 2
#define NONE_MODE 3
#define AXIS_ROTATION_MODE 4
#define REAL_TRANSLATION_MODE 5
#define REAL_SCALING_MODE 6
#define REAL_ROTATION_MODE 7

#define SPIN_ENABLED 1
#define SPIN_DISABLED 0

/*note to cs1566 staff:
 colors and vertices below shd actually be in some .c file*/

GLfloat colors [][3] = {
{0.0, 0.0, 0.0},  /* black   */
{1.0, 0.0, 0.0},  /* red     */
{1.0, 1.0, 0.0},  /* yellow  */
{1.0, 0.0, 1.0},  /* magenta */
{0.0, 1.0, 0.0},  /* green   */
{0.0, 1.0, 1.0},  /* cyan    */
{0.0, 0.0, 1.0},  /* blue    */
{0.5, 0.5, 0.5},  /* 50%grey */
{1.0, 1.0, 1.0}   /* white   */
};

GLfloat vertices_axes[][4] = {
{0.0, 0.0, 0.0, 1.0},  /* origin */ 
{5.0, 0.0, 0.0, 1.0},  /* maxx */ 
{0.0, 5.0, 0.0, 1.0}, /* maxy */ 
{0.0, 0.0, 5.0, 1.0}  /* maxz */ 
};

void glut_setup(void) ;
void gl_setup(void) ;
void my_setup(void);
void my_display(void) ;
void my_mouse(int button, int state, int mousex, int mousey) ;
void my_reshape(int w, int h) ;
void my_keyboard_up( unsigned char key, int x, int y ) ;
void my_keyboard( unsigned char key, int x, int y ) ;
void my_idle(void) ;
void my_TimeOut(int id) ;
void make_cube_smart(double size);
void zoom (int width, int height);
void multiply(GLfloat mat1[4][4], GLfloat vect[3]);
#endif
