#pragma once

#include "Vector.h"
#include "Transform.h"
#include "point.h"

class Projector
{
// public member variables
public:
    //char    szProjectorName[MAX_PATH];  // This is just a reference name for the projector object -- used in debugging
    Vector  vSourceLocation;            // the location of the x-ray point source in world coordinates (in mm)
    // The central ray is defined as a vector originating at the x-ray source, perpendicular to the detector screen.
    Vector  vCentralRayOrientation;     // the rotation angles (in degrees) of the central ray vs. world coordinates
    Vector  vCR_Image_Intersection;     // the X-Y screen coordinates where the central ray lands (in screen pixel units)
    Vector  vProjectorScale;            // scale of the projection -- this is the pixel size of the detector screen (in mm).
    float   fSourceToDetector;          // the distance (in mm) from the source to the detector screen along the central ray.
    iPoint  ipImageSize;                // size of detector screen in pixels (for allocating the array)
                                        // note that image coord -Y aligns with Projector coordinate +Z and image +X aligns with Projector +X
    float   fDetectorPixelSize;         // size of square detector pixels (in mm)
    Vector  vPrincipalPosition;         // the location (in screen pixel units) of the central spot -- always (0,S2D,0).
									// You will need to use ColorTransferFunction for that

    Transform   tWorld2Projector;       // matrix to transform world coordinates to Projector coordinates
    Transform   tProjector2World;       // matrix to transform Projector coordinates to world coordinates

	bool bProjChanged;
public:
    Projector(void);
	~Projector(void);
    // Configure the x-ray projector in world space
    void Configure(Vector& XRaySourceLocation, 
                   Vector& CentralRayOrientation, 
                   float   SourceToDetectorDistance, 
                   iPoint& DetectorDimensions, 
                   float   DetectorPixelSize,
                   Vector& CentralRaySpotOnScreen);
    void CalculateTransformations(void);
    //Prepare the interpolation tables for FastPass processing at the specified sampling interval.
    //void CalculateFastPassInterpolationTables(void);
	void printProjector() ;
	//
	void ChangeProjectorLocation(float fDelta);
};
