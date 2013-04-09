/* Taken from MTW project by Abedul Haque, December 2009		*/
/* This file has been modified almost 90%						*/
/* The original file was much more bigger and complicated		*/
/* I Kept what was necessary for volume rendering settings.		*/
/* Comments may not seem appropriate since I didn't change 'em	*/
/* used for CS3610 class project								*/

#include "Parameters.h"
#include "direct.h"
#include "FatalError.h"
#include "atltime.h"

Parameters::Parameters(void)
{
    // wipe the control block to all zeros
    memset(this,0,sizeof(Parameters));
    // initialize default values
    int i,j;
    CtFileVoxelSpacing      = Vector(0,0,0);    // initialize the standard Vector objects to (0,0,0)
    CtStartingPosition      = Vector(0,0,0);
    CtStartingOrientation   = Vector(0,0,0);

    RedSourceLoc            = Vector(0,0,0);
    RedOrientation          = Vector(0,0,0);
    RedCentralSpotOnScreen  = Vector(0,0,0);

    FastPassSamplingInterval= MTW_Default_FastPassSamplingInterval;
}

Parameters::~Parameters(void)
{
}

// Read in all operational parameters from the Parameters.csv file
void Parameters::LoadParameters(char* szpFile, char *szp)
{
    char szWork[MAX_PATH * 10]; // utility string buffer

    // open the parameter file
    strcpy(this->szFilePath, szpFile);   // file name from the command line
    FILE* fpParm = NULL;
    if (strlen(szFilePath) > 0) fpParm = fopen(szFilePath,"rt");
    char szLine[MAX_PATH];
    if (fpParm == NULL)
    {
        // we couldn't find the file -- ask the operator for a file path using the common controls file dialog
		FATAL_ERROR("Unable to continue without a parameter file.");
    }
    // read and process each line
#define IsKeyWord(x) (_stricmp(szpKeyWord,x) == 0)
    for (int iLine=2; iLine<1000; iLine++)
    {
        if (fgets(szLine,MAX_PATH,fpParm) == NULL)  break;  // end of file
        char* szpKeyWord = strtok(szLine,ParameterSeparators);
        if (szpKeyWord == NULL) continue;   // skip blank lines
        _strupr(szpKeyWord);
        // isolate the parameters on the line
        char* szpParm1 = strtok(NULL,ParameterSeparators);
        char* szpParm2 = strtok(NULL,ParameterSeparators);
        char* szpParm3 = strtok(NULL,ParameterSeparators);
		char* szpParm4 = strtok(NULL,ParameterSeparators);

        if (IsKeyWord("KeyWord") || IsKeyWord("Header"))
        {   
            continue;   // this line is just a comment
        }
        if (IsKeyWord("CtFileName"))
        {   
            strcpy(CtFileName,szpParm1);
			strcpy(CtFileName,szp);
			strcat(CtFileName, szpParm1);			
			printf("\n\nFile name: %s\n\n", CtFileName);
        }
        else if (IsKeyWord("CtFileSize"))
        {   
                CtFileSize = iPoint(atoi(szpParm1),atoi(szpParm2),atoi(szpParm3));
        }
        else if (IsKeyWord("CtFileVoxelSpacing"))
        {   
            CtFileVoxelSpacing = Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
        }
        else if (IsKeyWord("CtSamplingInterval"))
        {   
            CtSamplingInterval = (float)atof(szpParm1);
        }
        else if (IsKeyWord("RedSourceLoc"))
        {   
            RedSourceLoc = Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
        }
        else if (IsKeyWord("RedOrientation"))
        {   
            RedOrientation = Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
        }
        else if (IsKeyWord("RedSourceToDetector"))
        {   
            RedSourceToDetector = (float)atof(szpParm1);
        }
        else if (IsKeyWord("RedPixelSize"))
        {   
            // we only accept the pixel size parameter if it was not encoded in the movie file
            if (RedPixelSize == 0) RedPixelSize = (float)atof(szpParm1);
        }
        else if (IsKeyWord("RedCentralSpotOnScreen"))
        {   
            RedCentralSpotOnScreen = Vector(atof(szpParm1),atof(szpParm2),0.0);
        }
        else if (IsKeyWord("RedMovieFileSize"))
        {   
                RedMovieFileSize = iPoint(atoi(szpParm1),atoi(szpParm2),atoi(szpParm3));
        }
        else if (IsKeyWord("FastPassSamplingInterval"))
        {   
            FastPassSamplingInterval = atoi(szpParm1);
        }
		else if(IsKeyWord("IntensityTranFunc")){
			this->opTranFunc.addPoint(atof(szpParm1), atof(szpParm2)) ;
		}
		else if(IsKeyWord("ColorTranFunc")){
			this->colTranFunc.addPoint(atof(szpParm1), atof(szpParm2), atof(szpParm3), atof(szpParm4)) ;
		}
        else if (IsKeyWord("SeedPoint"))
        {   
			SeedPointLoc = Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
        }
        else
        {
            sprintf(szWork,
                    "Unable to interpret keyword '%s' in line %d of the parameter file\n%s",
                    szpKeyWord,
                    iLine,
                    szFilePath);
            FATAL_ERROR(szWork); 
        }
    }
    // close the file and quit
    fclose(fpParm);

    // check the pixel size
    if (this->RedPixelSize   == 0)  FATAL_ERROR("You must enter the pixel size for the incoming fluoroscope files.");
 }

