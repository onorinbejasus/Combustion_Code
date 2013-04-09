// Timothy Luciani
// Assignment 3
// Transformer

#include <math.h>
#include <glut/glut.h>

#define HOUSE    0
#define CUBE     1
#define TESCUBE  2
#define CYLINDER 3
#define SPHERE   4
#define TORUS    5
#define MESH     6
#define GEODESIC 7
#define CONE	 8
#define LINE	 9
#define SUPER	10

typedef struct{
	
	GLfloat verts[50][50][4];
	int type, rs, vs, color;
}Shape;

typedef struct  {
	
	GLfloat Translation[4][4];
	GLfloat xRotation[4][4];
	GLfloat yRotation[4][4];
	GLfloat zRotation[4][4];
	GLfloat Scaling[4][4];
	GLfloat ArbitraryAxis[4][4];
	
	int tX, tY, tZ;
	GLfloat sX, sY, sZ;
	GLfloat xDeg, yDeg, zDeg;
	GLfloat deg;
	
}Transformation;

Shape theLine;

void make_shape(Shape *vert); // makes the shape

void make_cylinder(Shape *vert, double height, double ray); // creates the cylinder

void make_sphere(Shape *vert, double ray); // creates the sphere

void make_cone(Shape *vert, double height, double ray); // creates the cone

void make_torus(Shape *vert, double bigR, double littleR); // creates the torus

void make_line(Shape *vert, GLfloat p0[3], GLfloat p1[3]); // makes line

void make_house(Shape *vert); // makes house

void make_quadric(Shape *vert, float l1, float l2, float l3);
