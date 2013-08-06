/*#include "enums.hh"

// tensor files
#include <time.h>
#include <fstream>

#include "lib/std/FatalError.hh"
#include "lib/streamlines/Streamline.hh"
#include "lib/projector/Projector.hh"
//#include "lib/volume/VolumeRenderer.hh"
#include "lib/std/AllocArray.hh"
#include "lib/glyphs/VectorFieldRenderer.hh"
#include "lib/file_ops/Parameters.hh"
#include "lib/vector/Vector.hh"
#include "lib/vector/point.hh"
#include "lib/open_gl.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>*/

#include "callbacks.hh"

typedef unsigned int uint;
typedef unsigned char uchar;

#define MAX_EPSILON_ERROR 5.00f
#define THRESHOLD         0.30f

extern void copyInvViewMatrix(float *invViewMatrix, size_t sizeofMatrix);
extern void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, /*uint *d_cluster, */float* d_iRed, float* d_oRed, 
						float* d_iGreen, float* d_oGreen, float* d_iBlue, float* d_oBlue, float4* d_iColors, unsigned short* data, 
						/*unsigned short* cluster_data, */uint imageW, uint imageH, float density, float brightness, 
						float4 one, float4 two, float4 three, float4 four, float4 five, float4 six, uint tfsize, 
						void *h_volume, /*void *cluster, */cudaExtent volumeSize, cudaArray *d_volumeArray, /*cudaArray *d_volumeArray_cluster, */int *set);


extern GLuint compileShader(const char *vsource, const char *fsource);
extern void draw_volume(GLuint shader, GLuint tex, GLuint pos);

GLuint volumeShader = 0;
GLuint volTexture = 0;

// Define the files that are to be save and the reference images for validation
const char *sOriginal[] =
{
    "volume.ppm",
    NULL
};

const char *sReference[] =
{
    "ref_volume.ppm",
    NULL
};

const char *sSDKsample = "Tensor Volume";

//char *volumeFilename = "../data/divergence.raw";
//char *volumeFilename = "pressure.raw";
//char *cluster_file = "../data/ds2_clust3_ushort.raw";

cudaExtent volumeSize = make_cudaExtent(194,193,194);
//typedef unsigned short VolumeType;
//typedef float VolumeType;

dim3 blockSize(16, 16);
dim3 gridSize;

float3 viewRotation;
float3 viewTranslation = make_float3(0.0, 0.0, -4.0f);
float invViewMatrix[12];

float density = 0.05f;
float brightness = 1.0f;

GLuint pbo = 0;     // OpenGL pixel buffer object
GLuint tex = 0;     // OpenGL texture object

//GLuint pbo_cluster = 0;     // OpenGL pixel buffer object
//GLuint tex_cluster = 0;     // OpenGL texture object

struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)
//struct cudaGraphicsResource *cluster_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

unsigned int timer = 0;

bool r_flag = false;

// Auto-Verification Code
const int frameCheckNumber = 2;
int fpsCount = 0;        // FPS count for averaging
int fpsLimit = 1;        // FPS limit for sampling
int g_Index = 0;
unsigned int frameCount = 0;
unsigned int g_TotalErrors = 0;
bool g_Verify = false;
bool g_bQAReadback = false;
bool g_bQAGLVerify = false;
bool g_bFBODisplay = false;

int mode = 0;
bool method = false;

bool volumeVisible = false;

int tfsize;
const float** tf;
//float4* tf_host;
//cudaArray* tf_device;
float4 transfer[10];
/*float4 transfer[]  = {
			make_float4(0.0f,0.0f,0.0f,0.1f),
			make_float4(0.0, 0.3, 0.3, 0.1),
			make_float4(0.5, 0.5, 1.0, 0.1),
			make_float4(1.0, 1.0, 1.0, 0.1),
			make_float4(1.0, 0.2, 0.2, 0.1),
			make_float4(1.0, 0.0, 0.0, 0.1), 
};*/

// CheckFBO/BackBuffer class objects
CheckRender       *g_CheckRender = NULL;
//CheckRender       *g_CheckCluster = NULL;

int *pArgc;
char **pArgv;

cudaArray *d_volumeArray = 0;
//cudaArray *d_volumeArray_cluster = 0;

cudaArray *d_transferFuncArray;

float *d_oRed, *d_iRed;
float *d_oBlue, *d_iBlue;
float *d_oGreen, *d_iGreen;
ushort *data/*, *cluster_data*/;
void *h_volume/*, *cluster*/;

float4 *d_iColors;

int ox, oy;
int buttonState = 0;

extern GLuint shader;

int set[2];
//int window1, window2;
#define MAX(a,b) ((a > b) ? a : b)

void initPixelBuffer();

extern void DrawAxisAndBox(double, double, double);
extern unsigned int window_width; 
extern unsigned int window_height;
extern float mag;

static float zoom = 3.0;
static float radius = 0.1;	

static bool shift = false;

GLfloat vertices_axes[][4] = {
	{0.0, 0.0, 0.0, 1.0},  /* origin */ 
	{5.0, 0.0, 0.0, 1.0},  /* maxx */ 
	{0.0, 5.0, 0.0, 1.0}, /* maxy */ 
	{0.0, 0.0, 5.0, 1.0}  /* maxz */ 

};
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

void Magnify();

using namespace std;

InteractionMode CurrIM;

int buttonDownX, buttonDownY;
int LeftButtonDown, RightButtonDown ;
GLfloat anglex, angley, anglez ;

SelectedObjectCatagory ObjectCatagory ;
int iObjectName ;

int iSeedpointMoveDir = -1 ;			// 0 for x, 1 for y and 2 for z

bool magnifying = false;
GLfloat positionX = 0.0;
GLfloat positionY = 0.0;

