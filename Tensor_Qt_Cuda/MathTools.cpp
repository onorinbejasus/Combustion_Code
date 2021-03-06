/*
  mathtools.c - useful functions for manipulating vectors and matrices.
  vectors are in the form of VectorMT structures.  Matrices are 3X3 double
  arrays (e.g. m[row][col]).  All parameters are passed as pointers.

  The file "mathtools.h" must be #include'd in any program calling
  these routines.

  Routines:
  
  vangle       - returns the angle between two vectors
  vmag         - returns the magnitude of a vector
  dot          - returns dot product of 2 vectors
  distance     - returns distance between two vector endpoints
  norm         - returns unit vector in the direction of the input vector
  vproj        - calculates the projection of one vector onto another vector
  cross        - vector cross product
  copy_vector  - copies one vector to another
  add_vector   - adds two vectors
  sub_vector   - subtracts one vector from another
  sxvector     - multiplies a vector by a scalar
  matrix_det   - returns determinant of a 3x3 matrix
  transpose    - determines transpose of a 3x3 matrix
  copy_matrix  - copies one matrix to another
  mult_matrix  - multiplies two 3x3 matrices
  matxvec      - premultiplies a vector structure by a 3x3 matrix
  vecxmat      - postmultiplies a vector structure by a 3x3 matrix
  build_vector - creates a vector strucure from a 3-el. array
  strip_vector - creates a 3-el. array from a vector structure
  build_matrix - creates a 3x3 matrix from three vectors
  strip_matrix - extracts 3 column vectors from a matrix
  convert_vector- converts the location of a vector from 1 coordinate system to another
  vproj_value  - project one vector onto another, return + or - mag. of projection depending on direction
  
**********************************************************************/

// #ifndef SuppressStandardHeaders
//#include "stdafx.h"
// #endif

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "Mathtools.hh"


/**********************************************************************/

double vangle(VectorMT* v1, VectorMT* v2)

// VectorMT *v1, *v2;

{
  double angle = 0, cosine,a,b,c;
  if(vmag(v1)== 0.0 || vmag(v2) == 0.0) 
/*    angle=0.00;*/
    printf("zero angle");
  else{
    a=dot(v1,v2);
    b=vmag(v1);
    c=vmag(v2);
    cosine=dot(v1,v2)/vmag(v1)/vmag(v2);
    if(cosine>0.99999) cosine=1.;
    if(cosine<-0.99999) cosine=-1.;

    angle=acos(cosine);
   if(angle<0.0)angle=-angle;
  }
  return (angle);
}

/**********************************************************************/

double vmag(VectorMT* v)

// VectorMT *v;

{
  double vm;
  vm = sqrt(dot(v,v));
  if (vm < 0.0001)
    return (0);
  return (vm);
}

/**********************************************************************/

double dot(VectorMT* v1, VectorMT* v2)

// VectorMT *v1;
// VectorMT *v2;

{
  return (v1->x*v2->x + v1->y*v2->y + v1->z*v2->z);
}

/**********************************************************************/

double distance(VectorMT* v1, VectorMT* v2)

// VectorMT *v1;
// VectorMT *v2;

{
  return (sqrt((v2->x-v1->x)*(v2->x-v1->x) + (v2->y-v1->y)*(v2->y-v1->y) +
	       (v2->z-v1->z)*(v2->z-v1->z)));
}

/**********************************************************************/

void norm(VectorMT* v, VectorMT* normv)

// VectorMT *v;
// VectorMT *normv;

{
  double mag;

  mag = sqrt(dot(v,v));
  if (mag < .0001)
    return;
  normv->x = v->x/mag;
  normv->y = v->y/mag;
  normv->z = v->z/mag;
}

/**********************************************************************/
void vproj(VectorMT* v1, VectorMT* v2, VectorMT* v3)

// VectorMT *v1;
// VectorMT *v2;
// VectorMT *v3;

{
  double theta, b;

  theta=vangle(v1,v2);
  norm(v2,v3);
  b=vmag(v1)*cos(theta);
  v3->x = v3->x*b;
  v3->y = v3->y*b;
  v3->z = v3->z*b;
}

/**********************************************************************/
double vproj_value(VectorMT* v1, VectorMT* v2)

