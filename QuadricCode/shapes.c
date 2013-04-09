// Timothy Luciani
// Assignment 3
// Transformer

#include "shapes.h"

extern float cL;
extern float cP;
extern float cS;

void make_cube_smart(Shape *vert, double size)
{ // creates the cube according to the input
	
	size = size/2;
	
	vert->verts[0][0][0] = -size;
	vert->verts[0][0][1] = size;
	vert->verts[0][0][2] = size;
	vert->verts[0][0][3] = 1;
	vert->verts[0][1][0] = size;
	vert->verts[0][1][1] = size;
	vert->verts[0][1][2] = size;
	vert->verts[0][1][3] = 1;
	vert->verts[0][2][0] = size;
	vert->verts[0][2][1] = size;
	vert->verts[0][2][2] = -size;
	vert->verts[0][2][3] = 1;
	vert->verts[0][3][0] = -size;
	vert->verts[0][3][1] = size;
	vert->verts[0][3][2] = -size;
	vert->verts[0][3][3] = 1;
	
	vert->verts[1][0][0] = -size;
	vert->verts[1][0][1] = -size;
	vert->verts[1][0][2] = size;
	vert->verts[1][0][3] = 1;
	vert->verts[1][1][0] = size;
	vert->verts[1][1][1] = -size;
	vert->verts[1][1][2] = size;
	vert->verts[1][1][3] = 1;
	vert->verts[1][2][0] = size;
	vert->verts[1][2][1] = -size;
	vert->verts[1][2][2] = -size;
	vert->verts[1][2][3] = 1;
	vert->verts[1][3][0] = -size;
	vert->verts[1][3][1] = -size;
	vert->verts[1][3][2] = -size;
	vert->verts[1][3][3] = 1;
	
	int i, j;
	
	for(i = 2; i <= vert->vs; i++)
		for(j = 0; j <= vert->rs; j++)
		{
			vert->verts[i][j][0] = 0;
			vert->verts[i][j][1] = 0;
			vert->verts[i][j][2] = 0;
		}
	
	
}

void make_house(Shape *vert) // makes the house
{
	vert->verts[1][0][0] = -1;
	vert->verts[1][0][1] = 1;
	vert->verts[1][0][2] = 1;
	vert->verts[1][0][3] = 1;
	vert->verts[1][1][0] = 1;
	vert->verts[1][1][1] = 1;
	vert->verts[1][1][2] = 1;
	vert->verts[1][1][3] = 1;
	vert->verts[1][2][0] = 1;
	vert->verts[1][2][1] = 1;
	vert->verts[1][2][2] = -1;
	vert->verts[1][2][3] = 1;
	vert->verts[1][3][0] = -1;
	vert->verts[1][3][1] = 1;
	vert->verts[1][3][2] = -1;
	vert->verts[1][3][3] = 1;
	
	vert->verts[2][0][0] = -1;
	vert->verts[2][0][1] = -1;
	vert->verts[2][0][2] = 1;
	vert->verts[2][0][3] = 1;
	vert->verts[2][1][0] = 1;
	vert->verts[2][1][1] = -1;
	vert->verts[2][1][2] = 1;
	vert->verts[2][1][3] = 1;
	vert->verts[2][2][0] = 1;
	vert->verts[2][2][1] = -1;
	vert->verts[2][2][2] = -1;
	vert->verts[2][2][3] = 1;
	vert->verts[2][3][0] = -1;
	vert->verts[2][3][1] = -1;
	vert->verts[2][3][2] = -1;
	vert->verts[2][3][3] = 1;
	
	vert->verts[0][0][0] = 0;
	vert->verts[0][0][1] = 2;
	vert->verts[0][0][2] = 0;
	vert->verts[0][0][3] = 1;
	vert->verts[0][1][0] = 0;
	vert->verts[0][1][1] =  2;
	vert->verts[0][1][2] = 0;
	vert->verts[0][1][3] = 1;
	vert->verts[0][2][0] = 0;
	vert->verts[0][2][1] =  2;
	vert->verts[0][2][2] = 0;
	vert->verts[0][2][3] = 1;
	vert->verts[0][3][0] = 0;
	vert->verts[0][3][1] =  2;
	vert->verts[0][3][2] = 0;
	vert->verts[0][3][3] = 1;
	
	vert->verts[3][0][0] = 1;
	vert->verts[3][0][1] = 1;
	vert->verts[3][0][2] = -1;
	vert->verts[3][0][3] = 1;
	vert->verts[3][1][0] = 1;
	vert->verts[3][1][1] = 1;
	vert->verts[3][1][2] = -1;
	vert->verts[3][1][3] = 1;
	vert->verts[3][2][0] = 0;
	vert->verts[3][2][1] =  2;
	vert->verts[3][2][2] = 0;
	vert->verts[3][2][3] = 1;
	vert->verts[3][3][0] = 1;
	vert->verts[3][3][1] = 1;
	vert->verts[3][3][2] = -1;
	vert->verts[3][3][3] = 1;
}

