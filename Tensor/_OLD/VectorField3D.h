/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/

#pragma once

#include "Vector.h"
#include <GLUT.h>

#define MY_PI 3.14

class VectorField3D{

public:
	Vector ***VectorField;				// storage for the vector field
	int iSlices ;						// number of planes along z axis or least changed index
	int iRows ;							// number of rows or second lest changed index
	int iCols ;							// third index or number of cols

	float boxLenX ;						// voxel spacing
	float boxLenY ;
	float boxLenZ ;

	// these 3 numbers indicates which slice we are showing or rendering
	int iVectorFieldSliceNumberX ;		// slice number along x axis. 
	int iVectorFieldSliceNumberY ;		// along y
	int iVectorFieldSliceNumberZ ;		// along z . Initially we show this slice

	bool bSliceXVisible ;
	bool bSliceYVisible ;
	bool bSliceZVisible ;

	bool bVisible ;							// Global switch for visibility of the vector field. 
											// To see a slice through the vector field, this one has to be set to true
											// and appropriate slice number has to be set.
	// methods
	VectorField3D(){
		iVectorFieldSliceNumberX=0;		
		iVectorFieldSliceNumberY=0;		
		iVectorFieldSliceNumberZ=0;		

		bSliceXVisible = false ;
		bSliceYVisible = false ;
		bSliceZVisible = true  ;
		bVisible = false	;		
	} 
	~VectorField3D(){}
	void Configure(Vector*** vf, int k, int i, int j ) ;
	void setSpacing(float x, float y, float z) ;
	void RenderSlice() ;
	void IncreaseSliceNumber();
	void DecreaseSliceNumber() ;
	void ToggleSliceVisibilityX() ;
	void ToggleSliceVisibilityY() ;
	void ToggleSliceVisibilityZ() ;

private:
	void DrawAlignedGlyph(Vector v) ;
	void DrawGlyph(void) ;

};