// VectorMT *v1;
// VectorMT *v2;

{
  VectorMT v4;

  vproj(v1,v2,&v4);
  if(dot(v1,v2)>0.0)
    return(vmag(&v4));
  else
    return(-vmag(&v4));
}


/**********************************************************************/

void cross(VectorMT* v1, VectorMT* v2, VectorMT* vc)

// VectorMT *v1;
// VectorMT *v2;
// VectorMT *vc;

{
  vc->x = v1->y*v2->z - v1->z*v2->y;
  vc->y = v1->z*v2->x - v1->x*v2->z;
  vc->z = v1->x*v2->y - v1->y*v2->x;
}


/***********************************************************************

  copy_vector - copies one vector structure to another: vout = v1

*/

void copy_vector(VectorMT* v1, VectorMT* vout)

// VectorMT *v1;
// VectorMT *vout;

{
  vout->x = v1->x;
  vout->y = v1->y;
  vout->z = v1->z;
}

/***********************************************************************

  add_vector - adds two vector structures: vout = v1 + v2

*/

void add_vector(VectorMT* v1, VectorMT* v2, VectorMT* vout)

// VectorMT *v1,*v2;
// VectorMT *vout;

{
  vout->x = v1->x + v2->x;
  vout->y = v1->y + v2->y;
  vout->z = v1->z + v2->z;
}


/***********************************************************************

  sub_vector - subtracts one vector structure from another: vout = v1 - v2

*/

void sub_vector(VectorMT* v1, VectorMT* v2, VectorMT* vout)

// VectorMT *v1,*v2;
// VectorMT *vout;

{
  vout->x = v1->x - v2->x;
  vout->y = v1->y - v2->y;
  vout->z = v1->z - v2->z;
}


/***********************************************************************

  sxvector - multiplies a vector structure by a scalar: vout = s*v1

*/

void sxvector(double s, VectorMT* v, VectorMT* vout)

// double s;
// VectorMT *v;
// VectorMT *vout;

{
  vout->x = s*v->x;
  vout->y = s*v->y;
  vout->z = s*v->z;
}


/***********************************************************************

  transpose - determines transpose of a 3x3 matrix

*/

void transpose(double m[3][3], double mout[3][3])

// double m[3][3];
// double mout[3][3];

{
  int i, j;

  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      mout[i][j] = m[j][i];
}





/***********************************************************************

  matrix_det - returns the determinant of a 3x3 matrix

*/

double matrix_det(double a[3][3])

// double a[3][3];

{
  return(a[0][0]*a[1][1]*a[2][2] + a[0][1]*a[1][2]*a[2][0] +
	 a[0][2]*a[1][0]*a[2][1] - a[0][0]*a[1][2]*a[2][1] -
	 a[0][1]*a[1][0]*a[2][2] - a[0][2]*a[1][1]*a[2][0]);
}

/***************************************************************************

  mult_matrix - multiplies two 3x3 matrices: mprod = m1xm2

*/

void mult_matrix(double m1[3][3], double m2[3][3], double mprod[3][3])

// double m1[3][3];
// double m2[3][3];
// double mprod[3][3];

{
  // int i;
  int nrow, ncol;

  for (nrow=0; nrow<3; nrow++)
    for (ncol=0; ncol<3; ncol++)
      mprod[nrow][ncol] = m1[nrow][0]*m2[0][ncol] +
	                  m1[nrow][1]*m2[1][ncol] +
	                  m1[nrow][2]*m2[2][ncol];
}

/***************************************************************************

  copy_matrix - copies contents of one 3x3 matrix into another: mout = m1

*/

void copy_matrix(double m1[3][3], double mout[3][3])

// double m1[3][3];
// double mout[3][3];

{
  int nrow, ncol;

  for (nrow=0; nrow<3; nrow++)
    for (ncol=0; ncol<3; ncol++)
      mout[nrow][ncol] = m1[nrow][ncol];
}


/***************************************************************************

  matxvec - premultiplies a 3-el. vector structure by a 3x3 matrix
            vout = m x v  (v, vout are assumed to be column vectors)

*/

void matxvec(double m[3][3], VectorMT *v, VectorMT *vout)

