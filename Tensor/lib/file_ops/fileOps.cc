#include "../open_gl.hh"

// tensor files
#include "Parameters.hh"
#include <time.h>
#include <fstream>
#include "../std/FatalError.hh"
#include "../streamlines/Streamline.hh"
#include "../std/AllocArray.hh"
#include "../glyphs/VectorFieldRenderer.hh"
#include <iostream>
 
extern int slices;
extern int rows;
extern int cols;

extern Vector ***velocityField;// from callbacks.cc
extern Vector *** eigenValues; // from callbacks.cc
extern GLfloat 	*eigenfloats;  //from callbacks.cc
extern Vector 	***eigenVectors; 

extern std::string parameters;

#define MAX_LIGHTS  8
#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))

typedef struct _LITE{
	GLfloat amb[4];
	GLfloat diff[4];
	GLfloat spec[4];
	GLfloat pos[4];
	GLfloat dir[3];
	GLfloat angle;
}LITE;

GLfloat shine;
GLfloat emi[4];
GLfloat amb[4];
GLfloat diff[4];
GLfloat spec[4];

LITE my_lights[MAX_LIGHTS];
int num_objects;
int  num_lights;

void myabort(void) {
	abort();
	exit(1); /* exit so g++ knows we don't return. */
} 

void normalize(GLfloat *p) { 
	double d=0.0;
	int i;
	for(i=0; i<3; i++) d+=p[i]*p[i];
	d=sqrt(d);
	if(d > 0.0) for(i=0; i<3; i++) p[i]/=d;
}

void parse_floats(char *buffer, GLfloat nums[]) {
	int i;
	char *ps;
	
	ps = strtok(buffer, " ");
	for (i=0; ps; i++) {
		nums[i] = atof(ps);
		ps = strtok(NULL, " ");
		//printf("read %f ",nums[i]);
	}
}

void parse_light(char *buffer){
	LITE *pl;
	char *pamb, *pdiff, *pspec, *ppos, *pdir, *pang;
	my_assert ((num_lights < MAX_LIGHTS), "too many lights");
	pl = &my_lights[num_lights++];
	
	strtok(buffer, "()");
	pamb  = strtok(NULL, "()");  strtok(NULL, "()"); 
	pdiff = strtok(NULL, "()");  strtok(NULL, "()"); 
	pspec = strtok(NULL, "()");  strtok(NULL, "()"); 
	ppos  = strtok(NULL, "()");  strtok(NULL, "()"); 
	pdir  = strtok(NULL, "()");  strtok(NULL, "()"); 
	pang  = strtok(NULL, "()");
	
	parse_floats(pamb, pl->amb);
	parse_floats(pdiff, pl->diff);
	parse_floats(pspec, pl->spec);
	parse_floats(ppos, pl->pos);
	if (pdir) {
		parse_floats(pdir, pl->dir);
		pl->angle = atof(pang);
		//printf("angle %f\n", pl->angle);
	}
	else
		pl->dir[0]= pl->dir[1]= pl->dir[2] =0;
	printf("read light\n");
	
}

void parse_settings(char *buffer){
	
	char *pshape, *pshine, *pemi, *pamb, *pdiff, *pspec, *ptranslate, *pscale, *protate;
		
	pshape  = strtok(buffer, " ");
	//printf("pshape is %s\n",pshape);
	
	ptranslate    = strtok(NULL, "()");  strtok(NULL, "()");
	pscale        = strtok(NULL, "()");  strtok(NULL, "()"); 
	protate       = strtok(NULL, "()");  strtok(NULL, "()");  
	
	pshine  = strtok(NULL, "()");strtok(NULL, "()");
	//printf("pshine is %s\n",pshine);
	
	pemi    = strtok(NULL, "()");  strtok(NULL, "()"); 
	pamb    = strtok(NULL, "()");  strtok(NULL, "()");	
	pdiff   = strtok(NULL, "()");  strtok(NULL, "()"); 
	pspec   = strtok(NULL, "()");  strtok(NULL, "()"); 
	
	parse_floats(pemi, emi);
	parse_floats(pamb, amb);
	parse_floats(pdiff, diff);
	parse_floats(pspec, spec);
	
}
	
void read_spec(char *fname) {
	char buffer[300];
	FILE *fp;
	
	fp = fopen(fname, "r");
	my_assert(fp, "can't open spec");
		while(!feof(fp)){
		fgets(buffer, 300, fp);
		//printf("read line: %s\n", buffer);
		switch (buffer[0]) {
			case '#':
				break;
			case '1': //cube
				//read in the cube
				parse_settings(buffer);
				break;	
			case 'l':
				parse_light(buffer);
				break;
				
			default:
				break;
		}
	}
}

void readVelocityField(std::string path){
	
	printf("read velocity \n");	
	
	
	std::string p;
	int pos = path.find("parameters.csv");
	p = path.substr(0, pos);
	p.append("velocity.raw");
		
	velocityField = (Vector ***)AllocArray3D(slices, rows, cols, sizeof(Vector) ) ;	
	
	FILE* fin = fopen(p.c_str(), "rb") ;
	if(fin == NULL){
		printf("Can't open velocity file.\n") ;
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
		
	printf("read EigenVector \n");	
		
	std::string p;
	int pos = path.find("parameters.csv");
	p = path.substr(0, pos);
	p.append("eigenVector.raw");
	
	eigenVectors = (Vector ***)AllocArray3D(slices, rows, cols, sizeof(Vector) ) ;	
	
	FILE* fin = fopen(p.c_str(),"rb") ;
	if(fin == NULL){
		printf("Can't open eigen vector file.\n") ;
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
	//			printf("Eigen Vectors %f %f %f\n",velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]);
				
			 }
		 }
	 }

	free(velU) ;
	free(velV) ;
	free(velW) ;

}

void readEigenValues(std::string path){
	
	printf("read EigenValue \n");	
	
	std::string p;
	int pos = path.find("parameters.csv");
	p = path.substr(0, pos);
	p.append("eigenValue.raw");
	
	 int u = 0, v = 0, w = 0;
	
	eigenValues = (Vector ***)AllocArray3D(slices, rows, cols, sizeof(Vector) ) ;	
//	eigenfloats= (GLfloat*)malloc(slices * rows * cols * 3 * sizeof(GLfloat) );
	FILE* fin = fopen(p.c_str(),"rb") ;

	if(fin == NULL){
		printf("Can't open eigen value file.\n") ;
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
	//			printf("Eigen Values %f %f %f\n",velU[tupleIndex],velV[tupleIndex],velW[tupleIndex]);
			}
		 }
	 }
	free(velU) ;
	free(velV) ;
	free(velW) ;
}

void readData(){
	
	printf("%d %d %d\n",rows,cols,slices);
	
	readVelocityField(parameters) ;
	readEigenValues(parameters) ;
	readEigenvectorField(parameters) ;
	
	printf("after read\n");
}