Streamline				streamlines ;				//streamlines object.
Projector				projector;
//VolumeRenderer			volRenderer ;				//volume renderer
VectorFieldRenderer		vectorField ;				//vector field renderer
Vector 					***velocityField;			//vectorfield data
Vector 					***eigenValues; 
Vector 					***eigenVectors; 

GLfloat 				*eigenfloats; 				// eigen values in float form

int slices = 0;
int rows = 0;
int cols = 0;

void draw_axes( void ) {
  glLineWidth( 5.0 );
  glBegin(GL_LINES); 
  {
    glColor3fv(colors[1]);
    glVertex4fv(vertices_axes[0]);
    glVertex4fv(vertices_axes[1]);
		
    glColor3fv(colors[4]);
    glVertex4fv(vertices_axes[0]);
    glVertex4fv(vertices_axes[2]);
		
    glColor3fv(colors[6]);
    glVertex4fv(vertices_axes[0]);
    glVertex4fv(vertices_axes[3]);
  }
  glEnd();
  glLineWidth( 1.0 );
	
}

void keyboard_volume(unsigned char key, int x, int y)
{
    switch(key) {
        case 27:
	    shrQAFinishExit2(false, *pArgc, (const char **)pArgv, QA_PASSED);
            break;
  
        case '+':
            density += 0.01f;
            break;
        case '-':
            density -= 0.01f;
            break;

        case ']':
            brightness += 0.1f;
            break;
        case '[':
            brightness -= 0.1f;
            break;

        default:
            break;
    }
	
//	int currWin = glutGetWindow();
//	glutSetWindow(window1);
//  glutPostRedisplay();
//	glutSetWindow(window2);
//	glutPostRedisplay();
//	glutSetWindow(currWin);
}

// =======================
// = Keyboard Operations =
// =======================

// The keyboard callback
void keyboard(unsigned char key, int x, int y)
{
keyboard_volume(key, x, y);
   switch (key) {
		case 'e':				// explore mode
		case 'E':				// move, rotate, zoom
			printf("Entering EXPLORE_MODE.\n") ;
			CurrIM = ExploreMode ;
			break;
		case 's': // select mode
		case 'S':
			printf("Entering SELECT_MODE.\n") ;
			CurrIM = SelectMode ;
			break ;
		case '1':														// enable/disable volume rendering
			if (volumeVisible) 
				volumeVisible = false ;
			else
				volumeVisible = true ;
			glutPostRedisplay() ;			
			break ;
		case '2':													// enable/disable vector field view			
			if( vectorField.bVisible )
				vectorField.bVisible = false ;
			else
				vectorField.bVisible = true ;
			glutPostRedisplay() ;			
			break ;								  
		case '3':													// enable/disable streamlines
			if(streamlines.bVisible)
				streamlines.bVisible  = false ;		
			else
				streamlines.bVisible  = true ;
			glutPostRedisplay() ;			
			break ;
		case 'x':  // toggle grid x
		case 'X':
			if( CurrIM == SelectMode && ObjectCatagory == SEEDPOINT ){
				iSeedpointMoveDir = 0 ;	
			}else{
				vectorField.ToggleSliceVisibilityX();
			}
			glutPostRedisplay();			
			break ;
		case 'y':	// toggle grid y
		case 'Y':
			if( CurrIM == SelectMode && ObjectCatagory == SEEDPOINT ){
				iSeedpointMoveDir = 1 ;	
			}else{
				vectorField.ToggleSliceVisibilityY();
			}
			glutPostRedisplay();
			break ;
		case 'z':		// toggle grid z
		case 'Z':
			if( CurrIM == SelectMode && ObjectCatagory == SEEDPOINT ){
				iSeedpointMoveDir = 2 ;	
			}
			
			else{
				vectorField.ToggleSliceVisibilityZ();
			}
			
			glutPostRedisplay();
			break ;
		case 'm': 		// enable mag glass
		case 'M':
			magnifying = !magnifying;
			glutPostRedisplay();
			break;
		case 'l':		// increase radius
		case 'L':
			radius += 0.01;
			glutPostRedisplay();
			break;
		case 'k':			// decrease radius
		case 'K':
			radius -= 0.01;
			glutPostRedisplay();
			break;
		case '+':  			// zoom in
			if(magnifying){ 
				zoom += 0.5;
			}	
			glutPostRedisplay() ;		
			break ;
		case '-':	// zoom out
			
			if(magnifying){
				zoom -= 0.5;
				if(zoom < 1.0)
					zoom = 1.0;
			}	
			glutPostRedisplay() ;		
			break ;
		
		case '0':
			if(shift){
				vectorField.IncreaseSliceNumber();
				glutPostRedisplay();
			}
		break;
		case '9':
			if(shift){
				vectorField.DecreaseSliceNumber();
			}
		glutPostRedisplay();
		break;
		
		case 27:
			exit(0);
			break ;
			
		case 't':
		case 'T':
			shift = !shift;
			glutPostRedisplay();
			break;
   }

}

// ====================
// = Mouse Operations =
// ====================

