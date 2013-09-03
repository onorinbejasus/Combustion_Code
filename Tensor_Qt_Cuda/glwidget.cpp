#include "glwidget.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tensor files
#include <time.h>
#include <fstream>

#include "FatalError.hh"
#include "Streamline.hh"
#include "AllocArray.hh"
#include "VectorFieldRenderer.hh"
#include "Parameters.hh"

#include "Projector.hh"
#include "Parameters.hh"
#include "Vector.hh"
#include "point.hh"

#include "open_gl.hh"
#include "enums.hh"

#include <iostream>
#include <cmath>

#define PI 3.14159265

extern void DrawAxisAndBox(double, double, double); // from box.cpp

static float zoom = 3.0;
static float radius = 0.1;
static float mag = 1.0;
bool magnifying = false;
bool shift = false;

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

using namespace std;

InteractionMode CurrIM = ExploreMode;

int buttonDownX, buttonDownY;
int LeftButtonDown, RightButtonDown;
GLfloat anglex, angley, anglez;

SelectedObjectCatagory ObjectCatagory;
int iObjectName;

int iSeedpointMoveDir = -1;			// 0 for x, 1 for y and 2 for z

GLfloat positionX = 0.0;
GLfloat positionY = 0.0;

Streamline				streamlines;				//streamlines object.
Projector               projector;					//projector
VectorFieldRenderer		vectorField;				//vector field renderer
Vector 					***velocityField;			//vectorfield data
Vector 					***eigenValues;
Vector 					***eigenVectors;
GLfloat 				*eigenfloats; 				// eigen values in float form

int slices = 0;
int rows = 0;
int cols = 0;

extern void readData(); // from fileOps.cpp
extern void readVelocityField(std::string path); // from fileOps.cpp

std::string parameters;

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    paramsLoaded = false;
}

void GLWidget::initDisplay(std::string params)
{
    parameters = params;
    paramsLoaded = true;
    this->initializeGL();
}

float GLWidget::getMagZoom()
{
    return zoom;
}

void GLWidget::setMagZoom(float value)
{
    zoom = value;
    updateGL();
}

float GLWidget::getMagRadius()
{
    return radius;
}

void GLWidget::setMagRadius(float value)
{
    radius = value;
    updateGL();
}

