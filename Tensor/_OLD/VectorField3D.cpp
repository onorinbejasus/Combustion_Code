/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/

#include "VectorField3D.h"

void VectorField3D::Configure( Vector*** vf, int k, int i, int j){
	this->VectorField = vf ;
	this->iSlices = k ;
	this->iRows = i ;
	this->iCols = j ;
} 

void VectorField3D::DrawGlyph(){

	/*GLfloat angle = 0;
	GLfloat tip = 6.0 ;
	GLfloat rx, rz, prx, prz;
	GLfloat ry = 0.7*tip ;
	GLfloat radius = 1.0 ;

	GLfloat angleIncr = 45 ;

	glBegin(GL_LINES);
		glColor3f(1.0,1.0,1.0) ;
		glVertex3f(0,0,0) ;
		glVertex3f(0,tip,0) ;
	glEnd() ;

	prx = radius*cos(MY_PI*angle/180) ;
	prz = radius*sin(MY_PI*angle/180) ;
	angle += angleIncr ;
	glColor3f(0.9,0.6,0.6) ;
	while( angle <= 360){
		rx = radius*cos(MY_PI*angle/180) ;
		rz = radius*sin(MY_PI*angle/180) ;		

		glBegin(GL_LINE_LOOP) ;
			glVertex3f(prx, ry, prz) ;
			glVertex3f(rx, ry, rz) ;
			glVertex3f(0,tip,0) ;
		glEnd() ;

		prx = rx ;
		prz = rz ;
		angle += angleIncr ;
	} */

	GLfloat angle = 0;
	GLfloat tip = 10.0 ;
	GLfloat rx, ry, prx, pry;
	GLfloat rz = 0.7*tip ;
	GLfloat radius = 1.0 ;

	GLfloat angleIncr = 45 ;

	glBegin(GL_LINES);
		glColor3f(1.0,1.0,1.0) ;
		glVertex3f(0,0,0) ;
		glVertex3f(0,0,tip) ;
	glEnd() ;

	prx = radius*cos(MY_PI*angle/180) ;
	pry = radius*sin(MY_PI*angle/180) ;
	angle += angleIncr ;
	glColor3f(0.9,0.6,0.6) ;
	while( angle <= 360){
		rx = radius*cos(MY_PI*angle/180) ;
		ry = radius*sin(MY_PI*angle/180) ;		

		glBegin(GL_LINE_LOOP) ;
			glVertex3f(prx, pry, rz) ;
			glVertex3f(rx, ry, rz) ;
			glVertex3f(0,0,tip) ;
		glEnd() ;

		prx = rx ;
		pry = ry ;
		angle += angleIncr ;
	}	

}


void VectorField3D::DrawAlignedGlyph(Vector v){
	/*GLfloat angle1, angle2, temp ;
	glPushMatrix();
	
	//if( v.x != 0 )
	
	if(v.VectorZ == 0 && v.VectorX == 0 )
		angle1 = 0 ;
	else{
		angle1 = atan(v.VectorZ/v.VectorX) ;
		angle1 = 180*angle1/MY_PI ;
	}
	//else
	//printf("%f %f %f\n",v.x, v.y, v.z) ;

	temp = sqrt(v.VectorZ*v.VectorZ + v.VectorX*v.VectorX) ;
	angle2 = atan(temp/v.VectorY);
	angle2 = 180*angle2/MY_PI ;
	//printf("angles %f %f\n",angle1,angle2) ;

	if( v.VectorX < 0 )
		angle1 += 180 ;

	glRotatef(-1*angle1,0,1,0) ;

	if( v.VectorY < 0 )
		angle2 += 180 ;

	glRotatef(-1*angle2,0,0,1) ;

	this->DrawGlyph() ;
	glPopMatrix() ; */
	GLfloat angle1, angle2, temp ;
	glPushMatrix();
	
	//if( v.x != 0 )
	
	if( v.VectorZ == 0 && v.VectorX == 0 )
		angle1 = 0 ;
	else{
		
		angle1 = atan2(v.VectorX,v.VectorZ) ;
		angle1 = 180.0*angle1/MY_PI ;
		if( angle1 < 0 )
			angle1 += 360.0 ;
	}
	//else
	//printf("%f %f %f\n",v.x, v.y, v.z) ;

	temp = sqrt(v.VectorZ*v.VectorZ + v.VectorX*v.VectorX) ;
	if( v.VectorY == 0 && temp == 0 )
		angle2 = 0 ;
	else{
		angle2 = atan2(v.VectorY,temp);
		angle2 = 180.0*angle2/MY_PI ;
		if (angle2 < 0.0 ){
			angle2 += 360.0 ;
		}
	}

//printf("angles %f %f\n",angle1,angle2) ;

	glRotatef(angle1,0,1,0) ;
	glRotatef(-1*angle2,1,0,0) ;

	this->DrawGlyph() ; 
	glPopMatrix() ;

}

