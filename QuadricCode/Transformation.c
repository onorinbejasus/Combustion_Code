#include "Transformation.h"

void makeTransformations(){
	
	scaling();
	translation();
	rotation();
	arbitraryAxis(&theLine);
}

void scaling()
{
	
	T.Scaling[0][0] = T.sX;
	T.Scaling[0][1] = 0;
	T.Scaling[0][2] = 0;
	T.Scaling[0][3] = 0;
	T.Scaling[1][0] = 0;
	T.Scaling[1][1] = T.sY;
	T.Scaling[1][2] = 0;
	T.Scaling[1][3] = 0;
	T.Scaling[2][0] = 0;
	T.Scaling[2][1] = 0;
	T.Scaling[2][2] = T.sZ;
	T.Scaling[2][3]	= 0;
	T.Scaling[3][0] = 0;
	T.Scaling[3][1] = 0;
	T.Scaling[3][2] = 0;
	T.Scaling[3][3] = 1;
}
void translation(){
	
	T.Translation[0][0] = 1;
	T.Translation[0][1] = 0;
	T.Translation[0][2] = 0;
	T.Translation[0][3] = T.tX;
	T.Translation[1][0] = 0;
	T.Translation[1][1] = 1;
	T.Translation[1][2] = 0;
	T.Translation[1][3] = T.tY;
	T.Translation[2][0] = 0;
	T.Translation[2][1] = 0;
	T.Translation[2][2] = 1;
	T.Translation[2][3]	= T.tZ;
	T.Translation[3][0] = 0;
	T.Translation[3][1] = 0;
	T.Translation[3][2] = 0;
	T.Translation[3][3] = 1;
}
void rotation(){
	
	
	T.xRotation[0][0] = 1;
	T.xRotation[0][1] = 0;
	T.xRotation[0][2] = 0;
	T.xRotation[0][3] = 0;
	T.xRotation[1][0] = 0;
	T.xRotation[1][1] = cos(T.xDeg);
	T.xRotation[1][2] = -sin(T.xDeg);
	T.xRotation[1][3] = 0;
	T.xRotation[2][0] = 0;
	T.xRotation[2][1] = sin(T.xDeg);
	T.xRotation[2][2] = cos(T.xDeg);
	T.xRotation[2][3] = 0;
	T.xRotation[3][0] = 0;
	T.xRotation[3][1] = 0;
	T.xRotation[3][2] = 0;
	T.xRotation[3][3] = 1;
	
	T.yRotation[0][0] = cos(T.yDeg);
	T.yRotation[0][1] = 0;
	T.yRotation[0][2] = sin(T.yDeg);
	T.yRotation[0][3] = 0;
	T.yRotation[1][0] = 0;
	T.yRotation[1][1] = 1;
	T.yRotation[1][2] = 0;
	T.yRotation[1][3] = 0;
	T.yRotation[2][0] = -sin(T.yDeg);
	T.yRotation[2][1] = 0;
	T.yRotation[2][2] = cos(T.yDeg);
	T.yRotation[2][3] = 0;
	T.yRotation[3][0] = 0;
	T.yRotation[3][1] = 0;
	T.yRotation[3][2] = 0;
	T.yRotation[3][3] = 1;
	
	T.zRotation[0][0] = cos(T.zDeg);
	T.zRotation[0][1] = -sin(T.zDeg);
	T.zRotation[0][2] = 0;
	T.zRotation[0][3] = 0;
	T.zRotation[1][0] = sin(T.zDeg);
	T.zRotation[1][1] = cos(T.zDeg);
	T.zRotation[1][2] = 0;
	T.zRotation[1][3] = 0;
	T.zRotation[2][0] = 0;
	T.zRotation[2][1] = 0;
	T.zRotation[2][2] = 1;
	T.zRotation[2][3] = 0;
	T.zRotation[3][0] = 0;
	T.zRotation[3][1] = 0;
	T.zRotation[3][2] = 0;
	T.zRotation[3][3] = 1;
}

void arbitraryAxis(Shape *theLine){																  
																  
	float tx = theLine->verts[0][0][0]-theLine->verts[1][0][0];
	float ty = theLine->verts[0][0][1]-theLine->verts[1][0][1];
	float tz = theLine->verts[0][0][2]-theLine->verts[1][0][2];
	
	float invveclen = 1.0f/(float)sqrt(tx*tx+ty*ty+tz*tz);
	tx*=invveclen;
	ty*=invveclen;
	tz*=invveclen;														  
	
	T.xDeg = -theLine->verts[1][0][0];
	T.yDeg = -theLine->verts[1][0][1];
	T.zDeg = -theLine->verts[1][0][2];
	
	T.ArbitraryAxis[0][0] = tx*tx+cos(T.deg)*(1-tx*tx);
	T.ArbitraryAxis[0][1] = tx*ty*(1-cos(T.deg))-tz*sin(T.deg);
	T.ArbitraryAxis[0][2] = tz*tx*(1-cos(T.deg))+ty*sin(T.deg);
	T.ArbitraryAxis[0][3] = 0;
	T.ArbitraryAxis[1][0] = tx*ty*(1-cos(T.deg))+tz*sin(T.deg);
	T.ArbitraryAxis[1][1] = ty*ty+cos(T.deg)*(1-ty*ty);
	T.ArbitraryAxis[1][2] = ty*tz*(1-cos(T.deg))-tz*sin(T.deg);
	T.ArbitraryAxis[1][3] = 0;
	T.ArbitraryAxis[2][0] = tz*tx*(1-cos(T.deg))-ty*sin(T.deg);
	T.ArbitraryAxis[2][1] = ty*tz*(1-cos(T.deg))+tx*sin(T.deg);
	T.ArbitraryAxis[2][2] = tz*tz+cos(T.deg)*(1-tz*tz);		
	T.ArbitraryAxis[2][3] = 0;
	T.ArbitraryAxis[3][0] = 0;
	T.ArbitraryAxis[3][1] = 0;
	T.ArbitraryAxis[3][2] = 0;
	T.ArbitraryAxis[3][3] = 1;

	int i, j;
	
	//for(i = 0; i < 4; i ++)
//		for(j = 0; j < 4; j++)
//			printf("%f", T.ArbitraryAxis[i][j]);
	
}

