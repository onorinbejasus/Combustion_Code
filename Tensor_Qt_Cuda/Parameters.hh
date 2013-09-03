/* Taken from MTW project by Abedul Haque, December 2009		*/
/* This file has been modified almost 90%						*/
/* The original file was much more bigger and complicated		*/
/* I Kept what was necessary for volume rendering settings.		*/
/* Comments may not seem appropriate since I didn't change 'em	*/
/* used for CS3610 class project								*/

#pragma once

#include "point.hh"
#include "vector.hh"
#include "TransferFunction.hh"
#include "ColorTransferFunction.hh"
#include <iostream>

#define ParameterSeparators                 " ,\t/\n;"
class Parameters
{
// member variables
public:
// Volume file parameters
    static char* CtFileName;               			// Volume file to be matched
    static char* szFilePath;
    static iPoint* CtFileSize;                         // Volume file dimensions
    static Vector* CtFileVoxelSpacing;                 // voxel spacing of file
    static Vector* CtStartingPosition;                 // allows the operator to specify a starting position for the CT object at load time
    static Vector* CtStartingOrientation;              // allows the operator to specify a starting orientation for the CT object at load time
    static float CtSamplingInterval;                 // distance between ray-cast samples in millimeters

// red fluoroscope parameters
    static Vector* RedSourceLoc;                       // location of x-ray source in world coordinates (mm)
    static Vector* RedOrientation;                     // central ray orientation vs. world axes (just a made-up configuration)
    static float RedSourceToDetector;                // source-to-detector distance (in mm)
    static float RedPixelSize;                       // detector pixel size (in mm)
    static Vector* RedCentralSpotOnScreen;             // location of central spot in screen coordinates
    static iPoint* RedMovieFileSize;                   // dimensions of the fluoroscope file
                                                // NOTE: additional codes have been created (05-24-2007)

    static TransferFunction* opTranFunc;				// Opacity transfer function of the projector
    static ColorTransferFunction* colTranFunc ;			// color transfer function for the projector

    static fPoint* SeedPointLoc;

#define MTW_Default_FastPassSamplingInterval    8
    static int FastPassSamplingInterval;           // FastPass mode samples the CT DRR at this interval in both X and Y

// member functions
public:
    Parameters(void);
    ~Parameters(void);
    // Read in all operational parameters from the Parameters.csv file
    static void LoadParameters(std::string szpFile);
    // process BOTH of the column-order variables
    void ProcessColumnOrderCommand(void);
    // Write out all operational parameters in a new Parameters*.csv file
    void SaveParameters(void);
};