void VectorField3D::setSpacing(float x, float y, float z){
	this->boxLenX = x ;
	this->boxLenY = y ;
	this->boxLenZ = z ;
}
	 

void VectorField3D::RenderSlice(){
	int i , j , k , index;
	GLfloat posX, posY, posZ ;
	Vector dirV ;


	if( ! this->bVisible  )			// if not visible, not need to render
		return ;

	if( this->bSliceZVisible ) { 		
		//glPushMatrix() ;
		//glTranslatef(0,0,0.15);
		k = this->iVectorFieldSliceNumberZ ;
		for(i = 0 ; i < this->iRows  ; i++){
			if( i%5 )
				continue ;
			for ( j = 0 ; j < this->iCols ; j++){
				if (j%5)
					continue ;
				index =  k*this->iCols*this->iRows + i * this->iCols + j ;

				dirV = this->VectorField[k][i][j]  ; 
				if( ( dirV.VectorX == dirV.VectorY) && ( dirV.VectorY == dirV.VectorZ ) && ( dirV.VectorZ == 0.0)  ){
					posX = j*boxLenX + boxLenX/2 ;
					posY = i*boxLenY + boxLenY/2 ;
					posZ = k*boxLenZ + boxLenZ/2 ;

					glPushMatrix() ;
					glTranslatef(posX,posY,posZ) ;
					glColor3f(0.4, 0.4, 0.4) ;
					glutSolidSphere(2, 10, 10) ;
					glPopMatrix() ;
					continue ;				
				}
				
				dirV.Normalize() ; 
				posX = j*boxLenX + boxLenX/2 ;
				posY = i*boxLenY + boxLenY/2 ;
				posZ = k*boxLenZ + boxLenZ/2 ;

				glPushMatrix() ;
				glTranslatef(posX,posY,posZ) ;
				DrawAlignedGlyph(dirV) ;				
				glPopMatrix() ;
			} //for ( j = 0 ; j < this->iCols ; j++)
		} //for(i = 0 ; i < this->iRows  ; i++)
	} //if( this->bSliceZVisible )
	if( this->bSliceYVisible ){
		//glPushMatrix();
		//glTranslatef(-0.5,0,0) ;
		i = this->iVectorFieldSliceNumberY;
		for(k = 0 ; k < this->iSlices ; k++){
			if( k%5 )
				continue ;
			for ( j = 0 ; j < this->iCols ; j++){
				if (j%5)
					continue ;
				index =  k*this->iRows*this->iCols + i * this->iCols + j ;

				dirV = this->VectorField[k][i][j] ; 
				
				if( ( dirV.VectorX == dirV.VectorY) && ( dirV.VectorY == dirV.VectorZ ) && ( dirV.VectorZ == 0.0)  ){
					posX = j*boxLenX + boxLenX/2 ;
					posY = i*boxLenY + boxLenY/2 ;
					posZ = k*boxLenZ + boxLenZ/2 ;

					glPushMatrix() ;
					glTranslatef(posX,posY,posZ) ;
					glColor3f(0.4, 0.4, 0.4) ;
					glutSolidSphere(2, 10, 10) ;
					glPopMatrix() ;
					continue ;
				}

				dirV.Normalize() ; 
				posX = j*boxLenX + boxLenX/2 ;
				posY = i*boxLenY + boxLenY/2 ;
				posZ = k*boxLenZ + boxLenZ/2 ;

				glPushMatrix() ;
				glTranslatef(posX,posY,posZ) ;
				DrawAlignedGlyph(dirV) ;				
				glPopMatrix() ;
			} //for ( j = 0 ; j < this->iCols ; j++)
		} //for(k = 0 ; k < this->iSlices ; k++)
	} //else if( this->bSliceYVisible )
	if( this->bSliceXVisible ){
		//glPushMatrix();
		//glTranslatef(-0.5,0,0) ;
		j = this->iVectorFieldSliceNumberX  ;
		for(k = 0 ; k < this->iSlices ; k++){
			if( k%5 )
			continue ;
			for ( i = 0 ; i < this->iRows ; i++){
				if (i%5)
				continue ;
				index =  k*this->iRows*this->iCols + i * this->iCols + j ;

				dirV = this->VectorField[k][i][j]  ; 
					
				if( ( dirV.VectorX == dirV.VectorY) && ( dirV.VectorY == dirV.VectorZ ) && ( dirV.VectorZ == 0.0)  ){
					posX = j*boxLenX + boxLenX/2 ;
					posY = i*boxLenY + boxLenY/2 ;
					posZ = k*boxLenZ + boxLenZ/2 ;
					glPushMatrix() ;
					glTranslatef(posX,posY,posZ) ;
					glColor3f(0.4, 0.4, 0.4) ;
					glutSolidSphere(2, 10, 10) ;
					glPopMatrix() ;
					continue ;
				}

				dirV.Normalize() ; 
				posX = j*boxLenX + boxLenX/2 ;
				posY = i*boxLenY + boxLenY/2 ;
				posZ = k*boxLenZ + boxLenZ/2 ;

				glPushMatrix() ;
				glTranslatef(posX,posY,posZ) ;
				//glBegin(GL_LINES) ;
				//	glColor3f(0.0, 1.0, 0.0);
				//	glVertex3f(0.0, 0.0, 0.0) ;
				//	glVertex3f(dirV.VectorX*2, dirV.VectorY*2, dirV.VectorZ*2) ;
				//glEnd();
				DrawAlignedGlyph(dirV) ;				
				glPopMatrix() ;
			} //for ( i = 0 ; i < this->iRows ; i++)
		} //for(k = 0 ; k < this->iSlices ; k++)		
	} //else if( this->bSliceXVisible )
}