// VectorMT *v;
// VectorMT *vout;
// double m[3][3];

{
  vout->x = v->x*m[0][0] + v->y*m[0][1] + v->z*m[0][2];
  vout->y = v->x*m[1][0] + v->y*m[1][1] + v->z*m[1][2];
  vout->z = v->x*m[2][0] + v->y*m[2][1] + v->z*m[2][2];
}


/***************************************************************************

  vecxmat - postmultiplies a 3-el. vector structure by a 3x3 matrix
            vout = v x m  (v, vout are assumed to be row vectors)

	    note: this is the same as vout = transpose(m) x transpose(v)

*/

void vecxmat(double m[3][3], VectorMT *v, VectorMT *vout)

// VectorMT *v;
// VectorMT *vout;
// double m[3][3];

{
  vout->x = v->x*m[0][0] + v->y*m[1][0] + v->z*m[2][0];
  vout->y = v->x*m[0][1] + v->y*m[1][1] + v->z*m[2][1];
  vout->z = v->x*m[0][2] + v->y*m[1][2] + v->z*m[2][2];
}

/***************************************************************************/


/***************************************************************************

  vec2xmat - use this function when you know the location of a 3D point (or vector) in
             a coordinate system and the coordinate system changes.
             Input the point location relative to the coordinate system (v)
             and the orthogonal coordinate system.  The location of the point 
             in the new coordinate system is output.

             -- --     -- --     --       --
             | x |     | x |     | 00 01 02 |
       vout  | y | = v | y | x m | 10 11 12 |
             | z |     | z |     | 20 21 22 |
             -- --     -- --     --        --
            
             where v[x,y,z] is the loction of the point in the original coordinate system
             and m[0][0], m[1][0], m[2][0] is the unit vector in the x direction
*/



/*****************************************************/

void vecxmat2(double m[3][3], VectorMT *v, VectorMT *vout)

// VectorMT *v;
// VectorMT *vout;
// double m[3][3];

{
  vout->x = v->x*m[0][0] + v->y*m[0][1] + v->z*m[0][2];
  vout->y = v->x*m[1][0] + v->y*m[1][1] + v->z*m[1][2];
  vout->z = v->x*m[2][0] + v->y*m[2][1] + v->z*m[2][2];
}


/***************************************************************************

  build_vector - loads values from v[3] into a vector structure

*/

void build_vector(double v[3], VectorMT *vout)

// double v[3];
// VectorMT *vout;

{
  vout->x = v[0];
  vout->y = v[1];
  vout->z = v[2];
}

/***************************************************************************

  strip_vector - loads values from  a vector structure into vout[3]

*/

void strip_vector(VectorMT *v, double vout[3])

// double vout[3];
// VectorMT *v;

{
  vout[0] = v->x;
  vout[1] = v->y;
  vout[2] = v->z;
}


/***************************************************************************

  build_matrix - creates a matrix (double 3x3 array) whose columns consist
  of three input vectors in the order supplied.

*/

void build_matrix(VectorMT *v1, VectorMT *v2, VectorMT *v3, double m[3][3])

// VectorMT *v1, *v2, *v3;
// double m[3][3];

{
  m[0][0] = v1->x;
  m[1][0] = v1->y;
  m[2][0] = v1->z;

  m[0][1] = v2->x;
  m[1][1] = v2->y;
  m[2][1] = v2->z;

  m[0][2] = v3->x;
  m[1][2] = v3->y;
  m[2][2] = v3->z;
}

/***************************************************************************

  build_matrix2 - creates a matrix (double 3x3 array) whose rows consist
  of three input vectors in the order supplied.

*/

void build_matrix2(VectorMT *v1, VectorMT *v2, VectorMT *v3, double m[3][3])

// VectorMT *v1, *v2, *v3;
// double m[3][3];

{
  m[0][0] = v1->x;
  m[1][0] = v2->x;
  m[2][0] = v3->x;

  m[0][1] = v1->y;
  m[1][1] = v2->y;
  m[2][1] = v3->y;

  m[0][2] = v1->z;
  m[1][2] = v2->z;
  m[2][2] = v3->z;
}



/***************************************************************************

  strip_matrix - creates 3 vectors from the columns of a matrix (double 3x3
  array)

*/

