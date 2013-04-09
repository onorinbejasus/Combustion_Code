#pragma once
#pragma once

#include "Vector.h"
#include "AllocArray.h"
#include "Point.h"
#include "Parameters.h"
//#include "Volume.h"

#include <iostream>

class TensorData{

public :

	// volume size
	iPoint  ipVolSize;   
	// inter sample spacing
	Vector  vSpacing;
	
	// streamline related paramter
	int iNumberOfStreamlines ;			// total number of streamlines to be generated
	Vector vSeedPoint;

	Vector ***velocityField;							//velocity vectorfield data
	Vector ***eigenVectorField;							//eigenvector field data
	Vector ***eigenValues;								// eigen values
	//Volume *volumeImg;

	char dirPath[MAX_PATH];
	char volFileName[MAX_PATH];
	bool bDataLoaded ;

	TensorData();
	~TensorData();
	void readEigenValues();
	void readVelocityField();
	void readEigenvectorField();
	void readData() ;

	void SetDirPath(char *path){
		strcpy(dirPath, path) ;
	}

	void Configure(Parameters *param);

	void SetDimension(iPoint ipDim){
		ipVolSize = ipDim ;
	}

	void SetSpacing(Vector vSpace){
		vSpacing = vSpace ;
	}

	void SetStreamlineInfo(Vector vSeed, int iCount){
		vSeedPoint = vSeed ;
		iNumberOfStreamlines = iCount;
	}
};