#include "open_gl.hh"

// tensor files
#include "Parameters.hh"
#include <time.h>
#include <fstream>
#include "FatalError.hh"
#include "Streamline.hh"
#include "AllocArray.hh"
#include "VectorFieldRenderer.hh"
#include <iostream>
 
extern int slices; //from glwidget.cpp
extern int rows; //from glwidget.cpp
extern int cols; //from glwidget.cpp

extern Vector ***velocityField;// from glwidget.cpp
extern Vector ***eigenValues; // from glwidget.cpp
extern Vector ***eigenVectors; //from glwidget.cpp

extern std::string parameters; //from glwidget.cpp

void readVelocityField(std::string path){
	
    std::string p = "";
	int pos = path.find("parameters.csv");
	p = path.substr(0, pos);
	p.append("velocity.raw");
		
    velocityField = new Vector**[slices];

    for(int ii = 0; ii < slices; ii++){
        velocityField[ii] = new Vector*[rows];
        for(int jj = 0; jj < rows; jj++)
            velocityField[ii][jj] = new Vector[cols];
    }

    //(Vector ***)AllocArray3D(slices, rows, cols, sizeof(Vector) ) ;
    FILE* fin = fopen(p.c_str(), "rb") ;

    if(fin == NULL){
        printf("Can't open velocity file (%s).\n",p.c_str());
        exit(1) ;
    }

	double *velU, *velV, *velW;							// since I am reading component by component
	// The first component of the vector
	velU = (double *) malloc( sizeof(double)*slices*rows*cols) ;
	if( velU == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	int nCount = fread(velU, sizeof(double),rows*cols*slices, fin) ;
	if( nCount != rows*cols*slices ){
		printf("can'r read u component.\n") ;
		exit(1) ;
	}
	// The second component of the vector
	velV = (double *) malloc( sizeof(double)*slices*rows*cols) ;
	if( velV == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velV, sizeof(double),rows*cols*slices, fin) ;
	if( nCount != rows*cols*slices ){
		printf("can'r read V component.\n") ;
		exit(1) ;
	}

	// The third component of the vector
	velW = (double *) malloc( sizeof(double)*slices*rows*cols) ;
	if( velW == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velW, sizeof(double),rows*cols*slices, fin) ;
	if( nCount != rows*cols*slices ){
		printf("can'r read W component.\n") ;
		exit(1) ;
	}
	fclose(fin) ;
	printf("Reading finished.\n") ;


	// now fill up the velocity vectors.
	static int tupleIndex ;
	for(int k = 0 ; k < slices ; k++){				// plane
		for(int i = 0 ; i < rows ; i++){			// rows
			for(int j = 0 ; j < cols ;  j++){		// cols
				tupleIndex = k*rows*cols + i * cols + j ;
                velocityField[k][i][j] = new Vector(velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]) ;
			 }
		 }
	 }

	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void readEigenvectorField(std::string path){

    std::string p = "";
    int pos = path.find("parameters.csv");
    p = path.substr(0, pos);
    p.append("eigenVector.raw");

    eigenVectors = new Vector**[slices];

    for(int ii = 0; ii < slices;ii++){
        eigenVectors[ii] = new Vector*[rows];
        for(int jj = 0; jj <rows; jj++)
            eigenVectors[ii][jj] = new Vector[cols];
    }

    //eigenVectors = (Vector ***)AllocArray3D(slices, rows, cols, sizeof(Vector) ) ;
    FILE* fin = fopen(p.c_str(), "rb") ;

    if(fin == NULL){
        printf("Can't open eigenvector file (%s).\n",p.c_str());
        exit(1) ;
    }

    float *velU, *velV, *velW;							// since I am reading component by component
	// The first component of the vector
	velU = (float *) malloc( sizeof(float)*slices*rows*cols) ;
	if( velU == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	int nCount = fread(velU, sizeof(float),rows*cols*slices, fin) ;
	if( nCount != rows*cols*slices ){
		printf("can'r read u component.\n") ;
		exit(1) ;
	}
	// The second component of the vector
	velV = (float *) malloc( sizeof(float)*slices*rows*cols) ;
	if( velV == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velV, sizeof(float),rows*cols*slices, fin) ;
	if( nCount != rows*cols*slices ){
		printf("can't read V component.\n") ;
		exit(1) ;
	}

	// The third component of the vector
	velW = (float *) malloc( sizeof(float)*slices*rows*cols) ;
	if( velW == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velW, sizeof(float),rows*cols*slices, fin) ;
	if( nCount != rows*cols*slices ){
		printf("can't read W component.\n") ;
		exit(1) ;
	}
	fclose(fin) ;
	printf("Reading finished.\n") ;

	// now fill up the velocity vectors.
	static int tupleIndex ;
	for(int k = 0 ; k < slices ; k++){				// plane
		for(int i = 0 ; i < rows ; i++){			// rows
			for(int j = 0 ; j < cols ;  j++){		// cols
				tupleIndex = k*rows*cols + i * cols + j ;
                eigenVectors[k][i][j] = new Vector(velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]) ;
			 }
		 }
	 }

	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void readEigenValues(std::string path){
	
	std::string p;
	int pos = path.find("parameters.csv");
	p = path.substr(0, pos);
	p.append("eigenValue.raw");
	
    eigenValues = new Vector**[slices];

    for(int ii = 0; ii < slices;ii++){
        eigenValues[ii] = new Vector*[rows];
        for(int jj = 0; jj < rows; jj++)
            eigenValues[ii][jj] = new Vector[cols];
    }

    //= (Vector ***)AllocArray3D(slices, rows, cols, sizeof(Vector) ) ;
    FILE* fin = fopen(p.c_str(), "rb") ;

    if(fin == NULL){
        printf("Can't open eigenvalue file (%s).\n",p.c_str());
        exit(1) ;
    }
    float *velU, *velV, *velW;							// since I am reading component by component
	// The first component of the vector
	velU = (float *) malloc( sizeof(float)*slices*rows*cols) ;
	if( velU == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	int nCount = fread(velU, sizeof(float),slices*rows*cols, fin) ;
	if( nCount != slices*rows*cols){
		printf("can't read u component.\n") ;
		exit(1) ;
	}
	// The second component of the vector
	velV = (float *) malloc( sizeof(float)*slices*rows*cols) ;
	if( velV == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velV, sizeof(float),slices*rows*cols, fin) ;
	if( nCount != slices*rows*cols ){
		printf("can't read V component.\n") ;
		exit(1) ;
	}
	// The third component of the vector
	velW = (float *) malloc( sizeof(float)*slices*rows*cols) ;
	if( velW == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velW, sizeof(float),slices*rows*cols, fin) ;
	if( nCount != slices*rows*cols ){
		printf("can't read W component.\n") ;
		exit(1) ;
	}
	fclose(fin) ;
	printf("Reading finished.\n") ;

	// now fill up the velocity vectors.
	static int tupleIndex ;
	for(int k = 0 ; k < slices ; k++){	// plane
		for(int i = 0 ; i < rows ; i++){			// ipVolSize.y
			for(int j = 0 ; j < cols ;  j++){		// ipVolSize.x
				tupleIndex = k*rows*cols + i * cols + j ;
				eigenValues[k][i][j] = new Vector(velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]) ;
			}
		 }
	 }
	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void readData(){
	
    printf("%i %i %i\n", rows, cols, slices);

    readVelocityField(parameters) ;
    readEigenValues(parameters) ;
    readEigenvectorField(parameters) ;
}