void processHits (GLint hits, GLuint buffer[])
{
		
   unsigned int i, j;
   GLuint names, *ptr;

   float fMinMin = 10.0f, fMaxMin= 10.0f ;		// since the z value should be in the range 0 to 1
   int iMinZWinner=-1, iMaxZWinner=-1 ;
   SelectedObjectCatagory ObjCat = NO_OBJECT ;

   //printf ("hits = %d\n", hits);
   ptr = (GLuint *) buffer;

   for (i = 0; i < (unsigned)hits; i++) {	/*  for each hit  */
      names = *ptr;
      //printf(" number of names for hit = %d\n", names); 
	  ptr++;
	  if( names != 2 ){
		printf("We have problem here.\n") ;
		exit(1) ;
	  }
	  float z1 = (float) *ptr/0x7fffffff ; ptr++;
	  float z2 = (float) *ptr/0x7fffffff ; ptr++;
	  int iIndex ;

      //printf(" z1 is %g\n", z1 ); 
      //printf(" z2 is %g\n", z2 ) ; 
		  	
	  int oc = *ptr ; ptr++ ;
	  iIndex = *ptr ; ptr++ ;

	  if( oc == SEEDPOINT ){		// SEEDPOINT HAS HIGHER PRIORITY
		  ObjCat = (SelectedObjectCatagory)oc ;
		  iMinZWinner = iIndex ;
		  printf("Seedpoint selected.\n") ;
	  }								//if( oc == SEEDPOINT )
	  else{
		  if( z1 < fMinMin ){
			fMinMin = z1 ;
			iMinZWinner = iIndex ;
			ObjCat = (SelectedObjectCatagory)oc ;
		  }
		  if( z2 < fMaxMin){
			fMaxMin = z2;
			iMaxZWinner = iIndex ;
		  }
	  }//else
   } //for (i = 0; i < (unsigned)hits; i++) 

	ObjectCatagory =  ObjCat;
	iObjectName  = iMinZWinner ;
	printf("Catagory = %d and ObjecSelected = %d\n",ObjCat, iMinZWinner) ;
}

// selects an object at location X,Y in screen coordinate
// single object selection. Catagory of the object will be set in oc. Right now we have just one catagory. STREAMLINE
// and name of the object will be set in on
#define BUFSIZE					512			// buffer to process mouse click hits
void SelectObjectAtXY(int x, int y){
	GLint hits;
	GLint viewport[4];
	glGetIntegerv (GL_VIEWPORT, viewport);
	GLuint selectBuf[BUFSIZE];

	glSelectBuffer (BUFSIZE, selectBuf);
	(void) glRenderMode (GL_SELECT);

	glInitNames();									// initializing the name stack

	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
			/*  create 5x5 pixel picking region near cursor location	*/
	gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y), 
				  5.0, 5.0, viewport);
			
	glMatrixMode (GL_MODELVIEW);
	glPushName(STREAMLINE);							// signature of a streamline		
	streamlines.SelectModeStreamlinesDraw() ; 
	glPopName() ;									// pop streamline signature

	glPushName(SEEDPOINT);							// signature of a seedpoint		
	streamlines.SelectModeSeedPointDraw() ; 
	glPopName() ;									// pop seedpoint signature

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	//glFlush ();

	hits = glRenderMode (GL_RENDER);
	if( hits > 0 )		
		processHits (hits, selectBuf);	
	else{		// no hit
		ObjectCatagory = NO_OBJECT ;
	}
}

void reshape(int w, int h) {
		
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
//glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
	float left = -1.0 * w/2 * projector.fDetectorPixelSize;
	float right = 1.0 * w/2 * projector.fDetectorPixelSize;
	float bottom = -1.0 * h/2 * projector.fDetectorPixelSize;
	float top = 1.0 * h/2 * projector.fDetectorPixelSize;
	float near = projector.fSourceToDetector;
	float far = 15000;
	//printf("left = %f\n", left); fflush(stdout);
	//printf("right = %f\n", right); fflush(stdout);
	//printf("bottom = %f\n", bottom); fflush(stdout);
	//printf("top = %f\n", top); fflush(stdout);
	//printf("near = %f\n", near); fflush(stdout);
	//printf("far = %f\n", far); fflush(stdout);
//glFrustum(-1, 1, -1, 1, -1, 1);
glFrustum(left, right, bottom, top, near, far);
	/*glFrustum (-1.0 * w/2 * fProjectionPlanePixelSize,
				1.0	* w/2 * fProjectionPlanePixelSize,
				-1.0 * h/2 * fProjectionPlanePixelSize,
				1.0 * h/2 * fProjectionPlanePixelSize, 
				projector.fSourceToDetector, 15000);
*/
	glMatrixMode (GL_MODELVIEW);
}

/*void reshape_volume(int w, int h)
{
    window_width = w; window_height = h;
    initPixelBuffer();

    // calculate new grid size
    gridSize = dim3(iDivUp(window_width, blockSize.x), iDivUp(window_height, blockSize.y));

    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}*/

void MouseMoveToRotate(int diffX, int diffY){
	// map mousemotion into degrees
	//normalize mouse motion
	float yNormalizer = 360*3/window_width  ;  // rotate 360/3 = 120 degree if your mouse drag covers half of the screen
	float xNormalizer = 360*3/window_height ; // rotate 120 degree if your mouse drag covers half of the screen

	angley += diffX/yNormalizer ;
	anglex += diffY/xNormalizer ;
	
	//volRenderer.RotateObject(anglex, angley, anglez ) ;
	//volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity 
	
	glutPostRedisplay() ;
}

void MouseMoveToSeedpointMove(float diffX, float diffY){
	fPoint fsp;
	streamlines.GetSeedPoint(fsp) ;
	//printf("%f\n",diffY) ;
	switch(iSeedpointMoveDir){
		case 0:			//along x axis
			fsp.x -= (diffY*2) ;
			break ;
		case 1:
			fsp.y -= (diffY*2) ;	
			break ;
		case 2:
			fsp.z -= (diffY*2) ;
			break ;
	}
	streamlines.MoveSeedPoint(fsp) ; 
} 

void MouseMoveToZoom(int diffX, int diffY){

	projector.ChangeLocation(diffY) ; 
	//volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity 
		
	glutPostRedisplay() ;
}

void motion_volume(int x, int y)
{
    float dx, dy;
    dx = (float)(x - ox);
    dy = (float)(y - oy);

    if (buttonState == 4) {
        // right = zoom
        viewTranslation.z += dy / 100.0f;
    } 
    else if (buttonState == 2) {
        // middle = translate
        viewTranslation.x += dx / 100.0f;
        viewTranslation.y -= dy / 100.0f;
    }
    else if (buttonState == 1) {
        // left = rotate
        viewRotation.x += dy / 5.0f;
        viewRotation.y += dx / 5.0f;
    }

    ox = x; oy = y;
//	int currWin = glutGetWindow();
//	glutSetWindow(window1);
    glutPostRedisplay();
//	glutSetWindow(window2);
//	glutPostRedisplay();
//	glutSetWindow(currWin);
}

