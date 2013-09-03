#include "glwidget.h"


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

    void GLWidget::draw_axes( void ) {
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
    void GLWidget::processHits (GLint hits, GLuint buffer[])
    {

        printf("hits\n");

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

    void GLWidget::MouseMoveToRotate(int diffX, int diffY){
        // map mousemotion into degrees
        //normalize mouse motion
        float yNormalizer = 360*3/this->width  ;  // rotate 360/3 = 120 degree if your mouse drag covers half of the screen
        float xNormalizer = 360*3/this->height ; // rotate 120 degree if your mouse drag covers half of the screen

        angley += diffX/yNormalizer ;
        anglex += diffY/xNormalizer ;

    //    volRenderer.RotateObject(anglex, angley, anglez ) ;
    //    volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity

        updateGL();
    }

    void GLWidget::MouseMoveToSeedpointMove(float diffX, float diffY){
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

    void GLWidget::MouseMoveToZoom(int diffX, int diffY){

        volRenderer.ChangeProjectorLocation(diffY) ;
        volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity

        updateGL();
    }

    // mouse motion callback

    void GLWidget::mouseMoveEvent(QMouseEvent *event){

        printf("motion\n");

        if( CurrIM == ExploreMode){
            if(LeftButtonDown == 1 ){				// rotate the volume
                MouseMoveToRotate(event->x()-buttonDownX, event->y()-buttonDownY) ;
                buttonDownX = event->x() ;
                buttonDownY = event->y() ;
            }
            else if( RightButtonDown == 1){			// move the camera, zoom in or out
                MouseMoveToZoom(event->x()-buttonDownX, event->y()-buttonDownY) ;
                buttonDownX = event->x() ;
                buttonDownY = event->y() ;
            }
        } //if( InteractionMode == ExploreMode)
        else if( CurrIM == SelectMode){
            if(LeftButtonDown == 1 ){				// temporary streamline selection or seedpoint move
                if( streamlines.bSeedpointSelected && iSeedpointMoveDir != -1 ){// seedpoint was already selected
                    MouseMoveToSeedpointMove(event->x()-buttonDownX, event->y()-buttonDownY) ;
                    buttonDownX = event->x() ;
                    buttonDownY = event->y() ;
                    updateGL();
                }
                else{
                    SelectObjectAtXY(event->x(), event->y());
                    switch(ObjectCatagory){
                        case STREAMLINE:
                            //printf("ObjectName %d\n",iObjectName) ;
                            streamlines.bSeedpointSelected = false ;
                            streamlines.TemporarySelectedStreamline(iObjectName) ;
                            updateGL();
                            break ;
                        case SEEDPOINT:
                            if(!streamlines.bSeedpointSelected){		// user just moved in to seed point
                                buttonDownX = event->x() ;
                                buttonDownY = event->y() ;
                                streamlines.bSeedpointSelected = true ;
                                updateGL();
                            }
                            break ;
                        case NO_OBJECT:
                            streamlines.TempSelectedStreamline = -1 ;
                            streamlines.bSeedpointSelected = false ;
                            updateGL();
                            break ;
                        default:
                            printf("Unidentified object.\n") ;
                            break ;
                    }  //switch(iObjectCatagory){
                } // else
            } //if(LeftButtonDown == 1 ){
        } //else if( CurrIM == SelectMode){
    }

    // the mouse callback
    void GLWidget::mousePressEvent(QMouseEvent *event){

        printf("press\n");

        if( CurrIM == ExploreMode ){				// xplore mode mouse interaction
            if (event->buttons() ) {
                 if (Qt::LeftButton){
                      LeftButtonDown = 1 ;
                      buttonDownX = event->x() ;
                      buttonDownY = event->y() ;
                 }

                 else if(Qt::RightButton){
                      RightButtonDown = 1 ;
                      buttonDownX = event->x() ;
                      buttonDownY = event->y() ;
                }
            } //if (state == GLUT_DOWN )
            else if( event->buttons() ){
                LeftButtonDown = RightButtonDown = 0 ;
                volRenderer.SetProjectionResolution(false, false) ;
                updateGL() ;
            }
        } //if( CurrIM == ExploreMode )
        else if(CurrIM == SelectMode ){
            if (event->buttons() ) {
                if( Qt::RightButton){
                        LeftButtonDown = 1 ;
                        SelectObjectAtXY(event->x(), event->y());
                        switch(ObjectCatagory){
                            case STREAMLINE:
                                streamlines.TemporarySelectedStreamline(iObjectName);
                                updateGL() ;
                                break ;
                            case SEEDPOINT:
                            buttonDownX = event->x ();
                            buttonDownY = event->y() ;
                                streamlines.bSeedpointSelected = true ;
                                updateGL() ;
                                break ;
                            case NO_OBJECT:
                                break;
                            default:
                                printf("Unidentified object.\n") ;
                                break ;
                        }//switch(iObjectCatagory){
            }
            } //if (state == GLUT_DOWN )
            else if( event->buttons() ){
                LeftButtonDown = RightButtonDown = 0 ;
                //clear if any streamline was temporarily selected
                streamlines.TempSelectedStreamline = -1 ;
                switch(ObjectCatagory){
                    case STREAMLINE:
                        streamlines.UpdateStreamlineStatus(iObjectName) ;
                        updateGL() ;
                        break ;
                    case SEEDPOINT:
                        if( streamlines.bSeedpointMoved )
                            streamlines.UpdateSeedPoint() ;
                        streamlines.bSeedpointMoved = false ;
                        streamlines.bSeedpointSelected = false ;
                        ObjectCatagory = NO_OBJECT ;
                        iSeedpointMoveDir = -1 ;
                        updateGL() ;
                        break ;
                    case NO_OBJECT:
                        break ;
                }//switch(ObjectCatagory){
            }//else if( state == GLUT_UP )
        } //else if(CurrIM == SelectMode )
    }

    void GLWidget::Magnify(){

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
