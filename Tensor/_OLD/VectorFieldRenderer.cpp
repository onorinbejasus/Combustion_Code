/* written by Abedul Haque, December 2009		*/
/* for CS3610 class project						*/

//#include "VBO.h"
#include "VectorFieldRenderer.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <float.h>


/* vector field without eigen values */

void VectorFieldRenderer::Configure( Vector*** vf,iPoint volSize, Vector vSpacing ){
	this->VectorField = vf ;

	this->iSlices = volSize.z ;
	this->iRows = volSize.y ;
	this->iCols = volSize.x ;

	this->boxLenX = vSpacing.VectorX ;
	this->boxLenY = vSpacing.VectorY ;
	this->boxLenZ = vSpacing.VectorZ ;
} 

/* vector field with eigen values */

void VectorFieldRenderer::Configure( Vector*** vf, Vector ***ev, iPoint volSize, Vector vSpacing ){
	
	this->VectorField = vf ;
	this->EigenValues = ev;

	this->iSlices = volSize.z ;
	this->iRows = volSize.y ;
	this->iCols = volSize.x ;

	this->boxLenX = vSpacing.VectorX ;
	this->boxLenY = vSpacing.VectorY ;
	this->boxLenZ = vSpacing.VectorZ ;
}

void VectorFieldRenderer::drawQuadric(GLfloat verts[50][50][4], int rs, int vs){

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0,0.0,0.0);
	for(int i = 0; i < rs; i++)
		for(int j = 0; j < vs; j++){
	

			glVertex4fv(verts[i][j]);
			glVertex4fv(verts[i][j+1]);
			glVertex4fv(verts[i+1][j]);
			glVertex4fv(verts[i+1][j+1]);
		}

	glEnd();
}

void VectorFieldRenderer::DrawSuperQuadric(Vector e){

	/* draw the superquadrics along the stream lines */

	float denom = e.X() + e.Y() + e.Z(); // common denmoinator
	float cL = ( e.X() - e.Y() )/denom; // linear
	float cP = ( 2.0 * (e.Y() - e.Z() ) )/denom; // planar
	float cS = ( 3.0 * e.Z() )/denom; // spherical

	float alpha = 0.0f;
	float beta = 0.0f;
	float gamma = 2.0f;

	float theta = 0.0f;
	float phi = 0.0f;

	int radialS = 10;
	int verticalS = 10;

	GLfloat verts[50][50][4]; 

	if(cL >= cP){ // if more linear in shape

		alpha = pow( static_cast<float>(1.0-cP), gamma);
		beta = pow( static_cast<float>(1.0-cL), gamma);

		for(int i = 0; i <= radialS; i++){
			theta = (2.0 * MY_PI * i)/radialS;			
			for(int j = 0; j <= verticalS; j++){

				phi = (MY_PI * j)/verticalS;

			verts[i][j][0] = _copysign( 1.0, cos(phi) ) * pow( fabs( cos(phi) ), beta);
			verts[i][j][1] = -_copysign( 1.0, sin(theta) ) * pow( fabs( sin(theta) ), alpha ) *
						_copysign( 1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta );
			verts[i][j][2] = _copysign( 1.0, cos(theta) ) * pow( fabs( cos(theta) ), alpha ) *
						_copysign( 1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta );
			verts[i][j][3] = 1.0;
			
			} // end j

		} // end i

	drawQuadric(verts, radialS, verticalS);

	}else if(cL < cP){
		
		alpha = pow( static_cast<float>(1.0-cL), gamma);
		beta = pow( static_cast<float>(1.0-cP), gamma);

		for(int i = 0; i <= radialS; i++){
			theta = (2.0 * MY_PI * i)/radialS;			
			for(int j = 0; j <= verticalS; j++){

				phi = (MY_PI * j)/verticalS;

				verts[i][j][0] = _copysign( 1.0, cos(theta) ) * pow( fabs( cos(theta) ), alpha ) *
						_copysign( 1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta );
				verts[i][j][1] = _copysign( 1.0, sin(theta) ) * pow( fabs( sin(theta) ), alpha ) *
						_copysign( 1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta );
				verts[i][j][2] = _copysign( 1.0, cos(phi) ) * pow( fabs( cos(phi) ), beta);
				verts[i][j][3] = 1.0;

			} // end j

		} // end i

		drawQuadric(verts, radialS, verticalS);

	} // end else
}