// mouse motion callback
void motion(int x, int y){	

motion_volume(x, y);
	if( CurrIM == ExploreMode){
		if(LeftButtonDown == 1 ){				// rotate the volume
			MouseMoveToRotate((x-buttonDownX)/2, (y-buttonDownY)/2) ;
			buttonDownX = x ;
			buttonDownY = y ;
		}
		else if( RightButtonDown == 1){			// move the camera, zoom in or out
			MouseMoveToZoom(x-buttonDownX, y-buttonDownY) ;
			buttonDownX = x ;
			buttonDownY = y ;	
		}
	} //if( InteractionMode == ExploreMode)
	else if( CurrIM == SelectMode){
		if(LeftButtonDown == 1 ){				// temporary streamline selection or seedpoint move
			if( streamlines.bSeedpointSelected && iSeedpointMoveDir != -1 ){// seedpoint was already selected
				MouseMoveToSeedpointMove(x-buttonDownX, y-buttonDownY) ;
				buttonDownX = x ;
				buttonDownY = y ;
				glutPostRedisplay() ;
			}
			else{
				SelectObjectAtXY(x, y);
				switch(ObjectCatagory){
					case STREAMLINE:
						//printf("ObjectName %d\n",iObjectName) ;
						streamlines.bSeedpointSelected = false ;
						streamlines.TemporarySelectedStreamline(iObjectName) ;
						glutPostRedisplay() ;
						break ;
					case SEEDPOINT:
						if(!streamlines.bSeedpointSelected){		// user just moved in to seed point
							buttonDownX = x ;
							buttonDownY = y ;
							streamlines.bSeedpointSelected = true ;
							glutPostRedisplay() ;
						}
						break ;
					case NO_OBJECT:
						streamlines.TempSelectedStreamline = -1 ;
						streamlines.bSeedpointSelected = false ;
						glutPostRedisplay() ;
						break ;
					default:
						printf("Unidentified object.\n") ;
						break ;
				}  //switch(iObjectCatagory){	
			} // else
		} //if(LeftButtonDown == 1 ){			
	} //else if( CurrIM == SelectMode){
}

void mouse_volume(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
        buttonState  |= 1<<button;
    else if (state == GLUT_UP)
        buttonState = 0;

    ox = x; oy = y;
//	int currWin = glutGetWindow();
//	glutSetWindow(window1);
    glutPostRedisplay();
//	glutSetWindow(window2);
//	glutPostRedisplay();
//	glutSetWindow(currWin);
}

// the mouse callback
void mouse(int button, int state, int x ,int y){	

mouse_volume(button, state, x, y);
	if( CurrIM == ExploreMode ){				// xplore mode mouse interaction
		if (state == GLUT_DOWN ) { 
			 switch (button) {
			 case GLUT_LEFT_BUTTON:
				  LeftButtonDown = 1 ;
				  buttonDownX = x ;
				  buttonDownY = y ;
				  break;
			 case GLUT_MIDDLE_BUTTON:
				  break;
			 case GLUT_RIGHT_BUTTON:
				  RightButtonDown = 1 ;
				  buttonDownX = x ;
				  buttonDownY = y ;
				  break;
			 }
		} //if (state == GLUT_DOWN ) 
		else if( state == GLUT_UP ){		
			LeftButtonDown = RightButtonDown = 0 ;
			//volRenderer.SetProjectionResolution(false, false) ; 
			glutPostRedisplay() ;
		}
    } //if( CurrIM == ExploreMode )
	else if(CurrIM == SelectMode ){		
		if (state == GLUT_DOWN ) { 
			switch (button) {
				case GLUT_LEFT_BUTTON:
					LeftButtonDown = 1 ;
					SelectObjectAtXY(x, y);
					switch(ObjectCatagory){
						case STREAMLINE:
							streamlines.TemporarySelectedStreamline(iObjectName);
							glutPostRedisplay() ;
							break ;
						case SEEDPOINT:
							buttonDownX = x ;
							buttonDownY = y ;
							streamlines.bSeedpointSelected = true ;
							glutPostRedisplay() ;
							break ;
						case NO_OBJECT:
							break;
						default:
							printf("Unidentified object.\n") ;							
							break ;
					}//switch(iObjectCatagory){  			
				break;
			}//switch (button) {
		} //if (state == GLUT_DOWN ) 
		else if( state == GLUT_UP ){		
			LeftButtonDown = RightButtonDown = 0 ;						
			//clear if any streamline was temporarily selected
			streamlines.TempSelectedStreamline = -1 ;
			switch(ObjectCatagory){				
				case STREAMLINE:					
					streamlines.UpdateStreamlineStatus(iObjectName) ;					
					glutPostRedisplay() ;
					break ;
				case SEEDPOINT:
					if( streamlines.bSeedpointMoved )
						streamlines.UpdateSeedPoint() ;
					streamlines.bSeedpointMoved = false ;
					streamlines.bSeedpointSelected = false ;
					ObjectCatagory = NO_OBJECT ;
					iSeedpointMoveDir = -1 ;
					glutPostRedisplay() ;
					break ;
				case NO_OBJECT:
					break ;
			}//switch(ObjectCatagory){  			
		}//else if( state == GLUT_UP )
	} //else if(CurrIM == SelectMode )
}

