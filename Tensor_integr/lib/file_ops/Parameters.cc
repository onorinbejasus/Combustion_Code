/* Taken from MTW project by Abedul Haque, December 2009		*/
/* This file has been modified almost 90%						*/
/* The original file was much more bigger and complicated		*/
/* I Kept what was necessary for volume rendering settings.		*/
/* Comments may not seem appropriate since I didn't change 'em	*/
/* used for CS3610 class project								*/

#include "Parameters.hh"

extern int slices;
extern int rows;
extern int cols;

char* Parameters::CtFileName = NULL;
char* Parameters::szFilePath = NULL;
iPoint* Parameters::CtFileSize = new iPoint(0,0,0);
Vector* Parameters::CtFileVoxelSpacing = new Vector(0,0,0);
Vector* Parameters::CtStartingPosition = new Vector(0,0,0);
Vector* Parameters::CtStartingOrientation = new Vector(0,0,0);
float Parameters::CtSamplingInterval = 0;
Vector* Parameters::RedSourceLoc = new Vector(0,0,0);
Vector* Parameters::RedOrientation = new Vector(0,0,0);
float Parameters::RedSourceToDetector = 0;
float Parameters::RedPixelSize = 0;
Vector* Parameters::RedCentralSpotOnScreen = new Vector(0,0,0);
iPoint* Parameters::RedMovieFileSize = new iPoint(0,0,0);
//TransferFunction* Parameters::opTranFunc = NULL;
ColorTransferFunction* Parameters::colTranFunc = NULL;
fPoint* Parameters::SeedPointLoc = new fPoint(0,0,0);
int Parameters::FastPassSamplingInterval = MTW_Default_FastPassSamplingInterval;

Parameters::Parameters(void)
{
    // wipe the control block to all zeros
    memset(this,0,sizeof(Parameters));
    // initialize default values
    //CtFileVoxelSpacing      = new Vector(0,0,0);
    //CtStartingPosition      = new Vector(0,0,0);
    //CtStartingOrientation   = new Vector(0,0,0);

    //RedSourceLoc            = new Vector(0,0,0);
    //RedOrientation          = new Vector(0,0,0);
    //RedCentralSpotOnScreen  = new Vector(0,0,0);

    //FastPassSamplingInterval= MTW_Default_FastPassSamplingInterval;
}

Parameters::~Parameters(void)
{
}

// Read in all operational parameters from the Parameters.csv file
void Parameters::LoadParameters(std::string szpFile)
{
    char szWork[MAX_PATH * 10]; // utility string buffer

    // open the parameter file
//    strcpy(this->szFilePath, szpFile);   // file name from the command line
    FILE* fpParm = fopen(szpFile.c_str(),"rt");
//    if (strlen(szFilePath) > 0) fpParm = fopen(szFilePath,"rt");
    char szLine[MAX_PATH];
    if (fpParm == NULL)
    {
        // we couldn't find the file -- ask the operator for a file path using the common controls file dialog
		FATAL_ERROR("Unable to continue without a parameter file.");
    }

	//opTranFunc = new TransferFunction();
	colTranFunc = new ColorTransferFunction();

    // read and process each line
#define IsKeyWord(x) (strcmp(szpKeyWord,x) == 0)
    for (int iLine=2; iLine<1000; iLine++)
    {
        if (fgets(szLine,MAX_PATH,fpParm) == NULL )  break;  // end of file
        char* szpKeyWord = strtok(szLine,ParameterSeparators);
        if (szpKeyWord == NULL) continue;   // skip blank lines
//        _strupr(szpKeyWord);
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
			std:: cout << szpFile << std::endl;
			int pos = szpFile.find("parameters.csv");
			std::string path = szpFile.substr(0, pos);
			
			path.append("divergence.raw");

			CtFileName = new char[path.size() + 1];
			strncpy(CtFileName, path.c_str(), path.size());
						
        }
        else if (IsKeyWord("CtFileSize"))
        {   
                CtFileSize = new iPoint(atoi(szpParm1),atoi(szpParm2),atoi(szpParm3));
				cols = atoi(szpParm1);
				rows = atoi(szpParm2);
				slices = atoi(szpParm3);
        }
        else if (IsKeyWord("CtFileVoxelSpacing"))
        {   
            CtFileVoxelSpacing = new Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
        }
        else if (IsKeyWord("CtSamplingInterval"))
        {   
            CtSamplingInterval = (float)atof(szpParm1);
        }
        else if (IsKeyWord("RedSourceLoc"))
        {   
            RedSourceLoc = new Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
        }
        else if (IsKeyWord("RedOrientation"))
        {   
            RedOrientation = new Vector(atof(szpParm1),atof(szpParm2),atof(szpParm3));
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
            RedCentralSpotOnScreen = new Vector(atof(szpParm1),atof(szpParm2),0.0);
        }
        else if (IsKeyWord("RedMovieFileSize"))
        {   
                RedMovieFileSize = new iPoint(atoi(szpParm1),atoi(szpParm2),atoi(szpParm3));
        }
        else if (IsKeyWord("FastPassSamplingInterval"))
        {   
            FastPassSamplingInterval = atoi(szpParm1);
        }
		else if(IsKeyWord("IntensityTranFunc")){
			//opTranFunc->addPoint(atof(szpParm1), atof(szpParm2)) ;
		}
		else if(IsKeyWord("ColorTranFunc")){
			colTranFunc->addPoint(atof(szpParm1), atof(szpParm2), atof(szpParm3), atof(szpParm4)) ;
		}
        else if (IsKeyWord("SeedPoint"))
        {   
			SeedPointLoc = new fPoint(atof(szpParm1),atof(szpParm2),atof(szpParm3));
			//printf("%f %f %f\n", SeedPointLoc.x,SeedPointLoc.y,SeedPointLoc.z );
        }
        else
        {
            sprintf(szWork,
                    "Unable to interpret keyword '%s' in line %d of the parameter file\n%s",
                    szpKeyWord,
                    iLine,
                   szpFile.c_str());
            FATAL_ERROR(szWork); 
        }
    }
    // close the file and quit
    fclose(fpParm);

    // check the pixel size
//    if (this->RedPixelSize   == 0)  FATAL_ERROR("You must enter the pixel size for the incoming fluoroscope files.");
 }

