/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/

#pragma once

#include"Streamline.h"

Streamline::Streamline(){
	this->bReadyToProcess = false ;		// We are not ready to generate streamlines
	this->bVisible = false	;			// streamlines are not visible			
	TempSelectedStreamline = -1;		// no streamline temporarily selected

	//three selected streamline color
	streamlineColor[0][0] = 0.2 ; streamlineColor[0][1] = 0.3 ;	streamlineColor[0][2] = 0.8 ;	
	streamlineColor[1][0] = 0.3 ; streamlineColor[1][1] = 0.8 ;	streamlineColor[1][2] = 0.4 ;	
	streamlineColor[2][0] = 0.8 ; streamlineColor[2][1] = 0.3 ;	streamlineColor[2][2] = 0.2 ;	
	//unselected streamline
	streamlineColor[3][0] = 0.6 ; streamlineColor[3][1] = 0.6 ;	streamlineColor[3][2] = 0.6 ;	
	//temporary selected streamline
	streamlineColor[4][0] = 0.8 ; streamlineColor[4][1] = 0.8 ;	streamlineColor[4][2] = 0.8 ;		
} 

void Streamline::biLinearInterpolate(Vector& v1, Vector& v2, Vector& v3, Vector& v4, float fDelta1, float fDelta2){
	v1.Interpolate(v2,fDelta1) ;
	v3.Interpolate(v4,fDelta1) ;
	v1.Interpolate(v3,fDelta2) ; 
}

void Streamline::Configure(fPoint& SeedPoint, int n){
	this->fSeedPosX = SeedPoint.x ;								// set the seed point
	this->fSeedPosY = SeedPoint.y ;
	this->fSeedPosZ = SeedPoint.z ;
	this->iNumberOfStreamlines = min(MAX_NUMBER_STREAMLINE, n) ;//# of streamlines needed

	StreamlineDisplayListIndex = glGenLists(this->iNumberOfStreamlines) ;		//Creating display list indices
	if( StreamlineDisplayListIndex == 0 ){
		printf("Can't allocate display list indices.\n") ;
		this->bReadyToProcess = false ;
	}
	else
		this->bReadyToProcess = true ;							//All data has been set. We can generate streamline now

	for( int i = 0 ; i < this->iNumberOfStreamlines ; i++){
		SelectedStreamlines[i] = UNSELECTED ;					// not selected
		streamlineColorIndex[i] = 3 ;							// not selected color
	}
	for(int i = 0 ; i < 3 ; i++ )								
		this->freeColorForSelected[i] = 1 ;						// this color has not been used		
}

void Streamline::SetSpacing(float dx, float dy, float dz){
	this->boxLenX = dx ;
	this->boxLenY = dy ;
	this->boxLenZ = dz ;
} 

void Streamline::DrawSeedPoint(){
	glPushMatrix() ;
	glTranslatef(this->fSeedPosX*this->boxLenX, this->fSeedPosY*this->boxLenY, this->fSeedPosZ*this->boxLenZ) ;   
	//glScalef(this->boxLenX, this->boxLenY, this->boxLenZ );
	if( this->bSeedpointSelected )
		glColor3f(0.5, 0.5, 0.0) ;
	else
		glColor3f(0.0, 0.5, 0.5) ;
	glutSolidSphere(3, 10, 10) ;
	glPopMatrix() ;
} 

// once you delete strealines you have to configure again with all the data
void Streamline::DeleteStreamlines(){
	glDeleteLists(this->StreamlineDisplayListIndex, this->iNumberOfStreamlines) ;	// delete display list
	this->bReadyToProcess = false ;													// must reconfigure to make it true
} 

