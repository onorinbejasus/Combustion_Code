#pragma once
#include <QtOpenGL/qgl.h>

class CommonParameters{

public:

	float angleX, angleY, angleZ;

public:
	CommonParameters(){
		angleX = angleY = angleZ = 0.0 ;
	}
	void ChangeAngle(float diffX, float diffY, float diffZ){
		angleX += diffX ;
		angleY += diffY ;
		angleZ += diffZ ;
	}
};