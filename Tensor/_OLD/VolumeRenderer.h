/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/
/* VolumeRenderer class file header				*/

#pragma once

#include "Vector.h"
#include "Transform.h"
#include "Volume.h"
#include "TransferFunction.h"
#include "ColorTransferFunction.h"
#include "CheckForSSE.h"
#include "Projector.h"
#include "Parameters.h"
#include "point.h"
#include <GLUT.h>


#define AccumulateFour(x) ((double)x.m128_f32[0] + (double)x.m128_f32[1] + (double)x.m128_f32[2] + (double)x.m128_f32[3])
#define MaxFour(x) max(max(x.m128_f32[0],x.m128_f32[1]),max(x.m128_f32[2],x.m128_f32[3]))

#define FAST_PASS_SAMPLING 8 

typedef struct FPinterp
{
    float   fpWeight[2][2];             // this is the weight given to each true value in the interpolation
}   FPinterp;

typedef struct ResamplePoint
{
    u_short i;                          // these are the coordinates of a point on the edge of a fastpass DRR that need to be resampled
    u_short j;                          //
}   ResamplePoint;


#define ColorBarWidth	20
#define ColorBarHeight	100

class VolumeRenderer{
public:	

	Volume *volumeImg;							// Pointer to the volume image
												// the image is in the TensorData class
	Projector *projector;						// pointer to the projector 
												// projector is part of thw Tensor1 class

	iPoint  ipImageSize;
    float** fImage;                     // pointer to standard AllocArray2D array.
	float** fRGBImage;

	TransferFunction opacityFunction ;	// Opacity transfer function of the projector
	ColorTransferFunction colorTranFun ;


    Vector  vImageMinScrCoord;          // Bounding rectangle for projection of CT volume on screen
    Vector  vImageMaxScrCoord;          // "
    int     iProjectionCount;           // number of images projected with this object
    int     iLastProjectionTime;        // number of milliseconds to render the last view
    int     iTotalProjectionTime;       // total time spend in RayCast() function (in milliseconds)

	float fMaxImageValue;               // Maximum pixel value in the image
	float fMaxRedValue ;				// for color projection
	float fMaxGreenValue ;
	float fMaxBlueValue ;
    float fMeanImageValue;              // Mean pixel value
    float fStdDevImageValue;            // Pixel value standard deviation

	//bool bRayCastProjChanged;					// Do we need to update the projection image
	bool bVisible;								// Is volume rendering enabled
	bool bIntensity;							// The defult image is intensity image	
	bool bColorBar;
	//bool bObjectRotated ;	
	float fRGBColorBar[ColorBarHeight][ColorBarWidth][4] ;


    enum eSSE
    {
        eSSE_unknown,
        eSSE_SSE2,
        eSSE_nonSSE2
    };
    eSSE SSE_Mode;                      // identifies the mode of processing for SIMD code

    // fast-pass DRR rendering controls (added on 11-24-2007)
    bool    bFastPassMethod;            // if 'true' RayCast() will plot every Nth row and column, then interpolate to get full image
    int     iFastPassSamplingInterval;  // this is the N in the above line; it comes from the parameters file.
    FPinterp*** fpiInterpolation;       // this table controls the interpolation process for the Nth sampling
    ResamplePoint* rppResample;         // this is a list of the points at the edge of the DRR that we need to resample
    int     nResample;                  // this is a count of the above list
    int     iMaxResamplesPossible;      // this is the limit of the resampling process

	
public:

	//methods
	VolumeRenderer() ;	
	VolumeRenderer(int w, int h);
	~VolumeRenderer(){}

	//void Initialize(char* paramFileName, char *szp, int width, int type) ;
	void ConfigureVolRenderer(Volume *vol, Projector *prj){
		volumeImg = vol ;
		projector = prj ;
	}
	//call this method to create volume image by ray casting
	void CreateVolImage() ;
	void RenderVolImage() ;
	//void RotateObject(float fAngleX, float fAngleY, float fAngleZ) ;
	void ChangeProjectorLocation(float fDelta) ;
	void SetProjectionResolution( bool bFastPass, bool bIntensity) ;
	void BuildColorBar() ;
	void ShowColorBar() ;

private:
    // Project the ctvObj onto the fImage buffer of renderer
    void RayCast();
	// RGB image version of the raycast. The RayCast method generates intensity image. This version generates RGB image
	void Normalize() ;
	// But we need to initialize ColorTransferFunction for that
	void ColorRayCast();
	// normalizing the image
	void NormalizeRGB() ;
    // Return the average number of milliseconds used per rendering operation

public:
    int AverageRenderTime(void);
    // Calculates the world-to-projector and projector-to-world transforms from the position and orientation members
	//void Reset(void) ;
	void SetRenderImageSize(iPoint  ipSize);
    // Set the projector screen buffer to all zeros
    void Clear(void);

};