// returns false if the position is out of bound
bool Streamline::DerivativeAtPosition(Vector CurrPos, Vector& vDerivative){
	int iXMin = floor(CurrPos.VectorX) ;
	int iXMax = ceil (CurrPos.VectorX) ;		
	float fDelta1 = CurrPos.VectorX - (float)iXMin ;

	int iYMin = floor(CurrPos.VectorY) ;
	int iYMax = ceil(CurrPos.VectorY) ;
	float fDelta2 = CurrPos.VectorY - (float)iYMin;

	int iZMin = floor(CurrPos.VectorZ) ;
	int iZMax = ceil(CurrPos.VectorZ) ;
	float fDelta3 = CurrPos.VectorZ - (float)iZMin ;

	if( iXMin < 0 || iXMin >= this->cols  )			// condition check. if your current position is outside
		return false;									// the volume, then don't continue with the line
	if( iXMax < 0 || iXMax >= this->cols )				
		return false;		
	if( iYMin < 0 || iYMax >= this->rows  )			// I am not sure we need all the conditions
		return false;									// may be we can do Min with 0 and Max with ROWS/COLS/SLICES
	if( iYMax < 0 || iYMax >= this->rows )				// but right now I am not taking any chance
		return false;	
	if( iZMin < 0 || iZMin >= this->slices )
		return false;	
	if( iZMax < 0 || iZMax >= this->slices )
		return false;	

	// get four corners of the first XY plane
	//int index = iZMin*ROWS*COLS + iYMin*rows +
	Vector v1 =  vectorField[iZMin][iYMin][iXMin] ;
	Vector v2 =  vectorField[iZMin][iYMin][iXMax] ;
	Vector v3 =  vectorField[iZMin][iYMax][iXMin] ;
	Vector v4 =  vectorField[iZMin][iYMax][iXMax] ;
	biLinearInterpolate(v1, v2, v3, v4, fDelta1, fDelta2) ;		// doing bi linear interpolation in this plane	
																				// result is in v1
	// get four corners of the first XY plane
	//int index = iZMin*ROWS*COLS + iYMin*rows +
	Vector v5 =  vectorField[iZMax][iYMin][iXMin] ;
	Vector v6 =  vectorField[iZMax][iYMin][iXMax] ;
	Vector v7 =  vectorField[iZMax][iYMax][iXMin] ;
	Vector v8 =  vectorField[iZMax][iYMax][iXMax] ;
	biLinearInterpolate(v5, v6, v7, v8, fDelta1, fDelta2) ;		// doing bi linear interpolation in this plane	
																				// result is in v5	
					
	// now interpolate v1 and v5
	v1.Interpolate(v5, fDelta3) ;
	v1.Normalize() ;
	vDerivative.VectorX = v1.VectorX ;
	vDerivative.VectorY = v1.VectorY ;
	vDerivative.VectorZ = v1.VectorZ ;

	return true ;			// return success
} 
// returns false if the position is out of bound
// true if the position is good. Then new postion is set in NextPos vector
bool Streamline::EulerStep(Vector CurrPos, Vector& NextPos, float fStepSize){

	Vector vDerivative(0.0f, 0.0f, 0.0f) ;

	if( !this->DerivativeAtPosition(CurrPos, vDerivative) )			// if the position is out of bound
		return false ;

	vDerivative.ScalarMult(fStepSize) ; 
	NextPos.VectorX = CurrPos.VectorX + vDerivative.VectorX ;
	NextPos.VectorY = CurrPos.VectorY + vDerivative.VectorY ;
	NextPos.VectorZ = CurrPos.VectorZ + vDerivative.VectorZ ;

	return true;
}