void GLWidget::configure()
{
    readData(); // read a vector field of size slices, rows AND cols

    double boxLenX = Parameters::CtFileVoxelSpacing->VectorX;
    double boxLenY = Parameters::CtFileVoxelSpacing->VectorY;
    double boxLenZ = Parameters::CtFileVoxelSpacing->VectorZ;

    // setup the streamline object
    streamlines.SetVectorField(velocityField, slices, rows, cols);

    streamlines.Configure(*(Parameters::SeedPointLoc), 54);
    streamlines.SetSpacing(boxLenX, boxLenY, boxLenZ);
    if(streamlines.bReadyToProcess)		// generate streamlines and save them in display lists
        streamlines.CreateStreamlines();
    else{
        printf("We have problem. Streamlines can't be generated.\n");
        exit(1);
    }

    vectorField.Configure(eigenVectors, eigenValues, &streamlines, slices, rows, cols);
    vectorField.SetSpacing(boxLenX, boxLenY, boxLenZ);

    vectorField.RenderStreamlines(mag) ;	// render vector field
}

    void GLWidget::processHits(GLint hits, GLuint buffer[])
    {

       unsigned int i, j;
       GLuint names, *ptr;

       float fMinMin = 10.0f, fMaxMin= 10.0f;		// since the z value should be in the range 0 to 1
       int iMinZWinner=-1, iMaxZWinner=-1;
       SelectedObjectCatagory ObjCat = NO_OBJECT;

       //printf ("hits = %d\n", hits);
       ptr = (GLuint *) buffer;

       for (i = 0; i < (unsigned)hits; i++) {	/*  for each hit  */
          names = *ptr;
          //printf(" number of names for hit = %d\n", names);
          ptr++;
          if( names != 2 ){
            printf("We have problem here.\n");
            exit(1);
          }
          float z1 = (float) *ptr/0x7fffffff; ptr++;
          float z2 = (float) *ptr/0x7fffffff; ptr++;
          int iIndex;

          //printf(" z1 is %g\n", z1 );
          //printf(" z2 is %g\n", z2 );

          int oc = *ptr; ptr++;
          iIndex = *ptr; ptr++;

          if( oc == SEEDPOINT ){		// SEEDPOINT HAS HIGHER PRIORITY
              ObjCat = (SelectedObjectCatagory)oc;
              iMinZWinner = iIndex;
              printf("Seedpoint selected.\n");
          }								//if( oc == SEEDPOINT )
          else{
              if( z1 < fMinMin ){
                fMinMin = z1;
                iMinZWinner = iIndex;
                ObjCat = (SelectedObjectCatagory)oc;
              }
              if( z2 < fMaxMin){
                fMaxMin = z2;
                iMaxZWinner = iIndex;
              }
          }//else
       } //for (i = 0; i < (unsigned)hits; i++)

        ObjectCatagory =  ObjCat;
        iObjectName  = iMinZWinner;
        printf("Catagory = %d and ObjecSelected = %d\n",ObjCat, iMinZWinner);
    }

    // selects an object at location X,Y in screen coordinate
    // single object selection. Catagory of the object will be set in oc. Right now we have just one catagory. STREAMLINE
    // and name of the object will be set in on
    #define BUFSIZE					512			// buffer to process mouse click hits

    void GLWidget::SelectObjectAtXY(int x, int y){
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
        gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y), 5.0, 5.0, viewport);

        glMatrixMode (GL_MODELVIEW);
        glPushName(STREAMLINE);							// signature of a streamline
        streamlines.SelectModeStreamlinesDraw();
        glPopName();									// pop streamline signature

        glPushName(SEEDPOINT);							// signature of a seedpoint
        streamlines.SelectModeSeedPointDraw();
        glPopName();									// pop seedpoint signature

        glMatrixMode (GL_PROJECTION);
        glPopMatrix ();
        //glFlush ();

        hits = glRenderMode (GL_RENDER);
        if( hits > 0 )
            processHits (hits, selectBuf);
        else{		// no hit
            ObjectCatagory = NO_OBJECT;
        }
    }

    void GLWidget::MouseMoveToRotate(int diffX, int diffY){
        // map mousemotion into degrees
        //normalize mouse motion
        float yNormalizer = 360*3/this->width;  // rotate 360/3 = 120 degree if your mouse drag covers half of the screen
        float xNormalizer = 360*3/this->height; // rotate 120 degree if your mouse drag covers half of the screen

        angley += diffX/yNormalizer;
        anglex += diffY/xNormalizer;

    //    volRenderer.RotateObject(anglex, angley, anglez );
    //    volRenderer.SetProjectionResolution(true, true);			// Fastpass and intensity

        updateGL();
    }

    void GLWidget::MouseMoveToSeedpointMove(float diffX, float diffY){
        fPoint fsp;
        streamlines.GetSeedPoint(fsp);
        //printf("%f\n",diffY);
        switch(iSeedpointMoveDir){
            case 0:			//along x axis
                fsp.x -= (diffY*2);
                break;
            case 1:
                fsp.y -= (diffY*2);
                break;
            case 2:
                fsp.z -= (diffY*2);
                break;
        }
        streamlines.MoveSeedPoint(fsp);
    }

    void GLWidget::MouseMoveToZoom(int diffX, int diffY){

//        volRenderer.ChangeProjectorLocation(diffY);
//        volRenderer.SetProjectionResolution(true, true);			// Fastpass and intensity
//        updateGL();
    }

    // mouse motion callback

    void GLWidget::mouseMoveEvent(QMouseEvent *event){

        mouseMotion(event->pos().x(), event->y());

        if( CurrIM == ExploreMode){
            if(LeftButtonDown == 1 ){				// rotate the volume
                MouseMoveToRotate(event->x()-buttonDownX, event->y()-buttonDownY);
                buttonDownX = event->x();
                buttonDownY = event->y();
            }
            else if( RightButtonDown == 1){			// move the camera, zoom in or out
                MouseMoveToZoom(event->x()-buttonDownX, event->y()-buttonDownY);
                buttonDownX = event->x();
                buttonDownY = event->y();
            }
        } //if( InteractionMode == ExploreMode)
        else if( CurrIM == SelectMode){
            if(LeftButtonDown == 1 ){				// temporary streamline selection or seedpoint move
                if( streamlines.bSeedpointSelected && iSeedpointMoveDir != -1 ){// seedpoint was already selected
                    MouseMoveToSeedpointMove(event->x()-buttonDownX, event->y()-buttonDownY);
                    buttonDownX = event->x();
                    buttonDownY = event->y();
                    updateGL();
                }
                else{
                    SelectObjectAtXY(event->x(), event->y());
                    switch(ObjectCatagory){
                        case STREAMLINE:
                            //printf("ObjectName %d\n",iObjectName);
                            streamlines.bSeedpointSelected = false;
                            streamlines.TemporarySelectedStreamline(iObjectName);
                            updateGL();
                            break;
                        case SEEDPOINT:
                            if(!streamlines.bSeedpointSelected){		// user just moved in to seed point
                                buttonDownX = event->x();
                                buttonDownY = event->y();
                                streamlines.bSeedpointSelected = true;
                                updateGL();
                            }
                            break;
                        case NO_OBJECT:
                            streamlines.TempSelectedStreamline = -1;
                            streamlines.bSeedpointSelected = false;
                            updateGL();
                            break;
                        default:
                            printf("Unidentified object.\n");
                            break;
                    }  //switch(iObjectCatagory){
                } // else
            } //if(LeftButtonDown == 1 ){
        } //else if( CurrIM == SelectMode){
    }

    void GLWidget::mouseReleaseEvent(QMouseEvent *event)
    {
        LeftButtonDown = RightButtonDown = 0;
/*
        if(CurrIM == ExploreMode){
            LeftButtonDown = RightButtonDown = 0;
            //volRenderer.SetProjectionResolution(false, false);
            updateGL();
        } else {
            LeftButtonDown = RightButtonDown = 0;
            //clear if any streamline was temporarily selected
            streamlines.TempSelectedStreamline = -1;
            switch(ObjectCatagory){
                case STREAMLINE:
                    streamlines.UpdateStreamlineStatus(iObjectName);
                    updateGL();
                    break;
                case SEEDPOINT:
                    if( streamlines.bSeedpointMoved )
                        streamlines.UpdateSeedPoint();
                    streamlines.bSeedpointMoved = false;
                    streamlines.bSeedpointSelected = false;
                    ObjectCatagory = NO_OBJECT;
                    iSeedpointMoveDir = -1;
                    updateGL();
                    break;
                case NO_OBJECT:
                    break;
            }//switch(ObjectCatagory){
        }
*/
    }

    // the mouse callback
    void GLWidget::mousePressEvent(QMouseEvent *event){
        if( CurrIM == ExploreMode ){
            if (Qt::LeftButton){
                LeftButtonDown = 1;
                buttonDownX = event->x();
                buttonDownY = event->y();
            }
            else if(Qt::RightButton){
                RightButtonDown = 1;
                buttonDownX = event->x();
                buttonDownY = event->y();
            }
        } //if( CurrIM == ExploreMode )
        else if(CurrIM == SelectMode ){
            if(Qt::LeftButton){
                LeftButtonDown = 1;
                SelectObjectAtXY(event->x(), event->y());
                std::cout << "ObjectCatagory = " << ObjectCatagory << std::endl;
                switch(ObjectCatagory){
                    case STREAMLINE:
                        streamlines.TemporarySelectedStreamline(iObjectName);
                        updateGL();
                        break;
                    case SEEDPOINT:
                        buttonDownX = event->x();
                        buttonDownY = event->y();
                        streamlines.bSeedpointSelected = true;
                        updateGL();
                        break;
                    case NO_OBJECT:
                        break;
                    default:
                        printf("Unidentified object.\n");
                        break;
                } //switch(iObjectCatagory)
            } //if(Qt::LeftButton)
        } //else if(CurrIM == SelectMode )
    }

    void GLWidget::Magnify(){

        LeftButtonDown = RightButtonDown = 0;

        // go back to 2D first
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 1.0, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glBegin(GL_POINTS); // draw circle around area

        glColor3f(1.0, 1.0, 1.0); // white

        for(int i = 0; i <= 360; i++) { // increment thru the degrees

            glVertex2f( (float)(positionX/this->width) + radius * cos(i * M_PI/180.0) ,
                       ( (float)(positionY/this->height) ) + radius * sin(i * M_PI/180.0) );
        }

        glEnd();

         glClearStencil(0); // clear stencil

         glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); // disable drawing
         glEnable(GL_STENCIL_TEST); // endable stencil test

         glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF); // always past test, replace with 1
         glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // always replace

        // stencil magnification area

        glBegin(GL_TRIANGLE_FAN);

        glVertex2f( (float)(positionX/this->width), (float)(positionY/this->height) ); // origin

        for(int i = 0; i <= 360; i++) { // increment thru the degrees

            glVertex2f( (float)(positionX/this->width) + radius * cos(i * M_PI/180.0) ,
                       ( (float)(positionY/this->height) ) + radius * sin(i * M_PI/180.0) );
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
    void GLWidget::mouseMotion(int x, int y){

        if(!magnifying)
            return;

        /* mouse coordinate */

        positionX = x;
        positionY = this->height - y;

        updateGL();
    }

    void GLWidget::initializeGL()
    {
        if(paramsLoaded == false)
            return;
//        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL);
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_SMOOTH);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);

        this->width = this->geometry().width();
        this->height = this->geometry().height();
        this->setMouseTracking(true);

        //parameters = "/Users/amaries/Desktop/Combustion_Code/data/AbedToTim/2/parameters.csv";
        Parameters::LoadParameters(parameters);
        projector.Initialize(this->width, this->height);

        resizeGL(this->width, this->height);

        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity();  // init modelview to identity

        configure();
        paintGL();
    }

    void GLWidget::paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();             /* clear the matrix */

        gluLookAt(0.0, 0.0, projector.vSourceLocation.VectorZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        //volRenderer.RenderVolImage();			// render the volume image

        glClear(GL_DEPTH_BUFFER_BIT);			// so that this volume image does not block other objects
                                                // Since we have enabled depth test
        glRotatef(anglex,1.0,0.0,0.0);
        glRotatef(angley,0.0,1.0,0.0);
        glRotatef(anglez,0.0,0.0,1.0);

        double boxLenX = Parameters::CtFileVoxelSpacing->VectorX;
        double boxLenY = Parameters::CtFileVoxelSpacing->VectorY;
        double boxLenZ = Parameters::CtFileVoxelSpacing->VectorZ;

        // draw axis and bounding box
        if (paramsLoaded == true)
            DrawAxisAndBox(boxLenX, boxLenY, boxLenZ);

        // get the volume center in the coordinate center
        float tz = (slices)*boxLenZ/2;
        float ty = (rows)*boxLenY/2;
        float tx = (cols)*boxLenX/2;

        glTranslatef(-tx, -ty, -tz);

        if(streamlines.bVisible)
            streamlines.RenderStreamlines(eigenfloats);		// display streamlines

        if(vectorField.bVisible)
        {
            if(shift)
                vectorField.RenderSlice();
            else
                vectorField.drawQuadric();	// draw vector field
        }

        if(magnifying)
            Magnify();

        glPopMatrix();
        swapBuffers();
//        glutSwapBuffers();
//        paintGL();

    }

    void GLWidget::resizeGL(int w, int h)
    {
        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float fProjectionPlanePixelSize = projector.fDetectorPixelSize;

        /* viewing transformation */
        glFrustum (-1.0 * w/2 * fProjectionPlanePixelSize,
                   1.0 * w/2 * fProjectionPlanePixelSize,
                   -1.0 * h/2 * fProjectionPlanePixelSize,
                   1.0 * h/2 * fProjectionPlanePixelSize,
                   projector.fSourceToDetector, 15000);

        glMatrixMode (GL_MODELVIEW);
    }