void strip_matrix(VectorMT *v1, VectorMT *v2, VectorMT *v3, double m[3][3])

// VectorMT *v1, *v2, *v3;
// double m[3][3];

{
  v1->x =  m[0][0];
  v1->y =  m[0][1];
  v1->z =  m[0][2];
  
  v2->x = m[1][0];
  v2->y = m[1][1];
  v2->z = m[1][2];
  
  v3->x = m[2][0];
  v3->y = m[2][1];
  v3->z = m[2][2];
}

void strip_matrix2(VectorMT *v1, VectorMT *v2, VectorMT *v3, double m[3][3])

// VectorMT *v1, *v2, *v3;
// double m[3][3];

{
  v1->x =  m[0][0];
  v1->y =  m[1][0];
  v1->z =  m[2][0];
  
  v2->x = m[0][1];
  v2->y = m[1][1];
  v2->z = m[2][1];
  
  v3->x = m[0][2];
  v3->y = m[1][2];
  v3->z = m[2][2];
}

/**********************************************************************

  convert_vector - converts a vector from 1 coordinate system 
                   to another coordiante system

  v1: origin of final CS
  v2: vector to be converted to final CS
  m:  xyz directions of final CS, x-axis column 0, y-axis column 1, z-axis coulmn 2
  vout: v2 in final CS 

*/

void convert_vector(double v1[3], double v2[3], double m[3][3], double vout[3])

// double v1[3], v2[3], vout[3];
// double m[3][3];

{
  int xyz;
  double p[3];
  VectorMT r1, r2;

  for(xyz=0;xyz<3;xyz++)
    p[xyz]=v2[xyz]-v1[xyz];
  build_vector(p,&r1);
  vecxmat(m,&r1,&r2); 
  strip_vector(&r2,vout);
}

/**********************************************************************

  find_centroid - given the 3 vertices of a triangle, 
  finds the centroid of the triangle

  v1: vertex 1
  v2: vertex 2
  v3: vertex 3
  v4: centroid

  algorithm: Find the center of one leg.  
  Centroid is 2/3 of the way from opposite vertex to this center of leg point.

*/

void find_centroid(double v1[3], double v2[3], double v3[3], double v4[3])

// double v1[3], v2[3], v3[3], v4[3];

{
  int xyz;
  double midpoint[3];
  
  for(xyz=0;xyz<3;xyz++)
    midpoint[xyz]=v2[xyz]+0.5*(v3[xyz]-v2[xyz]);
  for(xyz=0;xyz<3;xyz++)
    v4[xyz]=v1[xyz]+0.666666*(midpoint[xyz]-v1[xyz]);
}
/**********************************************************************

  find_triangle_area - given the 3 vertices of a triangle, 
  finds the area of the triangle

  v1: vertex 1
  v2: vertex 2
  v3: vertex 3
  area: area

  algorithm: Sides of triangle are lengths a,b,c.  s is (a+b+c)/2
  Area is sqrt[s*(s-a)(s-b)*(s-c)]

*/

void find_triangle_area(double v1[3], double v2[3], double v3[3], double area)

// double v1[3], v2[3], v3[3];
// double area;

{
  VectorMT r1, r2, r3, vout1, vout2, vout3;

  double a, b, c, s;

  build_vector(v1,&r1);
  build_vector(v2,&r2);
  build_vector(v3,&r3);
  sub_vector(&r1,&r2,&vout1);
  sub_vector(&r2,&r3,&vout2);
  sub_vector(&r1,&r3,&vout3);
  a=vmag(&vout1);
  b=vmag(&vout2);
  c=vmag(&vout3);
  s=(a+b+c)/2.;

  area=sqrt(s*(s-a)*(s-b)*(s-c));
  /*(  printf("%6.4f\n",area);*/



}


/**********************************************************************

  print_matrix - prints out a matrix (double 3x3 array)

*/

void print_matrix(double m[3][3])

// double m[3][3];

{
  int  nrow;

  for(nrow=0;nrow<3;nrow++)
    printf("\n %f, %f, %f",m[nrow][0] ,m[nrow][1] ,m[nrow][2] );
  printf("\n");
}






