bool Streamline::MidPointMethod(Vector CurrPos, Vector& NextPos, float fStepSize){

	Vector vDerivative(0.0f, 0.0f, 0.0f) ;

	if( !this->DerivativeAtPosition(CurrPos, vDerivative) )			// if the position is out of bound
		return false ;

	// now get the midpoint
	// k1 = h*Derivative
	// we are doing fStepSize/2 to get k1/2
	// Midpoint will have X0 + k1/2 which is mid point between the current position 
	// and next position according to euler step
	vDerivative.ScalarMult(fStepSize/2) ; 
	Vector MidPoint(0.0, 0.0, 0.0) ;
	MidPoint.VectorX = CurrPos.VectorX + vDerivative.VectorX ;
	MidPoint.VectorY = CurrPos.VectorY + vDerivative.VectorY ;
	MidPoint.VectorZ = CurrPos.VectorZ + vDerivative.VectorZ ;

	// Now get the derivative at midPoint
	if( !this->DerivativeAtPosition(MidPoint, vDerivative) )			// if the position is out of bound
		return false ;
	// Now find the next position according to this derivative
	vDerivative.ScalarMult(fStepSize) ;									// now take the full step
	NextPos.VectorX = CurrPos.VectorX + vDerivative.VectorX ;
	NextPos.VectorY = CurrPos.VectorY + vDerivative.VectorY ;
	NextPos.VectorZ = CurrPos.VectorZ + vDerivative.VectorZ ;

	return true;
}
bool Streamline::RK4Method(Vector CurrPos, Vector& NextPos, float fStepSize){

	Vector vK1, vK2, vK3, vK4;
	
	// finding K1
	if( !this->DerivativeAtPosition(CurrPos, vK1) )					// if the position is out of bound
		return false ;	
	vK1.ScalarMult(fStepSize) ;										// k1 = h*Derivative
	// now prepare for K2
	Vector vTemp1(vK1) ;											// (k1)/2
	vTemp1.ScalarMult(0.5) ; 	
	Vector HalfK1Point;

	HalfK1Point.VectorX = CurrPos.VectorX + vTemp1.VectorX ;		// HalfK1Point = X0 + K1/2
	HalfK1Point.VectorY = CurrPos.VectorY + vTemp1.VectorY ;
	HalfK1Point.VectorZ = CurrPos.VectorZ + vTemp1.VectorZ ;
	// Now get the derivative at HalfK1Point which will be K2
	if( !this->DerivativeAtPosition(HalfK1Point, vK2) )				// calculating derivative at HalfK1 i.e f(X0+K1/2)
		return false ;
	
	vK2.ScalarMult(fStepSize) ;										// k2 = h*Derivative

	// Now prepare for K3
	Vector vTemp2(vK2) ;											// vTemp = (k2)/2
	vTemp2.ScalarMult(0.5) ; 	
	Vector HalfK2Point;											
	HalfK2Point.VectorX = CurrPos.VectorX + vTemp2.VectorX ;		// HalfK2Point = X0 + K2/2
	HalfK2Point.VectorY = CurrPos.VectorY + vTemp2.VectorY ;
	HalfK2Point.VectorZ = CurrPos.VectorZ + vTemp2.VectorZ ;
	// Now get the derivative at HalfK2Point
	if( !this->DerivativeAtPosition(HalfK2Point, vK3) )				// calculating derivative i.e. f(X0+K2/2)
		return false ;

	vK3.ScalarMult(fStepSize) ;										// k3 = h*Derivative

	// Now prepare for K4
	Vector vTemp3(vK3) ;											// vTemp = (K3), Note, there is no dividing by 2 here
	Vector K3Point;											
	K3Point.VectorX = CurrPos.VectorX + vTemp3.VectorX ;			// K3Point = X0 + K3
	K3Point.VectorY = CurrPos.VectorY + vTemp3.VectorY ;
	K3Point.VectorZ = CurrPos.VectorZ + vTemp3.VectorZ ;
	// Now get the derivative at K3Point
	if( !this->DerivativeAtPosition(K3Point, vK4) )					// calculating derivative i.e. f(X0+K3) 
		return false ;
	vK4.ScalarMult(fStepSize) ;										// k4 = h*Derivative

	// Now find the next position according to all these derivates
	vK1.ScalarMult(1.0/6.0) ;
	vK2.ScalarMult(1.0/3.0) ;
	vK3.ScalarMult(1.0/3.0) ;
	vK4.ScalarMult(1.0/6.0) ;

	NextPos.VectorX = CurrPos.VectorX ;
	NextPos.VectorY = CurrPos.VectorY ;
	NextPos.VectorZ = CurrPos.VectorZ ;

	NextPos.Add(vK1) ;						// NextPos = CurrPos + (1/6)K1
	NextPos.Add(vK2) ;						// NextPos = CurrPos + (1/6)K1 + (1/3)K2
	NextPos.Add(vK3) ;						// NextPos = CurrPos + (1/6)K1 + (1/3)K2 + (1/3)K3 
	NextPos.Add(vK4) ;						// NextPos = CurrPos + (1/6)K1 + (1/3)K2 + (1/3)K3 + (1/6)K4

	return true;
}
void Streamline::CreateStreamlines(){
	//float fCurrPosX, fCurrPosY, fCurrPosZ ;	
	//float fNextPosX, fNextPosY, fNextPosZ ;			
	Vector CurrPos, NextPos ;

	int iStepCount = 0 ;					// integration time step
	float fStepSize = 0.05 ;				// stepsize of each step
	int iMaxStepCount = 0 ;					// just to record the maximum number of steps required

	int ii  = this->StreamlineDisplayListIndex ; // display list index for the first streamline

	for( int k = -2 ; k <= 2 ; k+=2){
		for(int i = -2 ; i <= 2 ; i+=2){
			for(int j = -2 ; j <= 2 ; j+=2){
				glNewList(ii, GL_COMPILE) ;
				CurrPos.VectorX = fSeedPosX + j ;
				CurrPos.VectorY = fSeedPosY + i ;
				CurrPos.VectorZ = fSeedPosZ + k;
				iStepCount = 0 ;
				while( iStepCount < 40000 )			// max of 10000 steps
				{
					// do one euler step
					//if( !this->EulerStep(CurrPos, NextPos, fStepSize) )		// if the EulerStep returns false as a signal
					//	break ;

					// do one MidPoint method step
					//if( !this->MidPointMethod (CurrPos, NextPos, fStepSize*2) )		
					//	break ;
					
					// do one RK4 step
					if( !this->RK4Method (CurrPos, NextPos, fStepSize) )		
						break ;

					glBegin(GL_LINES) ;											// Now, draw the line
						glVertex3f(CurrPos.VectorX*this->boxLenX, CurrPos.VectorY*this->boxLenY, CurrPos.VectorZ*this->boxLenZ) ;
						glVertex3f(NextPos.VectorX*this->boxLenX, NextPos.VectorY*this->boxLenY, NextPos.VectorZ*this->boxLenZ) ;
					glEnd() ;
										
					CurrPos = NextPos ;		// update current					
					iStepCount++ ;				//update stepCount
				} //while( iStepCount < 10000 )
				glEndList() ;
				ii++ ;
				if( iMaxStepCount < iStepCount )
					iMaxStepCount = iStepCount ;
			} //for(int j = -2 ; j <= 2 ; j+=2)
		} //for(int i = -2 ; i <= 2 ; i+=2)
	} //for( int k = -2 ; k <= 2 ; k+=2)

	//printf("Max steps required %d\n",iMaxStepCount) ;
} 

