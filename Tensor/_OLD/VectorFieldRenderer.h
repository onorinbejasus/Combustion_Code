/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/

#pragma once

#include "StreamlineRenderer.h"
#include <GLUT.h>
#include "Vector.h"
#include "point.h"

#define MY_PI 3.14

class VectorFieldRenderer{

public:
	Vector ***VectorField;				// storage for the vector field
	Vector ***EigenValues;				// storage for the eigen values
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
	VectorFieldRenderer(){
		iVectorFieldSliceNumberX=0;		
		iVectorFieldSliceNumberY=0;		
		iVectorFieldSliceNumberZ=0;		

		bSliceXVisible = false ;
		bSliceYVisible = false ;
		bSliceZVisible = true  ;
		bVisible = false	;	

		VectorField = NULL;
	} 
	~VectorFieldRenderer(){}
	void Configure(Vector*** vf, iPoint volSize, Vector vSpacing ) ;
	void Configure( Vector*** vf, Vector ***ev, iPoint volSize, Vector vSpacing );
	//void setSpacing(float x, float y, float z) ;
	void RenderSlice() ;
	void RenderStreamlines(StreamlineRenderer *streamline);
	void IncreaseSliceNumber();
	void DecreaseSliceNumber() ;
	void ToggleSliceVisibilityX() ;
	void ToggleSliceVisibilityY() ;
	void ToggleSliceVisibilityZ() ;

private:
	void DrawAlignedGlyph(Vector v, Vector e) ;
	void DrawSuperQuadric(Vector V);
	void DrawGlyph(void) ;
	void drawQuadric(GLfloat verts[50][50][4], int rs, int vs);
};