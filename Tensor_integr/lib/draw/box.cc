#include "../open_gl.hh"
#include <stdio.h>

extern int slices;
extern int rows;
extern int cols;

void DrawAxisAndBox(double boxLenX, double boxLenY, double boxLenZ){

printf("start DrawAxisAndBox\n"); fflush(stdout);
   //// DRAWING THE BOUNDING BOX	
   glColor3f(0.2, 0.2, 0.2) ;
   glBegin(GL_LINE_LOOP) ;
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;	
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;	
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;	
   glEnd() ;

   glBegin(GL_LINE_LOOP) ;
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;	
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;	
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;	
   glEnd() ;

   glBegin(GL_LINES);
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(-boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, boxLenZ*slices/2.0, 1) ;		
	glVertex4f(-boxLenX*cols/2.0, -boxLenY*rows/2.0, -boxLenZ*slices/2.0, 1) ;		
   glEnd() ;
printf("end DrawAxisAndBox\n"); fflush(stdout);
}