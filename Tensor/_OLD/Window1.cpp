#include <math.h>
#include "Window1.h"

using namespace std;

#define NUM_OF_SOURCES 2

 Tensor1::Tensor1(int width, int height, int t, QWidget *parent)
     : QGLWidget(parent)
 {
	this->ImageWidth = width ;
	this->ImageHeight = height;
	this->type = t ;

	// defualt initialization
	parallel_rotate = false;
	//iSeedpointMoveDir = -1;

	tensorDataSet = NULL;
	// initialize the projector i.e. camera with the default values
	// note that to make the projector working you will need to 
	// set other parameters such as opacity and color transfer function
	// and last of all you will need to call calculateTransformations()
	projector = new Projector();
	Vector projectorLocation = Vector(0,0,1800) ;
	Vector projectorDirection(0.0, 0.0, -1.0) ;
	Vector centralSpot(ImageWidth/2,ImageHeight/2,0) ;
	iPoint viewportSize(ImageWidth,ImageHeight,0) ;
	float fSourceToDetector = 300 ;
	float fDetectorPixelSize = DEFUALT_PIXEL_SIZE;
    this->projector->Configure(projectorLocation,                  // set up the projector system
                    projectorDirection,
                    fSourceToDetector,
                    viewportSize,
                    fDetectorPixelSize,
                    centralSpot);														

	// visualization mode
	CurrIM = ExploreMode1 ;

	vectorField = new VectorFieldRenderer();
	streamlines = new StreamlineRenderer();
	//volRenderer = new VolumeRenderer(width,height);
 }

 Tensor1::~Tensor1()
 {
 }

 void Tensor1::ReconfigureProjector(Parameters *param){
	makeCurrent();
	// initialize the camera related parameters first
	Vector projectorLocation = Vector(0,0,1800) ;
	Vector projectorDirection(0.0, 0.0, -1.0) ;
	Vector centralSpot(ImageWidth/2,ImageHeight/2,0) ;
	iPoint viewportSize(ImageWidth,ImageHeight,0) ;
	float fSourceToDetector = param->RedSourceToDetector;// 
	float fDetectorPixelSize = DEFUALT_PIXEL_SIZE;
    projector->Configure(projectorLocation,                  // set up the projector system
                    projectorDirection,
                    fSourceToDetector,
                    viewportSize,
                    fDetectorPixelSize,
                    centralSpot);														

    //projector->iFastPassSamplingInterval = 8;				// configure the fast-pass projection method		
																// every 8th pixel is ray-casted. Rest are interpolated
	//updating the transfer functions
	//projector->opacityFunction = param->opTranFunc ;
	//projector->colorTranFun = param->colTranFunc ; 

	//calculate transformation matrices
	projector->CalculateTransformations() ; 
	//projector.printProjector() ; 


	// now initialize the viewing volume related parameters
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   float fProjectionPlanePixelSize = projector->fDetectorPixelSize;
  
   glFrustum (	-1.0* ImageWidth/2 * fProjectionPlanePixelSize,
				ImageWidth/2 * fProjectionPlanePixelSize,
				-1.0* ImageHeight/2 * fProjectionPlanePixelSize,
				ImageHeight/2 * fProjectionPlanePixelSize, 
				projector->fSourceToDetector, 
				25000); 
  
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();

   // reinitialize to explore mode
   CurrIM = ExploreMode1 ;

 }

 // configures the projector of the current window from
 // another projector
