#include "TensorData.h"

TensorData::TensorData(){
	velocityField = NULL ;
	eigenVectorField  = NULL ;
	//volumeImg = NULL;
	bDataLoaded = false;
	vSpacing = Vector(0,0,0);
	ipVolSize = iPoint(0,0,0); 
} 

TensorData::~TensorData(){

}

void TensorData::Configure(Parameters *param){
	SetDimension(param->CtFileSize);
	SetSpacing(param->CtFileVoxelSpacing) ;
	strcpy(volFileName, param->CtFileName);
	SetStreamlineInfo(param->SeedPointLoc, 27);
}

void TensorData::readVelocityField(){

	velocityField = (Vector ***)AllocArray3D(ipVolSize.z, ipVolSize.y, ipVolSize.x, sizeof(Vector) ) ;	
	
	int u = 0, v = 0, w = 0; // velocityField iterators

	std::string temp = dirPath;	
	temp.append("velocity.raw");

	FILE* fin = fopen(temp.data(),"rb") ;
	if(fin == NULL){
		printf("Can't open file.\n") ;
		exit(1) ;
	}

	double *velU, *velV, *velW;				// since I am reading component by component
	// The first component of the vector
	velU = (double *) malloc( sizeof(double)*ipVolSize.z*ipVolSize.y*ipVolSize.x);
	if( velU == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	int nCount = fread(velU, sizeof(double),ipVolSize.y*ipVolSize.x*ipVolSize.z, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can'r read u component.\n") ;
		exit(1) ;
	}
	// The second component of the vector
	velV = (double *) malloc( sizeof(double)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velV == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ; 
	}
	nCount = fread(velV, sizeof(double),ipVolSize.y*ipVolSize.x*ipVolSize.z, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can'r read V component.\n") ;
		exit(1) ;
	}

	// The third component of the vector
	velW = (double *) malloc( sizeof(double)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velW == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velW, sizeof(double),ipVolSize.y*ipVolSize.x*ipVolSize.z, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can'r read W component.\n") ;
		exit(1) ;
	}
	fclose(fin) ;
	printf("Reading finished.\n") ;

	// now fill up the velocity vectors.
	static int tupleIndex ;
	for(int k = 0 ; k < ipVolSize.z ; k++){				// plane
		for(int i = 0 ; i < ipVolSize.y ; i++){			// ipVolSize.y
			for(int j = 0 ; j < ipVolSize.x ;  j++){		// ipVolSize.x
				tupleIndex = k*ipVolSize.y*ipVolSize.x + i * ipVolSize.x + j ;
				velocityField[k][i][j] = Vector(velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]) ;
			 }
		 }
	 }

//	std::cout << "u, v, w: " << u << " " << v << " " << w << std::endl; 
	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void TensorData::readEigenvectorField(){
	
	 int u = 0, v = 0, w = 0;

	std::string temp = dirPath;	
	temp.append("eigenVector.raw");
	
	eigenVectorField = (Vector ***)AllocArray3D(ipVolSize.z, ipVolSize.y, ipVolSize.x, sizeof(Vector) ) ;	
	FILE* fin = fopen(temp.data(),"rb") ;
	if(fin == NULL){
		printf("Can't open file.\n") ;
		exit(1) ;
	}
	float *velU, *velV, *velW;							// since I am reading component by component
	// The first component of the vector
	velU = (float *) malloc( sizeof(float)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velU == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	int nCount = fread(velU, sizeof(float),ipVolSize.z*ipVolSize.y*ipVolSize.x, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x){
		printf("can't read u component.\n") ;
		exit(1) ;
	}
	// The second component of the vector
	velV = (float *) malloc( sizeof(float)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velV == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velV, sizeof(float),ipVolSize.z*ipVolSize.y*ipVolSize.x, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can't read V component.\n") ;
		exit(1) ;
	}
	// The third component of the vector
	velW = (float *) malloc( sizeof(float)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velW == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velW, sizeof(float),ipVolSize.z*ipVolSize.y*ipVolSize.x, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can't read W component.\n") ;
		exit(1) ;
	}
	fclose(fin) ;
	printf("Reading finished.\n") ;

	// now fill up the velocity vectors.
	static int tupleIndex ;
	for(int k = 0 ; k < ipVolSize.z ; k++){	// plane
		for(int i = 0 ; i < ipVolSize.y ; i++){			// ipVolSize.y
			for(int j = 0 ; j < ipVolSize.x ;  j++){		// ipVolSize.x
				tupleIndex = k*ipVolSize.y*ipVolSize.x + i * ipVolSize.x + j ;
				eigenVectorField[k][i][j] = Vector(velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]) ;
			 }
		 }
	 }
	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void TensorData::readEigenValues(){
	
	 int u = 0, v = 0, w = 0;

	std::string temp = dirPath;	
	temp.append("eigenValue.raw");
	
	eigenValues = (Vector ***)AllocArray3D(ipVolSize.z, ipVolSize.y, ipVolSize.x, sizeof(Vector) ) ;	
	FILE* fin = fopen(temp.data(),"rb") ;
	if(fin == NULL){
		printf("Can't open file.\n") ;
		exit(1) ;
	}
	float *velU, *velV, *velW;							// since I am reading component by component
	// The first component of the vector
	velU = (float *) malloc( sizeof(float)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velU == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	int nCount = fread(velU, sizeof(float),ipVolSize.z*ipVolSize.y*ipVolSize.x, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x){
		printf("can't read u component.\n") ;
		exit(1) ;
	}
	// The second component of the vector
	velV = (float *) malloc( sizeof(float)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velV == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velV, sizeof(float),ipVolSize.z*ipVolSize.y*ipVolSize.x, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can't read V component.\n") ;
		exit(1) ;
	}
	// The third component of the vector
	velW = (float *) malloc( sizeof(float)*ipVolSize.z*ipVolSize.y*ipVolSize.x) ;
	if( velW == NULL){
		printf("Cannot allocate memory.\n") ;
		exit(1) ;
	}
	nCount = fread(velW, sizeof(float),ipVolSize.z*ipVolSize.y*ipVolSize.x, fin) ;
	if( nCount != ipVolSize.z*ipVolSize.y*ipVolSize.x ){
		printf("can't read W component.\n") ;
		exit(1) ;
	}
	fclose(fin) ;
	printf("Reading finished.\n") ;

	// now fill up the velocity vectors.
	static int tupleIndex ;
	for(int k = 0 ; k < ipVolSize.z ; k++){	// plane
		for(int i = 0 ; i < ipVolSize.y ; i++){			// ipVolSize.y
			for(int j = 0 ; j < ipVolSize.x ;  j++){		// ipVolSize.x
				tupleIndex = k*ipVolSize.y*ipVolSize.x + i * ipVolSize.x + j ;
				eigenValues[k][i][j] = Vector(velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]) ;
			 }
		 }
	 }
	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void TensorData::readData(){
	readVelocityField() ;
	readEigenvectorField() ;
	readEigenValues();
	//volumeImg = new Volume();
	//volumeImg->LoadVolume(volFileName,				// volume filename   
	//					 ipVolSize,					// volume filesize
	//					 vSpacing);					// voxel size                                   	
	bDataLoaded = true ;
}