void Magnify(){
				
	// go back to 2D first	
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();	
	
//	glPushMatrix();
//	glTranslatef(radius, 0.0, 0.0);
	
	glBegin(GL_POINTS); // draw circle around area
	
	glColor3f(1.0, 1.0, 1.0); // white
	
    for(int i = 0; i <= 360; i++) { // increment thru the degrees
		
        glVertex2f( (float)(positionX/window_width) + radius * cos(i * M_PI/180.0) ,
                   ( (float)(positionY/window_height) ) + radius * sin(i * M_PI/180.0) );
    }
	
    glEnd();
	
//	glPopMatrix();
	
	 glClearStencil(0); // clear stencil
	 	  
	 glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); // disable drawing
	 glEnable(GL_STENCIL_TEST); // endable stencil test
	 	 
	 glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF); // always past test, replace with 1
	 glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // always replace
	 		
	// stencil magnification area
			
	glBegin(GL_TRIANGLE_FAN);

    glVertex2f( (float)(positionX/window_width), (float)(positionY/window_height) ); // origin
	
    for(int i = 0; i <= 360; i++) { // increment thru the degrees
		
        glVertex2f( (float)(positionX/window_width) + radius * cos(i * M_PI/180.0) ,
                   ( (float)(positionY/window_height) ) + radius * sin(i * M_PI/180.0) );
    }
	
    glEnd();
	
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); // enable drawing
	//  
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF); // replace everything within stencil
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // keep attributes of drawing
	
	// put back to normal
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);		

	if(vectorField.bVisible){
		
		if(shift){ // grid
			vectorField.setZoom(zoom);
		
		}else{	// along streamlines
		
			vectorField.setZoom(zoom, mag, false);
			vectorField.drawZoomedQuadric();
		} // end else
	} // end if visible
		
	glDisable(GL_STENCIL_TEST); // disable stencil test
	
}

// passive motion callback
void mouseMotion(int x, int y){
	
	if(!magnifying)
		return;
		
	/* mouse coordinate */	
	positionX = x;
	positionY = window_height - y;
	
	glutPostRedisplay();	
}

void draw_volume()
{
    // display results
//    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	gluLookAt(0.0, 0.0, -10, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // draw using texture
	// going into 2D render mode (maybe replace with shader later?)
 	glEnable(GL_TEXTURE_2D);
	//glUseProgram(volumeShader);
    
// copy from pbo to texture

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	
//	draw_volume(volumeShader, tex, volTexture);
	
    // draw textured quad
	// texture to 4 window coordinates (between -1 and 1)
     glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
         glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
         glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
         glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
     glEnd();
	
	// binding null (clearing the texture buffer)
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (g_CheckRender && g_CheckRender->IsQAReadback() && g_Verify) {
        // readback for QA testing
        shrLog("\n> (Frame %d) Readback BackBuffer\n", frameCount);
        g_CheckRender->readback( window_width, window_height );
        g_CheckRender->savePPM(sOriginal[g_Index], true, NULL);
        if (!g_CheckRender->PPMvsPPM(sOriginal[g_Index], sReference[g_Index], MAX_EPSILON_ERROR, THRESHOLD)) {
            g_TotalErrors++;
        }
        g_Verify = false;
    }

	//glutSwapBuffers() ;
	glutReportErrors();
	
 	glDisable(GL_TEXTURE_2D);

	// put back to normal
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

// ====================
// = Display Callback =
// ====================

/// The display callback.
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity ();             /* clear the matrix */

	if(volumeVisible)
		draw_volume();

/* viewing transformation  */
	//projector.printProjector();
	//printf("projector.vSourceLocation.VectorZ = %f\n", projector.vSourceLocation.VectorZ); fflush(stdout);
	gluLookAt(0.0, 0.0, projector.vSourceLocation.VectorZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	//volRenderer.RenderVolImage() ;			// render the volume image 
	//draw_volume();
	glClear(GL_DEPTH_BUFFER_BIT) ;			// so that this volume image does not block other objects
											// Since we have enabled depth test	
	glRotatef(anglex,1.0,0.0,0.0) ;
	glRotatef(angley,0.0,1.0,0.0) ;
	glRotatef(anglez,0.0,0.0,1.0) ;

	double boxLenX = Parameters::CtFileVoxelSpacing->VectorX ;
	double boxLenY = Parameters::CtFileVoxelSpacing->VectorY ;
	double boxLenZ = Parameters::CtFileVoxelSpacing->VectorZ ;

	DrawAxisAndBox(boxLenX, boxLenY, boxLenZ) ;						// draw axis and bounding box

	// get the volume center in the coordinate center
	float tz = (slices)*boxLenZ/2 ;
	float ty = (rows)*boxLenY/2 ;
	float tx = (cols)*boxLenX/2 ;		
	
	//printf("boxLenX = %f\n", boxLenX); fflush(stdout);
	//printf("boxLenY = %f\n", boxLenY); fflush(stdout);
	//printf("boxLenZ = %f\n", boxLenZ); fflush(stdout);
	//printf("tx = %f\n", tx); fflush(stdout);
	//printf("ty = %f\n", ty); fflush(stdout);
	//printf("tz = %f\n", tz); fflush(stdout);

	glTranslatef(-tx, -ty, -tz) ;
	
	streamlines.RenderStreamlines(eigenfloats) ;		// display streamlines
	
	if(vectorField.bVisible){
		if(shift){
			if(!vectorField.rendered){
				printf("render\n");
				vectorField.RenderSlice();
			}
			vectorField.drawQuadric(shift) ;
		}else
			vectorField.drawQuadric(shift) ;	// draw vector field
	}
	
	if(magnifying){
		Magnify();
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else
		glutSetCursor(GLUT_CURSOR_TOP_LEFT_CORNER);

//	if(volumeVisible)
//		draw_volume();
	
	glPopMatrix();
	
	glutSwapBuffers() ;
}

// render image using CUDA
void render()
{		
	copyInvViewMatrix(invViewMatrix, sizeof(float4)*3);

    // map PBO to get CUDA device pointer
    uint *d_output/*, *d_cluster*/;
	// map PBO to get CUDA device pointer
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_pbo_resource, 0));
//    cutilSafeCall(cudaGraphicsMapResources(1, &cluster_pbo_resource, 0));
	
	size_t num_bytes; 
    cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void **)&d_output, &num_bytes,  
						       cuda_pbo_resource));