void Streamline::RenderStreamlines(){
	int iDisplayListIndex ;
	if( this->bVisible ){								// if they are visible then draw. Otherwise no need.
		this->DrawSeedPoint() ; 
		for(int i = 0 ; i < this->iNumberOfStreamlines ; i++){
			if( i == this->TempSelectedStreamline )// mouse is hovering on this streamline, color is yellow
				glColor3f(streamlineColor[4][0] , streamlineColor[4][1], streamlineColor[4][2]) ;
			else{									
				int CI = this->streamlineColorIndex[i] ;
				glColor3f(streamlineColor[CI][0] , streamlineColor[CI][1], streamlineColor[CI][2]) ;		// Normal unselected streamlines
			}

			iDisplayListIndex = this->StreamlineDisplayListIndex + i ; 
			//printf("render display list %d\n",iDisplayListIndex) ;
			glCallList(iDisplayListIndex) ;
		}
	}
} 

void Streamline::SelectModeSeedPointDraw(){
	if( !this->bVisible  )
		return ;
	glPushMatrix() ;
	glPushName(1) ;				// object name for seed point
	glTranslatef(this->fSeedPosX*this->boxLenX, this->fSeedPosY*this->boxLenY, this->fSeedPosZ*this->boxLenZ) ;      
	glutSolidSphere(3, 10, 10) ;
	glPopName() ;
	glPopMatrix() ;
} 

void Streamline::SelectModeStreamlinesDraw(){

	int iDisplayListIndex ;
	if( !this->bVisible  )
		return ;
	glPushName(-1) ;					// placeholder for the next drawings
	for(int i = 0 ; i < this->iNumberOfStreamlines ; i++){
		iDisplayListIndex = this->StreamlineDisplayListIndex + i ; 
		glLoadName(iDisplayListIndex) ;					// display list index will work as a name for the object
		//printf("select mode display list %d\n",iDisplayListIndex) ;
		glCallList(iDisplayListIndex) ;
	}
	glPopName() ;
} 

void Streamline::UpdateStreamlineStatus(int iName){
	int iArrayIndex = iName - this->StreamlineDisplayListIndex ;
	if( this->SelectedStreamlines[iArrayIndex]  ){					// toggle status
		this->SelectedStreamlines[iArrayIndex] = UNSELECTED ;		// make it unselected
		int colorIndex = this->streamlineColorIndex[iArrayIndex] ;	// change the color	
		this->freeColorForSelected[colorIndex] = 1 ;				// make the color free 
		this->streamlineColorIndex[iArrayIndex] =  3 ;		// change the color	of the streamline
	}
	else{
		this->SelectedStreamlines[iArrayIndex] = SELECTED ;		
		//find a free color
		int freeColor = 0 ;
		for( int i = 0 ; i < 3 ; i++){
			if( this->freeColorForSelected[i] ){
				freeColor = i ;
				break ;
			}
		}
		this->streamlineColorIndex[iArrayIndex] =  freeColor ;
		this->freeColorForSelected[freeColor] = 0 ;						// this color is no longer free 
	}
} 

void Streamline::TemporarySelectedStreamline( int iName){
	int iArrayIndex = iName - this->StreamlineDisplayListIndex ;
	this->TempSelectedStreamline = iArrayIndex ;
} 
void Streamline::UpdateSeedPoint(){
	this->DeleteStreamlines() ; 
	this->Configure( fPoint(this->fSeedPosX, this->fSeedPosY, this->fSeedPosZ) ,this->iNumberOfStreamlines) ;
	this->CreateStreamlines() ; 
} 


void Streamline::GetSeedPoint(fPoint& fp){
	//fPoint seed(this->fSeedPosX, this->fSeedPosY, this->fSeedPosZ ) ;
	fp.x = this->fSeedPosX ;
	fp.y = this->fSeedPosY ;
	fp.z = this->fSeedPosZ ;	
} 
// change the seed point location but don't update streamlines 
void Streamline::MoveSeedPoint(fPoint& fp){
	this->fSeedPosX = fp.x ;
	this->fSeedPosY = fp.y ;
	this->fSeedPosZ = fp.z ;
	this->bSeedpointMoved = true ;
	printf("%f %f %f\n",this->fSeedPosX, this->fSeedPosY, this->fSeedPosZ ) ;
} 