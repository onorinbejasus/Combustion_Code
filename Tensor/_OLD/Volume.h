// Volume.h

//Volume class contains the 3D volume for volume rendering
//It has function for reading raw file given the size
//It also holds the transformation i.e. rotation and translation 
//information for the volume

#pragma once

#include "Vector.h"
#include "Transform.h"
#include "Point.h"
#include "assert.h"


#ifndef sqr
#define sqr(x)                   ((x) * (x))                               // a simple square macro
#endif
#ifndef QUIT
#define QUIT {printf("\n***** Fatal error on line %d of '%s'\n",__LINE__,__FILE__);assert(0);exit(999);} // fatal-error macro
#endif

//#define NOALLOC(x) {printf("\n***** Unable to allocate enough memory for %s\n",x);QUIT;}

typedef unsigned short int u_short;
typedef unsigned char      u_char;

class Volume
{
// public member variables
public:
    __declspec(align(16))
    Vector vMinNonzeroPlanes;   // indices of nonzero planes closest to usCtVol[0][0][0]
    __declspec(align(16))
    Vector vMaxNonzeroPlanes;   // indices of nonzero planes furthest from usCtVol[0][0][0]
    u_short*** usVol;         // this is a pointer to a standard AllocArray3D array allocation containing the data
    Vector vVoxelSpacing;       // voxel spacing measured in millimeters (float) after interpolation
    iPoint ipVolSize;           // dimensions of the usCtVol array (integer) after interpolation
    Vector vOrigin;             // location of the origin of the object coordinate system in index system of usCtVol[][][]
    Vector vOrientation;        // object rotations about the X, Y and Z axes of the object coordinate system, in degrees (float)
    char szFileName[MAX_PATH];  // file name of the volume that was loaded.
    iPoint ipFileSize;          // dimensions of the CT image on the file, before interpolation
    Vector vFileVoxelSpacing;   // voxel spacing as stored on the file in millimeters, before interpolation
    Vector vFileOrigin;         // location of the origin of the object coordinate system in file coordinates, before stretching
    Vector vPosition;           // location of the origin of the object coordinate system in world coordinates
    Transform tVolToWorld;    // matrix to transform the indices of usCtVol[][][] to world coordinates
    Transform tWorldToVol;    // matrix to transform world coordinates to the indices of usCtVol[][][]
    //float fRayCastStep;         // sampling interval to use for ray-casting this object; default is file voxel size
    bool bVolumeIsLoaded;     // set to true at the completion of the load process
	bool bObjectRotated ;
// public methods
public:
    Volume(void);             // constructor
    ~Volume(void);            // destructor
    // Calculate the CT-to-world and world-to-CT transform matrices from the object position and orientation
    void ComputeTransforms(void);
    // Sets the rotational origin of the volume to vNewOrigin (in file coordinates)
    void SetVolumeOrigin(Vector& vNewOrigin);
	//Rotate the volume
	void RotateObject(float fAngleX, float fAngleY, float fAngleZ);
    // Load the CT volume from a source file without any stretching.
    void LoadVolume(char*   szInFileName, 
                                 iPoint& ipDimensions, 
                                 Vector& vVoxelSpacingOnFile );
                                 //float   fSamplingInterval);
};