void VectorFieldRenderer::DrawGlyph(){

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
void VectorFieldRenderer::DrawAlignedGlyph(Vector v, Vector e){

	GLfloat angle1, angle2, temp ;
	glPushMatrix();
	

	if( v.VectorZ == 0 && v.VectorX == 0 )
		angle1 = 0 ;
	else{
		
		angle1 = atan2(v.VectorX,v.VectorZ) ;
		angle1 = 180.0*angle1/MY_PI ;
		if( angle1 < 0 )
			angle1 += 360.0 ;
	}

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

//	glScalef(2.0, 2.0, 2.0);
//	glRotatef(angle1,0,1,0) ;
//	glRotatef(-1*angle2,1,0,0) ;

//	this->DrawGlyph() ; 
//	this->DrawSuperQuadric(e);

	glPopMatrix() ;
}

/*void VectorFieldRenderer::setSpacing(float x, float y, float z){
	this->boxLenX = x ;
	this->boxLenY = y ;
	this->boxLenZ = z ;
}*/
	 
void VectorFieldRenderer::RenderStreamlines(StreamlineRenderer *streamline){

	GLfloat posX, posY, posZ;
	Vector dirV;
	Vector eigV;
	Vector strmpt;
//	GLuint vbo = 0;

//	GLuint shader = compileShader(vertexShader, spherePixelShader);

//	std::ofstream file;
//	file.open("values.txt", std::ios::out);

	int x, y, z;

	// position array
	GLfloat *verts = (GLfloat*)malloc(streamline->getSize() * 4.0 * sizeof(float));

	for(int i = 0; i < streamline->getSize(); i++){

		strmpt = streamline->getStreamlinePoint(i); // get the streamline point
		
		/* get the x, y, and z positions of the streamline point */
		x = verts[i*4+0] = (int)ceil(strmpt.X());
		y = verts[i*4+1] = (int)ceil(strmpt.Y());
		z = verts[i*4+2] = (int)ceil(strmpt.Z());
		verts[i*4+3] = 1.0;

		dirV = this->VectorField[z][y][x]  ;
		eigV = this->EigenValues[z][y][x] ;

//		file << eigV.X() << "," <<  eigV.Y() << "," << eigV.Z() << "\n"; 

		dirV.Normalize() ; 
		posX = x*boxLenX + boxLenX/2 ;
		posY = y*boxLenY + boxLenY/2 ;
		posZ = z*boxLenZ + boxLenZ/2 ;

		glPushMatrix() ;
	//	glScalef(2.0, 2.0, 2.0);
		glTranslatef(posX,posY,posZ) ;
		this->DrawSuperQuadric(eigV);				
		glPopMatrix() ; 
	} // end for i

//	create_VBO(vbo, streamline->getSize(), verts);
//	draw_VBO(vbo, shader, streamline->getSize());

//	file.close();
}

void VectorFieldRenderer::RenderSlice(){
	int i , j , k , index;
	GLfloat posX, posY, posZ ;
	Vector dirV ;
	Vector eigV;


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

				dirV = this->VectorField[k][i][j]  ;
				eigV = this->EigenValues[k][i][j] ;
				if( ( dirV.VectorX == dirV.VectorY) && ( dirV.VectorY == dirV.VectorZ ) && ( dirV.VectorZ == 0.0)  ){
				/*	posX = j*boxLenX + boxLenX/2 ;
					posY = i*boxLenY + boxLenY/2 ;
					posZ = k*boxLenZ + boxLenZ/2 ;

					glPushMatrix() ;
					glTranslatef(posX,posY,posZ) ;
					glColor3f(0.4, 0.4, 0.4) ;
					glutSolidSphere(2, 10, 10) ;
					glPopMatrix() ; */
					continue ;				
				} 
				
				dirV.Normalize() ; 
				posX = j*boxLenX + boxLenX/2 ;
				posY = i*boxLenY + boxLenY/2 ;
				posZ = k*boxLenZ + boxLenZ/2 ;

				glPushMatrix() ;
				glTranslatef(posX,posY,posZ) ;
				DrawAlignedGlyph(dirV, eigV) ;				
				glPopMatrix() ;
			} //for ( j = 0 ; j < this->iCols ; j++)
		} //for(i = 0 ; i < this->iRows  ; i++)
	} //if( this->bSliceZVisible )
	if( this->bSliceYVisible ){
		
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
			/*		posX = j*boxLenX + boxLenX/2 ;
					posY = i*boxLenY + boxLenY/2 ;
					posZ = k*boxLenZ + boxLenZ/2 ;
					glPushMatrix() ;
					glTranslatef(posX,posY,posZ) ;
					glColor3f(0.4, 0.4, 0.4) ;
					glutSolidSphere(2, 10, 10) ;
					glPopMatrix() ; */
					continue ;
				} 

				dirV.Normalize() ; 
				posX = j*boxLenX + boxLenX/2 ;
				posY = i*boxLenY + boxLenY/2 ;
				posZ = k*boxLenZ + boxLenZ/2 ;

				glPushMatrix() ;
				glTranslatef(posX,posY,posZ) ;
				DrawAlignedGlyph(dirV, eigV) ;				
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
				/*	posX = j*boxLenX + boxLenX/2 ;
					posY = i*boxLenY + boxLenY/2 ;
					posZ = k*boxLenZ + boxLenZ/2 ;
					glPushMatrix() ;
					glTranslatef(posX,posY,posZ) ;
					glColor3f(0.4, 0.4, 0.4) ;
					glutSolidSphere(2, 10, 10) ;
					glPopMatrix() ; */ 
					continue ;
				} 

				dirV.Normalize() ; 
				posX = j*boxLenX + boxLenX/2 ;
				posY = i*boxLenY + boxLenY/2 ;
				posZ = k*boxLenZ + boxLenZ/2 ;

				glPushMatrix() ;
				glTranslatef(posX,posY,posZ) ;
				
				DrawAlignedGlyph(dirV,eigV) ;				
				glPopMatrix() ;
			} //for ( i = 0 ; i < this->iRows ; i++)
		} //for(k = 0 ; k < this->iSlices ; k++)		
	} //else if( this->bSliceXVisible )
}


void VectorFieldRenderer::IncreaseSliceNumber(){
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

void VectorFieldRenderer::DecreaseSliceNumber(){
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

void VectorFieldRenderer::ToggleSliceVisibilityX(){
	if( this->bSliceXVisible  )
		this->bSliceXVisible   = false ;		  
	else{
		this->bSliceXVisible  = true ;		  
	}
} 

void VectorFieldRenderer::ToggleSliceVisibilityY(){
	if( this->bSliceYVisible  )
		this->bSliceYVisible   = false ;		  
	else{
		this->bSliceYVisible  = true ;		  
	}
} 

void VectorFieldRenderer::ToggleSliceVisibilityZ(){
	if( this->bSliceZVisible  )
		this->bSliceZVisible   = false ;		  
	else{
		this->bSliceZVisible  = true ;		  
	}
} 