void Tensor1::ReconfigureProjector(Projector *srcProjector){
	//projector->Reset();
	makeCurrent();
	// initialize the camera related parameters first
	//Vector projectorLocation = param->RedSourceLoc ; //Vector(0,0,1800) ;
	Vector projectorLocation = Vector(srcProjector->vSourceLocation.VectorX,
										srcProjector->vSourceLocation.VectorY,
										srcProjector->vSourceLocation.VectorZ);
	Vector projectorDirection(0.0, 0.0, -1.0) ;
	Vector centralSpot(ImageWidth/2,ImageHeight/2,0) ;
	iPoint viewportSize(ImageWidth,ImageHeight,0);

	float fSourceToDetector = srcProjector->fSourceToDetector;// 400 ;
	float fDetectorPixelSize = srcProjector->fDetectorPixelSize;
    projector->Configure(projectorLocation,                  // set up the projector system
                    projectorDirection,
                    fSourceToDetector,
                    viewportSize,
                    0.001,//fDetectorPixelSize,
                    centralSpot);														

    //projector->iFastPassSamplingInterval = 8;				// configure the fast-pass projection method		
																// every 8th pixel is ray-casted. Rest are interpolated
	//updating the transfer functions
	//projector->opacityFunction = srcProjector->opacityFunction ;
	//projector->colorTranFun = srcProjector->colorTranFun ; 

	//calculate transformation matrices
	projector->CalculateTransformations() ; 

	// now initialize the viewing volume related parameters
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   float fProjectionPlanePixelSize = srcProjector->fDetectorPixelSize ;
  
  /* glFrustum (	-1.0 * ImageWidth/2 * fProjectionPlanePixelSize,
				1.0	 * ImageWidth/2 * fProjectionPlanePixelSize,
				-1.0 * ImageHeight/2 * fProjectionPlanePixelSize,
				1.0  * ImageHeight/2 * fProjectionPlanePixelSize, 
				srcProjector->fSourceToDetector, 
				15000); */
  
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();

   // reinitialize to explore mode
   CurrIM = ExploreMode1 ; 
 }

 //void Tensor1::initializeGL(char* paramFileName, int size, Tensor1 *neigh)
 void Tensor1::initializeGL()
 {
	makeCurrent();
	setFocusPolicy(Qt::ClickFocus); 	
	// set neighbor node
	// neighbor = neigh; 
	 // opengl initialization
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH) ;
	glEnable(GL_POINT_SMOOTH) ;

 }

 void Tensor1::paintGL()
 {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity ();             /* clear the matrix */
           /* viewing transformation  */
    
	gluLookAt (0.0, 0.0, projector->vSourceLocation.VectorZ  , 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
	
	if(tensorDataSet!=NULL && tensorDataSet->bDataLoaded )
		//volRenderer.RenderVolImage() ;			// render the volume image 

	glClear(GL_DEPTH_BUFFER_BIT) ;

	if ( tensorDataSet!=NULL && tensorDataSet->bDataLoaded ){
		glRotatef(commonViewingParams.angleX,1.0,0.0,0.0) ;
		glRotatef(commonViewingParams.angleY,0.0,1.0,0.0) ;
		glRotatef(commonViewingParams.angleZ,0.0,0.0,1.0) ;

		DrawAxisAndBox() ;						// draw axis and bounding box

		// get the volume center in the coordinate center
		float tz = (tensorDataSet->ipVolSize.z)*tensorDataSet->vSpacing.VectorZ/2 ;
		float ty = (tensorDataSet->ipVolSize.y)*tensorDataSet->vSpacing.VectorY/2 ;
		float tx = (tensorDataSet->ipVolSize.x)*tensorDataSet->vSpacing.VectorX/2 ;;	
			
		glTranslatef(-tx, -ty, -tz) ;

	//	vectorField->RenderSlice() ;				// draw vector field
		streamlines->RenderStreamlines() ;		// display streamlines
		vectorField->RenderStreamlines(streamlines); // render glyphs
		//glColor3f(0.5, 1.0, 0.5) ;
		//glutSolidSphere(10, 50, 50) ;
	}
	//else{	// testing code
	//	glColor3f(1.0, 0.5, 0.5) ;
	//	glutSolidSphere(10, 50, 50) ;
	//}
 }

 void Tensor1::resizeGL(int width, int height)
 {	
   makeCurrent();
   std::cout << "width: " << width << " height: " << height << std::endl;

   ImageWidth = width;
   ImageHeight = height;
   glViewport (0, 0, (GLsizei) ImageWidth, (GLsizei) ImageHeight); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   float fProjectionPlanePixelSize = projector->fDetectorPixelSize ;
  
   /*
   glFrustum (	-1.0 * ImageWidth/2 * fProjectionPlanePixelSize,
				1.0	 * ImageWidth/2 * fProjectionPlanePixelSize,
				-1.0 * ImageHeight/2 * fProjectionPlanePixelSize,
				1.0  * ImageHeight/2 * fProjectionPlanePixelSize, 
				0.0001,//projector->fSourceToDetector, 
				15000); 
  */
   glMatrixMode (GL_MODELVIEW);
  
 }

 void Tensor1::mousePressEvent(QMouseEvent *event)
 {
	makeCurrent();
    if( CurrIM == ExploreMode1 ){				// xplore mode mouse interaction
		switch (event->buttons()) {
		case Qt::LeftButton:
			LeftButtonDown = 1 ;
			buttonDownX = event->x() ;
			buttonDownY = event->y() ;
			break;
		case Qt::MiddleButton:
			 break;
		case Qt::RightButton:
			RightButtonDown = 1 ;
			buttonDownX = event->x() ;
			buttonDownY = event->y() ;
				 break;
			 }// end switch		
    } //end if
	else if(CurrIM == SelectMode1 ){		
		switch (event->buttons()) {
			case Qt::LeftButton:
				LeftButtonDown = 1 ;
				SelectObjectAtXY(event->x(), event->y());
				switch(ObjectCatagory){
					case STREAMLINE1:
						//printf("ObjectName %d\n",ObjectName) ;
						streamlines->TemporarySelectedStreamline(iObjectName);
						updateGL() ;
						break ;
					case SEEDPOINT1:
						buttonDownX = event->x() ;
						buttonDownY = event->y() ;
						streamlines->bSeedpointSelected = true ;
						updateGL() ;
						break ;
					case NO_OBJECT1:
						break;
					default:
						printf("Unidentified object.\n") ;							
						break ;
				}//switch(iObjectCatagory){  			
			break;
		}//switch (button) 
	}// end else
}

 void Tensor1::mouseReleaseEvent(QMouseEvent *event){
		
	makeCurrent();

	if( CurrIM == ExploreMode1 ){
			LeftButtonDown = RightButtonDown = 0 ;
			//volRenderer.SetProjectionResolution(false, false) ; 
			updateGL() ;

			if(parallel_rotate && neighbor){
				//neighbor->volRenderer.SetProjectionResolution(false, false) ; 
				neighbor->updateGL() ;
			}

	}else if(CurrIM == SelectMode1 ){
		
		LeftButtonDown = RightButtonDown = 0 ;						
		//clear if any streamline was temporarily selected
		streamlines->TempSelectedStreamline = -1 ;
		switch(ObjectCatagory){				
			case STREAMLINE1:					
				streamlines->UpdateStreamlineStatus(iObjectName) ;					
				updateGL() ;
				break ;
			case SEEDPOINT1:
				if( streamlines->bSeedpointMoved )
					streamlines->UpdateSeedPoint() ;
				streamlines->bSeedpointMoved = false ;
				streamlines->bSeedpointSelected = false ;
				ObjectCatagory = NO_OBJECT1 ;
				streamlines->iSeedpointMoveDir = -1 ;
				updateGL() ;
				break ;
			case NO_OBJECT1:
				break ;
		}//switch(ObjectCatagory){  			
	}//else if

}
 void Tensor1::MouseMoveToRotate(int diffX, int diffY){
	float yNormalizer = 360*3/ImageWidth  ;  // rotate 360/3 = 120 degree if your mouse drag covers half of the screen
	float xNormalizer = 360*3/ImageHeight ; // rotate 120 degree if your mouse drag covers half of the screen

	commonViewingParams.ChangeAngle(diffY/xNormalizer, diffX/yNormalizer, 0);
	updateGL();

	//if(parallel_rotate && neighbor){
		//neighbor->volRenderer.RotateObject(anglex, angley, anglez ) ;
		//neighbor->volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity 		
	//}

	//volRenderer.RotateObject(anglex, angley, anglez ) ;
	//volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity 
}


 void Tensor1::MouseMoveToZoom(int diffX, int diffY){
 	
	 if(abs(diffY) >= this->projector->vSourceLocation.Z())
		this->projector->vSourceLocation.VectorZ += diffY/2 ; 

	 this->projector->vSourceLocation.VectorZ += diffY ; 
	//volRenderer.ChangeProjectorLocation(diffY) ; 
	//volRenderer.SetProjectionResolution(true, true) ;			// Fastpass and intensity 
	updateGL();
}

 void Tensor1::processHits (GLint hits, GLuint buffer[])
{

   unsigned int i, j;
   int names, *ptr;

   float fMinMin = 10.0f, fMaxMin= 10.0f ;		// since the z value should be in the range 0 to 1
   int iMinZWinner=-1, iMaxZWinner=-1 ;
   SelectedObjectCatagory1 ObjCat = NO_OBJECT1 ;

   //printf ("hits = %d\n", hits);
   ptr = (int *) buffer;

   for (i = 0; i < (unsigned)hits; i++) {	/*  for each hit  */
      names = *ptr;
      //printf(" number of names for hit = %d\n", names); 
	  ptr++;
	  if( names != 2 ){
		printf("We have problem here.\n") ;
		exit(1) ;
	  }
	  float z1 = (float) *ptr/0x7fffffff ; ptr++;
	  float z2 = (float) *ptr/0x7fffffff; ptr++;
	  int iIndex ;

      //printf(" z1 is %g\n", z1 ); 
      //printf(" z2 is %g\n", z2 ) ; 
		  	
	  int oc = *ptr ; ptr++ ;
	  iIndex = *ptr ; ptr++ ;

	  if( oc == SEEDPOINT1 ){		// SEEDPOINT1 HAS HIGHER PRIORITY
		  ObjCat = (SelectedObjectCatagory1)oc ;
		  iMinZWinner = iIndex ;
		  printf("seedpoint selected.\n") ;
	  }								//if( oc == SEEDPOINT1 )
	  else{
		  if( z1 < fMinMin ){
			fMinMin = z1 ;
			iMinZWinner = iIndex ;
			ObjCat = (SelectedObjectCatagory1)oc ;
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
// single object selection. Catagory of the object will be set in oc. Right now we have just one catagory. streamline
// and name of the object will be set in on
#define BUFSIZE					512			// buffer to process mouse click hits
 void Tensor1::SelectObjectAtXY(int x, int y){
	
	int hits;
	int viewport[4];
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
	float fPixelSize = projector->fDetectorPixelSize ;
/*	glFrustum (	-1.0 * ImageWidth/2 * fPixelSize, 
				1.0 * ImageWidth/2 * fPixelSize, 
				-1.0 * ImageHeight/2 * fPixelSize, 
				1.0 * ImageHeight/2 * fPixelSize, 
				projector->fSourceToDetector, 15000);*/
			
	glMatrixMode (GL_MODELVIEW);
	glPushName(STREAMLINE1);						// signature of a streamline		
	streamlines->SelectModeStreamlinesDraw() ; 
	glPopName() ;									// pop streamline signature

	glPushName(SEEDPOINT1);							// signature of a streamline		
	streamlines->SelectModeSeedPointDraw() ; 
	glPopName() ;									// pop streamline signature

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	//glFlush ();

	hits = glRenderMode (GL_RENDER);
	if( hits > 0 )		
		processHits (hits, selectBuf);	
	else{		// no hit
		ObjectCatagory = NO_OBJECT1 ;
	}
}

 void Tensor1::keyPressEvent(QKeyEvent *event){

	//makeCurrent();
	int temp ;
	switch (event->key()) {
		case 'e':				// explore mode
		case 'E':				// move, rotate, zoom
			printf("Entering EXPLORE_MODE.\n") ;
			CurrIM = ExploreMode1 ;
			break;
		case 's':
		case 'S':
			printf("Entering SELECT_MODE.\n") ;
			CurrIM = SelectMode1 ;
			break ;
		case '1':													// enable/disable volume rendering
			//if (volRenderer.bVisible) 
			//	volRenderer.bVisible = false ;
			//else
			//	volRenderer.bVisible = true ;
			//updateGL();					
			break ;		  
		case '2':													// enable/disable vector field view			
			//if( vectorField.bVisible )
			//	vectorField.bVisible = false ;
			//else
			//	vectorField.bVisible = true ;
			updateGL();					
			break ;								  
		case '3':													// enable/disable streamlines
			if(streamlines->bVisible)
				streamlines->bVisible  = false ;		
			else
				streamlines->bVisible  = true ;
			updateGL();					
			break ;

		// Switch source for glyph from velocity field to 
		// eigenvector field or vice versa

		case '4':	
			vfSource = (vfSource+1)%NUM_OF_SOURCES ;	
			switch(vfSource){
				case velocityFieldSource1:
					//vectorField.Configure(velocityField, slices, rows, cols) ;			
					break ;
				case eigenvectorFieldSource1:
					//vectorField.Configure(eigenVectorField, slices, rows, cols) ;			
					break ;
				default:
					printf("Invalid source status.\n") ;
					break ;
			}			
			updateGL();										
			break ;
		case 'x':
		case 'X':
			if( CurrIM == SelectMode1 && ObjectCatagory == SEEDPOINT1 ){
				streamlines->SetSeedPointMoveDir(0);
			}
			else{
				vectorField->ToggleSliceVisibilityX() ; 
				updateGL();					
			}
			break ;		

		case 'y':
		case 'Y':
			if( CurrIM == SelectMode1 && ObjectCatagory == SEEDPOINT1 ){
				streamlines->SetSeedPointMoveDir(1);
			}
			else{
				vectorField->ToggleSliceVisibilityY() ; 
				updateGL();						
			}
			break ;

		case 'z':
		case 'Z':
			if( CurrIM == SelectMode1 && ObjectCatagory == SEEDPOINT1 ){
				streamlines->SetSeedPointMoveDir(2);
			}
			else{
				vectorField->ToggleSliceVisibilityZ() ; 
				updateGL();				
			}
			break ;

		case '+':
			vectorField->IncreaseSliceNumber() ; 
			updateGL();				
			break ;
		case '-':
			vectorField->DecreaseSliceNumber() ; 
			updateGL();			
			break ;
		case 'f':
		case 'F':
			if( tensorDataSet && tensorDataSet->bDataLoaded )
				emit setRightWindow(this);
			break;
		
		case 'd':
		case 'D':
			if( tensorDataSet && tensorDataSet->bDataLoaded )
				emit setLeftWindow(this);
			break;

		case 'p':
		case 'P':
				parallel_rotate = !parallel_rotate;
				break;
			
   }

 }

 void Tensor1::mouseMoveEvent(QMouseEvent *event)
 {
	makeCurrent();

     if( CurrIM == ExploreMode1){
		if(LeftButtonDown == 1 ){				// rotate the volume
			MouseMoveToRotate(event->x()-buttonDownX, event->y()-buttonDownY) ;
			buttonDownX = event->x() ;
			buttonDownY = event->y() ;
		}
		else if( RightButtonDown == 1){			// move the camera, zoom in or out
			MouseMoveToZoom(event->x()-buttonDownX, event->y()-buttonDownY) ;
			buttonDownX = event->x() ;
			buttonDownY = event->y() ;	
			updateGL();
		}
	} //if( InteractionMode1 == ExploreMode1)
	else if( CurrIM == SelectMode1){
		if(LeftButtonDown == 1 ){				// temporary streamline selection or seedpoint move
			if( streamlines->bSeedpointSelected && streamlines->iSeedpointMoveDir != -1 ){// seedpoint was already selected
				streamlines->MoveSeedPoint(event->y()-buttonDownY) ;
				buttonDownX = event->x() ;
				buttonDownY = event->y() ;
				updateGL() ;
			}
			else{
				SelectObjectAtXY(event->x(), event->y());
				switch(ObjectCatagory){
					case STREAMLINE1:
						//printf("ObjectName %d\n",iObjectName) ;
						streamlines->bSeedpointSelected = false ;
						streamlines->TemporarySelectedStreamline(iObjectName) ;
						updateGL() ;
						break ;
					case SEEDPOINT1:
						if(!streamlines->bSeedpointSelected){		// user just moved in to seed point
							buttonDownX = event->x() ;
							buttonDownY = event->y() ;
							streamlines->bSeedpointSelected = true ;
							updateGL() ;
						}
						break ;
					case NO_OBJECT1:
						streamlines->TempSelectedStreamline = -1 ;
						streamlines->bSeedpointSelected = false ;
						updateGL() ;
						break ;
					default:
						printf("Unidentified object.\n") ;
						break ;
				}  //switch(iObjectCatagory){	
			} // else
		} //if(LeftButtonDown == 1 ){			
	} //else if( CurrIM == SelectMode1){
 }


 void Tensor1::DrawAxisAndBox(){
   // draw the axis lines
  
 //// DRAWING THE BOUNDING BOX	
	 float boxLenX = tensorDataSet->vSpacing.VectorX ;
	 float boxLenY = tensorDataSet->vSpacing.VectorY ;
	 float boxLenZ = tensorDataSet->vSpacing.VectorZ ;

	 int slices = tensorDataSet->ipVolSize.z;
	 int rows = tensorDataSet->ipVolSize.y;
	 int cols = tensorDataSet->ipVolSize.x;  

   glColor3f(0.2, 0.2, 0.2) ;
   glBegin(GL_LINE_LOOP) ;
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;	
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;	
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;	
   glEnd() ;

   glBegin(GL_LINE_LOOP) ;
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;	
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;	
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;	
   glEnd() ;

   glBegin(GL_LINES);
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
   glEnd() ;
}

 void Tensor1::UpdateRenderers(Tensor1 *srcTensor){
	this->commonViewingParams = srcTensor->commonViewingParams;
	this->vectorField = srcTensor->vectorField ;
	//this->streamlines = srcTensor->streamlines ;

	makeCurrent();
	streamlines->SetVectorField(srcTensor->tensorDataSet->velocityField,
								srcTensor->tensorDataSet->ipVolSize.z,
								srcTensor->tensorDataSet->ipVolSize.y,
								srcTensor->tensorDataSet->ipVolSize.x) ;	

	streamlines->Configure( srcTensor->tensorDataSet->vSeedPoint, 
							srcTensor->tensorDataSet->iNumberOfStreamlines) ; 

	streamlines->SetSpacing( srcTensor->tensorDataSet->vSpacing.VectorX, 
								srcTensor->tensorDataSet->vSpacing.VectorY,
								srcTensor->tensorDataSet->vSpacing.VectorZ) ; 

	if( streamlines->bReadyToProcess ){							// generate streamlines and save them in display lists
		streamlines->CreateStreamlines() ;
		
	}
	else{
		printf("We have problem. streamlines can't be generated.\n") ;
		exit(1) ;
	}
	streamlines->bVisible = true ;

 } 

void Tensor1::UpdateRenderers(){
	//all the processing for the current context
	makeCurrent();
	// configure the vector field
	ConfigureVectorField(); 
	// Configure the streamline rederer
	ConfigureStreamline();
	//ConfigureVolumeRendere
	ConfigureVolRenderer();
 } 

void Tensor1::ConfigureVolRenderer(){
	//this->volRenderer->
}

 void Tensor1::ConfigureStreamline(){
	streamlines->SetVectorField(tensorDataSet->velocityField, tensorDataSet->ipVolSize.z,
		tensorDataSet->ipVolSize.y,
		tensorDataSet->ipVolSize.x) ;	
	streamlines->Configure( tensorDataSet->vSeedPoint, tensorDataSet->iNumberOfStreamlines) ; 
	streamlines->SetSpacing( tensorDataSet->vSpacing.VectorX, 
								tensorDataSet->vSpacing.VectorY,
								tensorDataSet->vSpacing.VectorZ) ; 
	if( streamlines->bReadyToProcess ){							// generate streamlines and save them in display lists
		streamlines->CreateStreamlines() ;
		
	}
	else{
		printf("We have problem. streamlines can't be generated.\n") ;
		exit(1) ;
	}
	streamlines->bVisible = true ;
 }

void Tensor1::ConfigureVectorField(){
	vectorField->Configure( tensorDataSet->eigenVectorField, 
							tensorDataSet->eigenValues, // this is new
							tensorDataSet->ipVolSize,
							tensorDataSet->vSpacing); 
	

	vectorField->bVisible = true ; 
} 