//	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void **)&d_cluster, &num_bytes, 
//						       cluster_pbo_resource));					
    //printf("CUDA mapped PBO: May access %ld bytes\n", num_bytes);

    // clear image
    cutilSafeCall(cudaMemset(d_output, 0, window_width*window_height*4));
//	cutilSafeCall(cudaMemset(d_cluster, 0, window_width*window_height*4));

    // call CUDA kernel, writing results to PBO
	printf("tf before render_kernel call:\n");
	for(int i = 0; i < tfsize; i++)
		printf("%f, %f, %f, %f\n", tf[i][0], tf[i][1], tf[i][2], tf[i][3]);
    render_kernel(gridSize, blockSize, d_output, /*d_cluster, */d_iRed, d_oRed, d_iGreen, d_oGreen, d_iBlue, d_oBlue,
			d_iColors, data, /*cluster_data, */window_width, window_height, density, brightness, transfer[0], transfer[1], transfer[2],
			transfer[3], transfer[4], transfer[5], tfsize, h_volume, /*cluster, */volumeSize, d_volumeArray, /*d_volumeArray_cluster, */set);

    cutilCheckMsg("kernel failed");

	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0));
//    cutilSafeCall(cudaGraphicsUnmapResources(1, &cluster_pbo_resource, 0));

}

void idle()
{		
	// use OpenGL to build view matrix
    GLfloat modelView[16];
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glLoadIdentity();
        glRotatef(-viewRotation.x, 1.0, 0.0, 0.0);
        glRotatef(-viewRotation.y, 0.0, 1.0, 0.0);
        glTranslatef(-viewTranslation.x, -viewTranslation.y, -viewTranslation.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
    glPopMatrix();

    invViewMatrix[0] = modelView[0]; invViewMatrix[1] = modelView[4]; invViewMatrix[2] = modelView[8]; invViewMatrix[3] = modelView[12];
    invViewMatrix[4] = modelView[1]; invViewMatrix[5] = modelView[5]; invViewMatrix[6] = modelView[9]; invViewMatrix[7] = modelView[13];
    invViewMatrix[8] = modelView[2]; invViewMatrix[9] = modelView[6]; invViewMatrix[10] = modelView[10]; invViewMatrix[11] = modelView[14];

    render();
 
//	int currWin = glutGetWindow();
//	glutSetWindow(window1);
//    glutPostRedisplay();
//	glutSetWindow(window2);
	glutPostRedisplay();
//	glutSetWindow(currWin);	
}

void AutoQATest()
{
    if (g_CheckRender && g_CheckRender->IsQAReadback()) {
        char temp[256];
        sprintf(temp, "AutoTest: Volume Render");
        glutSetWindowTitle(temp);
	shrQAFinishExit2(false, *pArgc, (const char **)pArgv, QA_PASSED);
    }
}
void computeFPS()
{
    frameCount++;
    fpsCount++;
    if (fpsCount == fpsLimit-1) {
        g_Verify = true;
    }
    if (fpsCount == fpsLimit) {
        char fps[256];
        float ifps = 1.f / (cutGetAverageTimerValue(timer) / 1000.f);
        sprintf(fps, "%sVolume Render: %3.1f fps", 
                ((g_CheckRender && g_CheckRender->IsQAReadback()) ? "AutoTest: " : ""), ifps);  

        glutSetWindowTitle(fps);
        fpsCount = 0; 
        if (g_CheckRender && !g_CheckRender->IsQAReadback()) fpsLimit = (int)MAX(ifps, 1.f);

        cutilCheckError(cutResetTimer(timer));  

        AutoQATest();
    }
}

int iDivUp(int a, int b){
    return (a % b != 0) ? (a / b + 1) : (a / b);
}

void freeCudaBuffers()
{
    /* Free colors buffers */

    cutilSafeCall(cudaFreeArray(d_volumeArray));
//	cutilSafeCall(cudaFreeArray(d_volumeArray_cluster));

    cutilSafeCall(cudaFreeArray(d_transferFuncArray));

    cutilSafeCall(cudaFree(d_iColors) );

    cutilSafeCall(cudaFree(data));
//	cutilSafeCall(cudaFree(cluster_data));

    cutilSafeCall(cudaFree(d_iRed));
    cutilSafeCall(cudaFree(d_oRed));
    cutilSafeCall(cudaFree(d_iGreen));
    cutilSafeCall(cudaFree(d_oGreen));
    cutilSafeCall(cudaFree(d_iBlue));
    cutilSafeCall(cudaFree(d_oBlue));
}

void cleanup()
{	
    cutilCheckError( cutDeleteTimer( timer));

    freeCudaBuffers();

    if (pbo) {
        cudaGraphicsUnregisterResource(cuda_pbo_resource);
        glDeleteBuffersARB(1, &pbo);
        glDeleteTextures(1, &tex);
    }

//	if (pbo_cluster) {
//		cudaGraphicsUnregisterResource(cluster_pbo_resource);
//		glDeleteBuffersARB(1, &pbo_cluster);
//		glDeleteTextures(1, &tex_cluster);
//	}

    if (g_CheckRender) {
        delete g_CheckRender; g_CheckRender = NULL;
    }

//	if (g_CheckCluster) {
//		delete g_CheckCluster; g_CheckCluster = NULL;
//	}
}

void initPixelBuffer()
{
    if (pbo) {
        // unregister this buffer object from CUDA C
        cutilSafeCall(cudaGraphicsUnregisterResource(cuda_pbo_resource));

        // delete old buffer
        glDeleteBuffersARB(1, &pbo);
        glDeleteTextures(1, &tex);
    }
    // create pixel buffer object for display
    glGenBuffersARB(1, &pbo);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, window_width*window_height*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    // register this buffer object with CUDA
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard));	

    // create texture for display
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