void make_shape(Shape *vert)
{
	
    double	ray = 1; // radius
	int height = 1; // height
	double torus_radius = 1.2; // radius from origin to center of tube
	double tube_radius = 0.2; // tube radius
	GLfloat p0[3] = {0,0,0};
	GLfloat p1[3] = {0,1,0};
	theLine.type = LINE;
	
	float l1 = 1.0;
	float l2 = 0.75;
	float l3 = 0.50;
	
	switch(vert->type)
	{
		case HOUSE:
			make_house(vert);break;
		case SPHERE:
			make_sphere(vert, ray); break;
		case CYLINDER:
			make_cylinder(vert, ray, height); break;
		case CONE:
			make_cone(vert, height, ray); break;
		case TORUS:
			make_torus(vert,torus_radius, tube_radius); break;
		case CUBE:
			make_cube_smart(vert, 2); break;
		case LINE:
			make_line(vert, p0, p1);
		case SUPER:
			make_quadric(vert, l1, l2, l3);
	}	
}


void make_quadric(Shape *vert, float l1, float l2, float l3){
	
	int i,j,k; // initial variables to use outside of loops
	
	float denom = l1+l2+l3;
	
//	float cL = (l1-l2)/denom;
//	float cP = 2.0 * (l2-l3)/denom;
//	float cS = 3.0 * l3 / denom;
    
	double alpha = 0.0; 
	double beta = 0.0;
	float gamma = 3.0;
	
	for(i = 0; i <= vert->rs; i++) // iterates through the angles
	{
		GLfloat theta = (2 * M_PI * i)/vert->rs; // angle
		
		for(j = 0; j <= vert->vs; j++) // iterates through the layers
		{
			
			GLfloat phi = (M_PI * j)/vert->vs; // angle
			
			if(cL >= cP){
				
				alpha = pow((1.0-cP),gamma);
				beta = pow((1.0-cL), gamma);
                 
				vert->verts[i][j][0] = copysign(1.0, cos(phi) ) * pow(fabs( cos(phi) ), alpha ); 
				vert->verts[i][j][1] = -copysign(1.0, sin(theta) ) * pow( fabs( sin(theta) ), alpha) * 
                                                    copysign(1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta);
				vert->verts[i][j][2] = copysign(1.0, cos(theta) ) * pow( fabs( cos(theta) ), alpha) * 
                                                    copysign(1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta);
				vert->verts[i][j][3] = 1.0;	
			
			}else{
				
				alpha = pow((1.0-cL),gamma);
				beta = pow((1.0-cP), gamma);
				
				vert->verts[i][j][0] = copysign(1.0, cos(theta) ) * pow( fabs( cos(theta) ), alpha) * 
                                                    copysign(1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta);
                vert->verts[i][j][1] = copysign(1.0, sin(theta) ) * pow( fabs( sin(theta) ), alpha) * 
                                                    copysign(1.0, sin(phi) ) * pow( fabs( sin(phi) ), beta);
                vert->verts[i][j][2] = copysign(1.0, cos(phi) ) * pow(fabs( cos(phi) ), alpha ); 
				vert->verts[i][j][3] = 1.0;
				
			}
			
			
		}	
	}
	
}

