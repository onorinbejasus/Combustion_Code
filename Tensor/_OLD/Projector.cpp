#define _USE_MATH_DEFINES
//#include "stdafx.h"
#include "windows.h"
#include "math.h"
#include "assert.h"
#include "Projector.h"
//#include "AllocArray.h"
//#include "emmintrin.h"      // header file for SSE intrinsics
//#include "FatalError.h"

Projector::Projector(void)
{
    // clear out the object initially
    memset(this, 0, sizeof(Projector));	
	bProjChanged = false ;
}

Projector::~Projector(void)
{
	//if(fImage != NULL)
	//	free(fImage) ;
	//if(fRGBImage != NULL)
	//	free(fRGBImage) ;
}


// Configure the x-ray projector in world space
void Projector::Configure(Vector&   XRaySourceLocation, 
                          Vector&   CentralRayOrientation, 
                          float     SourceToDetectorDistance, 
                          iPoint&   DetectorDimensions, 
                          float     DetectorPixelSize,
                          Vector&   CentralRaySpotOnScreen)
{
    // allocate the detector screen buffer
    this->ipImageSize               = DetectorDimensions;
    this->vCR_Image_Intersection    = CentralRaySpotOnScreen;
    // capture the remaining configuration parameters
    this->vSourceLocation           = XRaySourceLocation  ;
	//this->vSourceLocation.ScalarMult(-1) ;									// -1 is to get the negative distance. So that a translation takes you to origin
    this->vCentralRayOrientation    = CentralRayOrientation;
    this->fSourceToDetector         = SourceToDetectorDistance;
    this->vProjectorScale           = Vector(DetectorPixelSize,
                                             DetectorPixelSize,
                                             DetectorPixelSize);
    this->fDetectorPixelSize        = DetectorPixelSize;
    this->vPrincipalPosition        = Vector(0.0f,
                                             0.0f,
                                             -SourceToDetectorDistance / DetectorPixelSize);		// location of detector in projector coordinate
																								// since projector projects towards negative z axis
    // calculate the world-to-Projector transformation
    CalculateTransformations();
    // clear out the performance counters
    //this->iProjectionCount = 0;
    //this->iTotalProjectionTime = 0;
}

// Set the projector screen buffer to all zeros




void Projector::ChangeProjectorLocation(float fDelta){
	bProjChanged = true ;			// we need to do new projection to get updated picture
	vSourceLocation.VectorZ += fDelta  ; 
} 

// Calculates the world-to-projector and projector-to-world transforms from the position and orientation members
void Projector::CalculateTransformations(void)
{
    this->vPrincipalPosition        = Vector(0.0f,
                                             0.0f,
                                             -(this->fSourceToDetector / this->fDetectorPixelSize) ); // since projector projects towards negative z axis

    Transform tScale,tMove,tXrot,tYrot,tZrot;                   // set up the individual transformations
    tScale.Scale(vProjectorScale);
    tZrot.RotateZ(vCentralRayOrientation.Z() * M_PI / 180.0);
    tYrot.RotateY(vCentralRayOrientation.Y() * M_PI / 180.0);
    tXrot.RotateX(vCentralRayOrientation.X() * M_PI / 180.0);
    tMove.Translate(this->vSourceLocation );
    tProjector2World = tScale;                                  // apply the transforms in sequence
    tProjector2World.LeftMult(tZrot);
    tProjector2World.LeftMult(tYrot);
    tProjector2World.LeftMult(tXrot);
    tProjector2World.LeftMult(tMove);
    tWorld2Projector.Invert(tProjector2World);                  // invert the transform for the reverse map
}


void Projector::printProjector(void) {
	printf("Projector location %s\n",this->vSourceLocation.Format()) ;
	printf("Projector central ray orientation %s\n",this->vCentralRayOrientation.Format() ) ;
	printf("Projector VCR_Image_Intersection %s\n",this->vCR_Image_Intersection.Format()  ) ;
	printf("Projector scale %s\n",this->vProjectorScale.Format()  ) ;
	printf("Source to Detector %f\n",this->fSourceToDetector) ;
	printf("Detector pixel size %f\n", this->fDetectorPixelSize) ;
	printf("Projector principal position %s\n",this->vPrincipalPosition.Format()  ) ;
	printf("Transform matrix for world to projector\n") ;
	this->tWorld2Projector.Print() ;
}
