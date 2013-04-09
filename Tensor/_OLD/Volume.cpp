// This class supports the loading and handling of a volume data.
// The data are assumed to be 16-bit, unsigned integers.  The volumes are
// of various dimensions, and are presented without header information (raw binary).
// The voxel spacing is assumed to be the same in the X and Y directions, but
// usually different in the Z direction (slice spacing).

#define _USE_MATH_DEFINES
#include "volume.h"
#include "windows.h"
#include "math.h"
#include "AllocArray.h"
#include "FatalError.h"

Volume::Volume(void)
{
    memset(this,0,sizeof(Volume));
	bObjectRotated = false ;
	bVolumeIsLoaded = false ;
}

Volume::~Volume(void)
{
}

void Volume::RotateObject(float fAngleX, float fAngleY, float fAngleZ){
	bObjectRotated = true ;							// we need to update volume rendering accordingly
	vOrientation = Vector(fAngleX, fAngleY, fAngleZ) ; 
}  

void Volume::ComputeTransforms(void)
{
    // calculate the individual moves and rotates
    Transform tObj2Vol,tScale,tXrot,tYrot,tZrot,tPosition;
    Vector vVol000 = vOrigin;
    vVol000.Negate();
    tObj2Vol.Translate(vVol000);

    tScale.Scale(vVoxelSpacing);
    tXrot.RotateX(vOrientation.X() * (float)(M_PI / 180.0));
    tYrot.RotateY(vOrientation.Y() * (float)(M_PI / 180.0));
    tZrot.RotateZ(vOrientation.Z() * (float)(M_PI / 180.0));
    tPosition.Translate(vPosition);
    // now stack them up to build one transform matrix
    tVolToWorld = tObj2Vol;
    tVolToWorld.LeftMult(tScale);
    tVolToWorld.LeftMult(tZrot);
    tVolToWorld.LeftMult(tYrot);
    tVolToWorld.LeftMult(tXrot);
    tVolToWorld.LeftMult(tPosition);
    // now invert the matrix to get the world-to-CT transform
    tWorldToVol.Invert(tVolToWorld);
	/*
#ifdef _DEBUG
    printf("\nTest transformations from CT volume to world coordinates:\n");
    Vector vCT(vOrigin),vWorld;
    tVolToWorld.RightMult(vCT,vWorld);
    vCT.Print();
    vWorld.Print();
    vCT.VectorX += 1.0;
    tCtVolToWorld.RightMult(vCT,vWorld);
    vCT.Print();
    vWorld.Print();
    vCT.VectorY += 1.0;
    tCtVolToWorld.RightMult(vCT,vWorld);
    vCT.Print();
    vWorld.Print();
    vCT.VectorZ += 1.0;
    tCtVolToWorld.RightMult(vCT,vWorld);
    vCT.Print();
    vWorld.Print();
#endif // _DEBUG
	*/
}

// Sets the rotational origin of the volume to vNewOrigin (in file coordinates)
void Volume::SetVolumeOrigin(Vector& vNewOrigin)
{
    // load the new file origin
    vFileOrigin = vNewOrigin;
    // calculate the interpolated volume origin
    vOrigin = Vector((float)(vFileOrigin.X()),
                     (float)(vFileOrigin.Y()),
                     (float)(vFileOrigin.Z()));
    // report the chante
    printf("Rotational origin of CT file changed to %s file voxels.\n",vFileOrigin.Format());
}

// Load the CT volume from a source file without any stretching.
void Volume::LoadVolume(char*   szInFileName, 
                                       iPoint& ipDimensions, 
                                       Vector& vVoxelSpacingOnFile)
                                       
{
    int i,j,k;     // utility indices
    // copy the parameters into this object
    strcpy(this->szFileName,szInFileName);
    this->ipFileSize        = ipDimensions;
    this->vFileVoxelSpacing = vVoxelSpacingOnFile;
    // allocate the main volume image array
    this->ipVolSize = ipDimensions; // the volume dimensions are the same as the input CT file
    this->usVol   = (u_short***)AllocArray3D(ipVolSize.z,
                                               ipVolSize.y,
                                               ipVolSize.x,
                                               sizeof(u_short));

    {
        // open the input file
        FILE* fpIn = fopen(szFileName,"rb");
        if (fpIn)
        {
            printf("'%s' opened for input\n",szFileName);
        }
        else
        {
            FATAL_ERROR(_strerror(szFileName));
        }
        // read the file one plane at a time and load them in reverse order so as to match the projector geometry
        for (k=0; k<=(ipVolSize.z-1); k++)
        {
            size_t stRead = fread(  &usVol[k][0][0],
                                    sizeof(u_short),
                                    ipVolSize.x * ipVolSize.y,
                                    fpIn);
            if ((int)stRead != (ipVolSize.x * ipVolSize.y))
            {
                // short read -- this is a problem
                FATAL_ERROR("Premature end-of-file on the CT volume");
            }
        }
        fclose(fpIn);
    }   // else// if (! bFileIsTiff) 

	vOrigin = Vector( (float)(ipVolSize.x)/2.0, 
						(float)(ipVolSize.y)/2.0,
						(float)(ipVolSize.z)/2.0) ;
    vMinNonzeroPlanes = Vector(0.0,0.0,0.0);
    vMaxNonzeroPlanes = Vector(ipVolSize.x-1 ,ipVolSize.y-1 ,ipVolSize.z-1);

    // report the origin in terms of the original file voxels
    vFileOrigin = Vector(vOrigin.X(),
                         vOrigin.Y(),
                         ipFileSize.z - vOrigin.Z());
    //printf("Rotational origin of CT file is @ %s in file voxels.\n",vFileOrigin.Format());
    // calculate the voxel spacing in the stored object
    vVoxelSpacing = Vector(vFileVoxelSpacing.X(), /// ipStretch.x,
                           vFileVoxelSpacing.Y(), /// ipStretch.y,
                           vFileVoxelSpacing.Z()); // / ipStretch.z);
    // calculate the default ray-casting step interval in units of the usCtVol[][][] array
    //if (fCtSamplingInterval)
    //{
      //  this->fRayCastStep = fCtSamplingInterval / max(max(vVoxelSpacing.X(),
                                                          // vVoxelSpacing.Y()),
                                                          // vVoxelSpacing.Z());
    //}
    //else
   // {
     //   this->fRayCastStep = (float)min(ipStretch.x, min(ipStretch.y, ipStretch.x));
   // }
    // clear the position and orientation to zero
    vPosition = vOrientation = Vector(0,0,0);
    // mark the volume as loaded
    bVolumeIsLoaded = true;
}