void make_cylinder(Shape *vert, double height, double ray)
{
	int i,j,k; // initial variables to use outside of loops
	
	for(i = 0; i <= vert->vs; i++) // iterates through the angles
	{
		
		for(j = 0; j <= vert->rs; j++) // iterates through the layers
		{
			GLfloat theta = (2 * M_PI * j)/vert->rs; // angle
			
			for(k = 0; k < 4; k++) // iterates through x,y,z coordinates
			{
				switch(k){
					case 0:
						vert->verts[i][j][k] = ray * cos(theta); break; // x coordinate
					case 1:
						vert->verts[i][j][k] = (height * i/vert->vs) - height/2; break; // y coordinate
					case 2:
						vert->verts[i][j][k] =  ray * sin(theta); break; // z coordinate
					case 3:
						vert->verts[i][j][k] = 1; // homogenious coordinate
				}
			}
		}	
	}
	
}	

void make_sphere(Shape *vert, double ray)
{
	int i,j,k; // initial variables to use outside of loops
		
	for(i = 0; i <= vert->vs; i++) // iterates through the angles
	{
		
		for(j = 0; j <= vert->rs; j++) // iterates through the layers
		{
			
			int t = (360/vert->rs * j); // theta
			int p = (180/vert->vs * i) - 90; // phi
			
			GLfloat theta = (t * M_PI/180); // theta angle
			GLfloat phi = (p * M_PI/180);  // phi angle
			
			for(k = 0; k < 4; k++) // iterates through x,y,z coordinates
			{
				switch(k){
					case 0:
						vert->verts[i][j][k] = ray * cos(phi) * cos(theta); break; // x coordinate
					case 1:
						vert->verts[i][j][k] = ray * sin(phi); break; // y coordinate
					case 2:
						vert->verts[i][j][k] = -ray * cos(phi) * sin(theta); break; // z coordinate
					case 3:
						vert->verts[i][j][k] = 1; // homogenious coordinate
				}
			}
		}	
	}
	
}

void make_cone(Shape *vert, double height, double ray)
{
	int i,j,k; // initial variables to use outside of loops
	
	for(i = 0; i <= vert->vs; i++) // iterates through the angles
	{
		
		for(j = 0; j <= vert->rs; j++) // iterates through the layers
		{
			int t = 360/vert->rs * j; // next angle
			
			GLfloat theta = (t * M_PI/180); // angle
			
			for(k = 0; k < 4; k++) // iterates through x,y,z coordinates
			{
				switch(k){
					case 0:
						vert->verts[i][j][k] = (height - (height/vert->vs * i))/height * ray * cos(theta); break; // x coordinate
					case 1:
						vert->verts[i][j][k] = (height/vert->vs * i) - height/2;break; // y coordinate
					case 2:
						vert->verts[i][j][k] = (height - (height/vert->vs * i))/height * ray * sin(theta); break; // z coordinate
					case 3:
						vert->verts[i][j][k] = 1; // homogenious coordinate
				}
			}
		}	
	}	
}

void make_torus(Shape *vert, double R, double r)
{
	int i,j,k; // initial variables to use outside of loops
	
	
	for(i = 0; i <= vert->rs; i++) // iterates through the angles
	{
		GLfloat theta = (2 * M_PI * i)/vert->rs; // angle
		
		for(j = 0; j <= vert->vs; j++) // iterates through the layers
		{
			
			GLfloat phi = (2 * M_PI * j)/vert->vs; // angle
			
			for(k = 0; k < 4; k++) // iterates through x,y,z coordinates
			{
				switch(k){
					case 0:
						vert->verts[i][j][k] = (R + r * cos(theta)) * cos(phi);break; // x coordinate
					case 1:
						vert->verts[i][j][k] = r * sin(theta);break; // y coordinate
					case 2:
						vert->verts[i][j][k] = -(R + r * cos(theta)) * sin(phi);break; // z coordinate
					case 3:
						vert->verts[i][j][k] = 1; // homogenious coordinate
				}
			}
		}	
	}		
}
void make_line(Shape *vert, GLfloat p0[3], GLfloat p1[3])
{ // makes the arbitrary line
	
	vert->rs = 1;
	vert->vs = 2;
	
	vert->verts[0][0][0] = p1[0];
	vert->verts[0][0][1] = p1[1];
	vert->verts[0][0][2] = p1[2];
	
	vert->verts[1][0][0] = p0[0];
	vert->verts[1][0][1] = p0[1];
	vert->verts[1][0][2] = p0[2];
		
}