/*	if (pbo_cluster) {
        // unregister this buffer object from CUDA C
        cutilSafeCall(cudaGraphicsUnregisterResource(cluster_pbo_resource));

        // delete old buffer
        glDeleteBuffersARB(1, &pbo_cluster);
        glDeleteTextures(1, &tex_cluster);
    }

    // create pixel buffer object for display
    glGenBuffersARB(1, &pbo_cluster);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_cluster);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, window_width*window_height*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    // register this buffer object with CUDA
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cluster_pbo_resource, pbo_cluster, cudaGraphicsMapFlagsWriteDiscard));	

    // create texture for display
    glGenTextures(1, &tex_cluster);
    glBindTexture(GL_TEXTURE_2D, tex_cluster);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
*/
}

// General initialization call for CUDA Device
int chooseCudaDevice(int argc, char **argv, bool bUseOpenGL)
{
    int result = 0;
    if (bUseOpenGL) {
        result = cutilChooseCudaGLDevice(argc, argv);
    } else {
        result = cutilChooseCudaDevice(argc, argv);
    }
    return result;
}

void initCuda(void *h_volume, cudaExtent volumeSize, float4 transfer[6], uint image_size)
{
    cutilSafeCall(cudaMalloc( (void**)&d_iColors, image_size * sizeof(float4) ) ); 
       	
    cutilSafeCall(cudaMalloc( (void**)&(d_oRed), image_size * sizeof(float) ) );
    cutilSafeCall(cudaMalloc( (void**)&(d_iRed), image_size * sizeof(float) ) );
    cutilSafeCall(cudaMalloc( (void**)&(d_oGreen), image_size * sizeof(float) ) );
    cutilSafeCall(cudaMalloc( (void**)&(d_iGreen), image_size * sizeof(float) ) );
    cutilSafeCall(cudaMalloc( (void**)&(d_oBlue), image_size * sizeof(float) ) );
    cutilSafeCall(cudaMalloc( (void**)&(d_iBlue), image_size * sizeof(float) ) );
}

/*void initGL(int *argc, char **argv)
	{
	    // initialize GLUT callback functions
	    glutInit(argc, argv);
	    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DOUBLE);
	    glutInitWindowSize(window_width, window_height);

		window1 = glutCreateWindow("Volume");
		glutDisplayFunc(display_volume);
	    glutKeyboardFunc(keyboard_volume);
	    glutMouseFunc(mouse_volume);
	    glutMotionFunc(motion_volume);
	    glutReshapeFunc(reshape_volume);

	//	window2 = glutCreateWindow("Cluster");
	//	glutPositionWindow(560,20);    
	//	glutDisplayFunc(display_cluster);
	//	glutKeyboardFunc(keyboard_volume);
	//	glutMouseFunc(mouse_volume);
	//	glutMotionFunc(motion_volume);
	//	glutReshapeFunc(reshape_cluster);

	    glutIdleFunc(idle);

	    glewInit();
	    if (!glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object")) {
	        shrLog("Required OpenGL extensions missing.");
		shrQAFinishExit(*argc, (const char **)argv, QA_WAIVED);
	    }
}*/

