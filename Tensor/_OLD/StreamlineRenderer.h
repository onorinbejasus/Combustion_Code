/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/

#pragma once
#include <QtOpenGL/qgl.h>
#include"Vector.h"
#include"Point.h"
#include<GLUT.h>
#include <vector>
#include <cmath>

#define MAX_NUMBER_STREAMLINE	50	

#define SELECTED	1 
#define UNSELECTED	0

class StreamlineRenderer
{
	public:
		Vector*** vectorField ;				// vector field on which we are going to calculate StreamlineRenderer
		int slices ;						// number of z-axis slices
		int rows ;							// number of rows
		int cols ;							// number of cols
		int iNumberOfStreamlines ;			// total number of streamlines to be generated
		float fSeedPosX ; 
		float fSeedPosY ; 
		float fSeedPosZ ; 

		float boxLenX ;
		float boxLenY ;
		float boxLenZ ;

		GLuint StreamlineDisplayListIndex ;	// each streamline will be compiled in a display list. The display list index
											// starting point is saved in streamlineDisplayListIndex. So, streamlineDisplayListIndex is
											// is the index for first streamline, streamlineDisplayListIndex+1 is the index for the second
											// streamline and so on.
		GLuint TempSelectedStreamline ;		// this is display list index of he streamline over which left-button 
											// down mouse cursor is hovering. Note, only one streamline can be selected in this way
		GLuint SelectedStreamlines[MAX_NUMBER_STREAMLINE] ;		//permanently selected streamlines. 0 if the streamline is not selected
																// 1 if it is selected.
		bool bReadyToProcess ;				// whether all data is OK and we can start strealine generation
		bool bVisible;						// whether the streamlines are visible or not. This must set to true if you
											// want to render the streamlines
		float streamlineColor[5][3] ;
		int streamlineColorIndex[MAX_NUMBER_STREAMLINE] ;
		int freeColorForSelected[3] ;

		std::vector<Vector*> points; // integrated points

		bool bSeedpointSelected ;
		bool bSeedpointMoved ;

	int iSeedpointMoveDir;	// 0 for x, 1 for y and 2 for z

	StreamlineRenderer(void) ;
    ~StreamlineRenderer(void){}

	void SetVectorField( Vector*** vf, int slices, int rows, int cols){
		this->vectorField = vf ;
		this->slices = slices ;
		this->rows = rows ;
		this->cols = cols ;
	}
	void SetSpacing(float dx, float dy, float dz) ;
	void biLinearInterpolate(Vector& v1, Vector& v2, Vector& v3, Vector& v4, float fDelta1, float fDelta2) ;
	void Configure(Vector vSeedPoint, int n) ;
	void DeleteStreamlines() ;
	void CreateStreamlines() ;
	void RenderStreamlines() ;
	void SelectModeStreamlinesDraw() ;			// draw the stream lines in GL_SELECT mode
	void SelectModeSeedPointDraw() ;			// draw the seedpoint in GL_SELECT mode
	void UpdateStreamlineStatus(int i) ;
	void TemporarySelectedStreamline(int i) ;
	void DrawSeedPoint() ;
	void UpdateSeedPoint() ;
	void MoveSeedPoint(int iDiff) ;
	void GetSeedPoint(Vector& vSeed) ;
	void SetSeedPointMoveDir(int iDir){
		iSeedpointMoveDir = iDir ;
	}
	Vector getStreamlinePoint(int index){
		return *(points[index]);
	}
	int getSize(){
		return points.size();
	}

private:
	bool EulerStep(Vector CurrPos, Vector& NextPos, float fStepSize) ;
	bool DerivativeAtPosition(Vector vCurrPos, Vector& vDerivative) ;
	bool MidPointMethod(Vector CurrPos, Vector& NextPos, float fStepSize) ; 
	bool RK4Method(Vector CurrPos, Vector& NextPos, float fStepSize) ; 
	void getUniquePoints();
};