void VectorField3D::IncreaseSliceNumber(){
	/*if( this->bSliceXVisible  ){
		this->iVectorFieldSliceNumberX++ ;
		this->iVectorFieldSliceNumberX = min(this->iVectorFieldSliceNumberX, this->iCols-1 ) ;
	}		  
	else if( this->bSliceYVisible ){
		this->iVectorFieldSliceNumberY++ ;
		this->iVectorFieldSliceNumberY = min(this->iVectorFieldSliceNumberY, this->iRows-1 ) ;
	}
	else if( this->bSliceZVisible ){
		this->iVectorFieldSliceNumberZ++ ;
		this->iVectorFieldSliceNumberZ = min(this->iVectorFieldSliceNumberZ, this->iSlices-1 ) ;
	}*/

	if( this->bSliceXVisible  ){
		this->iVectorFieldSliceNumberX++ ;
		this->iVectorFieldSliceNumberX = min(this->iVectorFieldSliceNumberX, this->iCols-1 ) ;
	}		  
	if( this->bSliceYVisible ){
		this->iVectorFieldSliceNumberY++ ;
		this->iVectorFieldSliceNumberY = min(this->iVectorFieldSliceNumberY, this->iRows-1 ) ;
	}
	if( this->bSliceZVisible ){
		this->iVectorFieldSliceNumberZ++ ;
		this->iVectorFieldSliceNumberZ = min(this->iVectorFieldSliceNumberZ, this->iSlices-1 ) ;
	}
} 

void VectorField3D::DecreaseSliceNumber(){
	/*if( this->bSliceXVisible  ){
		this->iVectorFieldSliceNumberX-- ;
		this->iVectorFieldSliceNumberX = max(this->iVectorFieldSliceNumberX, 0) ;
	}		  
	else if( this->bSliceYVisible ){
		this->iVectorFieldSliceNumberY-- ;
		this->iVectorFieldSliceNumberY = max(this->iVectorFieldSliceNumberY, 0) ;
	}
	else if( this->bSliceZVisible ){
		this->iVectorFieldSliceNumberZ-- ;
		this->iVectorFieldSliceNumberZ = max(this->iVectorFieldSliceNumberZ, 0) ;
	}*/

	if( this->bSliceXVisible  ){
		this->iVectorFieldSliceNumberX-- ;
		this->iVectorFieldSliceNumberX = max(this->iVectorFieldSliceNumberX, 0) ;
	}		  
	if( this->bSliceYVisible ){
		this->iVectorFieldSliceNumberY-- ;
		this->iVectorFieldSliceNumberY = max(this->iVectorFieldSliceNumberY, 0) ;
	}
	if( this->bSliceZVisible ){
		this->iVectorFieldSliceNumberZ-- ;
		this->iVectorFieldSliceNumberZ = max(this->iVectorFieldSliceNumberZ, 0) ;
	}
} 

void VectorField3D::ToggleSliceVisibilityX(){
	if( this->bSliceXVisible  )
		this->bSliceXVisible   = false ;		  
	else{
		this->bSliceXVisible  = true ;		  
		//this->bSliceYVisible  = false ;
		//this->bSliceZVisible  = false ;
	}
} 

void VectorField3D::ToggleSliceVisibilityY(){
	if( this->bSliceYVisible  )
		this->bSliceYVisible   = false ;		  
	else{
		this->bSliceYVisible  = true ;		  
		//this->bSliceXVisible  = false ;
		//this->bSliceZVisible  = false ;
	}
} 

void VectorField3D::ToggleSliceVisibilityZ(){
	if( this->bSliceZVisible  )
		this->bSliceZVisible   = false ;		  
	else{
		this->bSliceZVisible  = true ;		  
		//this->bSliceXVisible  = false ;
		//this->bSliceYVisible  = false ;
	}
} 