void InitVolRender(int argc, char ** argv)
{	
	
	volumeShader = compileShader(vertexShader, linePixelShader);
	volTexture = glGetUniformLocation(volumeShader, "volume");
	
	printf("start InitVolRender\n"); fflush(stdout);

	tfsize = Parameters::colTranFunc->getSize();
	tf = Parameters::colTranFunc->getPointList();
	for(int index = 0; index < tfsize; index++)
		transfer[index] = make_float4(tf[index][0], tf[index][1], tf[index][2], tf[index][3]);
	/*tf_host = new float4[tfsize];
	printf("tff_host (float4):\n");
	for(int i = 0; i < tfsize; i++)
	{
		tf_host[i] = make_float4(tf[i][1], tf[i][2], tf[i][3], 0.1f);
		printf("%f, %f, %f, %f\n", tf_host[i].x, tf_host[i].y, tf_host[i].z, tf_host[i].w);
	}
	cudaChannelFormatDesc channel = cudaCreateChannelDesc<float4>();
	cudaMallocArray(&tf_device, &channel, tfsize, 1, cudaArrayDefault);
	cudaMemcpy(tf_device, tf_host, tfsize, cudaMemcpyHostToDevice);*/

   	pArgc = &argc;
   	pArgv = argv;

    bool bTestResult = true;

    shrQAStart(argc, argv);

    //start logs
    shrSetLogFileName ("volumeRender.txt");
    shrLog("%s Starting...\n\n", argv[0]); 

    if (cutCheckCmdLineFlag(argc, (const char **)argv, "qatest") ||
        cutCheckCmdLineFlag(argc, (const char **)argv, "noprompt")) 
    {
        g_bQAReadback = true;
        fpsLimit = frameCheckNumber;
    }

    if (cutCheckCmdLineFlag(argc, (const char **)argv, "glverify")) 
    {
        g_bQAGLVerify = true;
        fpsLimit = frameCheckNumber;
    }

    if (g_bQAReadback) {
        // use command-line specified CUDA device, otherwise use device with highest Gflops/s
        chooseCudaDevice(argc, argv, false);
    } else {
        // First initialize OpenGL context, so we can properly set the GL for CUDA.
        // This is necessary in order to achieve optimal performance with OpenGL/CUDA interop.
//      initGL( &argc, argv );

        // use command-line specified CUDA device, otherwise use device with highest Gflops/s
        chooseCudaDevice(argc, argv, true);
    }

	initCuda(h_volume, volumeSize, transfer, window_width*window_height);

    cutilCheckError( cutCreateTimer( &timer ) );

    shrLog("Press '+' and '-' to change density (0.01 increments)\n"
           "      ']' and '[' to change brightness\n"
           "      ';' and ''' to modify transfer function offset\n"
           "      '.' and ',' to modify transfer function scale\n\n");

    // calculate new grid size
    gridSize = dim3(iDivUp(window_width, blockSize.x), iDivUp(window_height, blockSize.y));

    if (g_bQAReadback) {
        g_CheckRender = new CheckBackBuffer(window_width, window_height, 4, false);
        g_CheckRender->setPixelFormat(GL_RGBA);
        g_CheckRender->setExecPath(argv[0]);
        g_CheckRender->EnableQAReadback(true);

//		 g_CheckCluster = new CheckBackBuffer(window_width, window_height, 4, false);
//	     g_CheckCluster->setPixelFormat(GL_RGBA);
//	     g_CheckCluster->setExecPath(argv[0]);
//	     g_CheckCluster->EnableQAReadback(true);

        uint *d_output/*, *d_cluster*/;
        cutilSafeCall(cudaMalloc((void**)&d_output, window_width*window_height*sizeof(uint)));
        cutilSafeCall(cudaMemset(d_output, 0, window_width*window_height*sizeof(uint)));

//		cutilSafeCall(cudaMalloc((void**)&d_cluster, window_width*window_height*sizeof(uint)));
//		cutilSafeCall(cudaMemset(d_cluster, 0, window_width*window_height*sizeof(uint)));

        float modelView[16] = 
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 4.0f, 1.0f
        };

        invViewMatrix[0] = modelView[0]; invViewMatrix[1] = modelView[4]; invViewMatrix[2] = modelView[8]; invViewMatrix[3] = modelView[12];
        invViewMatrix[4] = modelView[1]; invViewMatrix[5] = modelView[5]; invViewMatrix[6] = modelView[9]; invViewMatrix[7] = modelView[13];
        invViewMatrix[8] = modelView[2]; invViewMatrix[9] = modelView[6]; invViewMatrix[10] = modelView[10]; invViewMatrix[11] = modelView[14];

        // call CUDA kernel, writing results to PBO
	    copyInvViewMatrix(invViewMatrix, sizeof(float4)*3);

        // Start timer 0 and process n loops on the GPU 
        int nIter = 10;
        for (int i = -1; i < nIter; i++)
        {
            if( i == 0 ) {
                cutilDeviceSynchronize();
                cutStartTimer(timer); 
            }
			render_kernel(gridSize, blockSize, d_output, /*d_cluster, */d_iRed, d_oRed, d_iGreen, d_oGreen, d_iBlue, d_oBlue, 
						d_iColors, data, /*cluster_data, */window_width, window_height, density, brightness, transfer[0], transfer[1], transfer[2], 
						transfer[3], transfer[4], transfer[5], tfsize, h_volume, /*cluster, */volumeSize, d_volumeArray, /*d_volumeArray_cluster, */set);
        }
        cutilDeviceSynchronize();
        cutStopTimer(timer);
        // Get elapsed time and throughput, then log to sample and master logs
        double dAvgTime = cutGetTimerValue(timer)/(nIter * 1000.0);
        shrLogEx(LOGBOTH | MASTER, 0, "volumeRender, Throughput = %.4f MTexels/s, Time = %.5f s, Size = %u Texels, NumDevsUsed = %u, Workgroup = %u\n", 
               (1.0e-6 * window_width * window_height)/dAvgTime, dAvgTime, (window_width * window_height), 1, blockSize.x * blockSize.y); 

        cutilCheckMsg("Error: render_kernel() execution FAILED");
        cutilSafeCall( cutilDeviceSynchronize() );

        cutilSafeCall( cudaMemcpy(g_CheckRender->imageData(), d_output, window_width*window_height*4, cudaMemcpyDeviceToHost) );
        g_CheckRender->savePPM(sOriginal[g_Index], true, NULL);

        bTestResult = g_CheckRender->PPMvsPPM(sOriginal[g_Index], sReference[g_Index], MAX_EPSILON_ERROR, THRESHOLD);

//		cutilSafeCall( cudaMemcpy(g_CheckCluster->imageData(), d_cluster, window_width*window_height*4, cudaMemcpyDeviceToHost) );
//		g_CheckRender->savePPM(sOriginal[g_Index], true, NULL);

//		bTestResult = g_CheckCluster->PPMvsPPM(sOriginal[g_Index], sReference[g_Index], MAX_EPSILON_ERROR, THRESHOLD);

        cudaFree(d_output);
    	freeCudaBuffers();

        if (g_CheckRender) {
            delete g_CheckRender; g_CheckRender = NULL;
        }
//		if (g_CheckCluster) {
//			delete g_CheckCluster; g_CheckCluster = NULL;
//		}

    } else {
        // This is the normal rendering path for VolumeRender

		initPixelBuffer();

        if (g_bQAGLVerify) {
            g_CheckRender = new CheckBackBuffer(window_width, window_height, 4);
            g_CheckRender->setPixelFormat(GL_RGBA);
            g_CheckRender->setExecPath(argv[0]);
            g_CheckRender->EnableQAReadback(true);

//			g_CheckCluster = new CheckBackBuffer(window_width, window_height, 4);
//			g_CheckCluster->setPixelFormat(GL_RGBA);
//			g_CheckCluster->setExecPath(argv[0]);
//			g_CheckCluster->EnableQAReadback(true);
        }

        atexit(cleanup);
printf("before glutMainLoop in callbacks::InitVolRender\n"); fflush(stdout);
        glutMainLoop();
    }

    cutilDeviceReset();
    shrQAFinishExit(argc, (const char **)argv, (bTestResult ? QA_PASSED : QA_FAILED) );
	printf("end InitVolRender\n"); fflush(stdout);
}