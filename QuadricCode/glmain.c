// Timothy Luciani
// Assignment 3
// Transformer

#include <glut/glut.h>
#include "glmain.h"
#include "stdio.h"
#include "stdlib.h"

Shape theShape;
Shape theDisplay;

#define MAX_LIGHTS  8
#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))

int crt_transform, crt_vs = 100, crt_rs = 100;
int crt_render_mode;// = GL_TRIANGLES;
int x_camera = 0, y_camera = 0, z_camera = 60;
int lookAtX = 0, lookAtY = 0, lookAtZ = 0;
int spin_val;

float cL = 0.33;
float cP = 0.27;
float cS = 0.0;

int theta_x = 0;
int theta_y = 0;

typedef struct _LITE{
	GLfloat amb[4];
	GLfloat diff[4];
	GLfloat spec[4];
	GLfloat pos[4];
	GLfloat dir[3];
	GLfloat angle;
}LITE;

typedef struct _CAM{
	GLfloat pos[4];
	GLfloat at[4];
	GLfloat up[4];
	
	GLfloat dir[4];
}CAM;

GLfloat shine;
GLfloat emi[4];
GLfloat amb[4];
GLfloat diff[4];
GLfloat spec[4];

CAM my_cam;
GLfloat camx, camy, camz;
GLfloat atx, aty, atz;
GLfloat upx, upy, upz;

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
void parse_obj(char *buffer){
	
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
	
void parse_camera(char *buffer){
		CAM *pc;
	char *ppos, *plook, *pup;
	
	pc = &my_cam;
	
	strtok(buffer, "()");
	ppos  = strtok(NULL, "()");  strtok(NULL, "()"); 
	plook  = strtok(NULL, "()");  strtok(NULL, "()"); 
	pup  = strtok(NULL, "()");  strtok(NULL, "()"); 
	
	parse_floats(ppos, pc->pos);
	parse_floats(plook, pc->at);
	parse_floats(pup, pc->up);
	
	pc->at[0] += pc->pos[0];
	pc->at[1] += pc->pos[1];
	pc->at[2] += pc->pos[2];
	
	pc->dir[0] = pc->at[0] - pc->pos[0];
	pc->dir[1] = pc->at[1] - pc->pos[1];
	pc->dir[2] = pc->at[2] - pc->pos[2];
	normalize(pc->dir);
	printf("read camera\n");
}

void lighting_setup () {
	int i;
	GLfloat globalAmb[]     = {.1, .1, .1, .1};
	
	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	
	// reflective propoerites -- global ambiant light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
	
	// create flashlight
	GLfloat amb[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat dif[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat spec[] = {0.0, 4.0, 1.0, 1.0};
	
	glLightfv(GL_LIGHT0, GL_POSITION, my_cam.at);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, my_cam.dir);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 5.0);
	
	glEnable(GL_LIGHT0);
	
	// setup properties of lighting
	for (i=1; i<num_lights; i++) {
		glEnable(GL_LIGHT0+i);
		glLightfv(GL_LIGHT0+i, GL_AMBIENT, my_lights[i].amb);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, my_lights[i].diff);
		glLightfv(GL_LIGHT0+i, GL_SPECULAR, my_lights[i].spec);
		glLightfv(GL_LIGHT0+i, GL_POSITION, my_lights[i].pos);
		if ((my_lights[i].dir[0] > 0) ||  (my_lights[i].dir[1] > 0) ||  (my_lights[i].dir[2] > 0)) {
			glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, my_lights[i].angle);
			glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, my_lights[i].dir);
		}
	}
	
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
				parse_obj(buffer);
				break;	
			case 'l':
				parse_light(buffer);
				break;
				
			case 'c':
				parse_camera(buffer);
				break;
				
			default:
				break;
		}
	}
}


/*******************************************************
 FUNCTION: main
 ARGS: argc, argv
 RETURN: 0
 DOES: main function (duh!); starts GL, GLU, GLUT, then loops 
 ********************************************************/
int main(int argc, char** argv)
{	
	glutInit(&argc, argv);
	glut_setup();
	gl_setup();
	my_setup();
	glutMainLoop();
	
	return(0);
}

/*******************************************************
 FUNCTION: glut_setup
 ARGS: none
 RETURN: none
 DOES: sets up GLUT; done here because we like our 'main's tidy
 ********************************************************/
/* This function sets up the windowing related glut calls */
void glut_setup (){
	
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(700,700);
	glutInitWindowPosition(20,20);
	glutCreateWindow("Quadrics");
	
	/* set up callback functions */
	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape);
	glutMouseFunc(my_mouse);
	glutKeyboardFunc(my_keyboard);
	glutKeyboardUpFunc( my_keyboard_up );	
	glutTimerFunc( 100, my_TimeOut, 0 );/* schedule a my_TimeOut call with the ID 0 in 20 seconds from now */
	
	theShape.type = SUPER;
	theShape.color = WHITE;
	theShape.rs =20;
	theShape.vs = 20;
	make_shape(&theShape);
	crt_transform = NONE_MODE;
	T.sX = 1; T.sY = 1; T.sZ = 1;
	T.deg = 0;
	return;
}

/*******************************************************
 FUNCTION: gl_setup
 ARGS: none
 RETURN: none
 DOES: sets up OpenGL (name starts with gl but not followed by capital letter, so it's ours)
 ********************************************************/
void gl_setup(void) {
	// enable depth handling (z-buffer)
	glEnable(GL_DEPTH_TEST);
	
	// define the background color 
	glClearColor(0,0,0,1);
	
	glMatrixMode(GL_PROJECTION) ;
	
	glLoadIdentity() ;
	
	gluPerspective(20, 1.0, 000.1, 10000.0);
	
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity() ;  // init modelview to identity
	
	return ;
}

/*******************************************************
 FUNCTION: my_setup
 ARGS: 
 RETURN:
 DOES: pre-computes stuff and presets some values
 ********************************************************/
/*TODO add make_cone, make_torus, make_your_shape etc.   */
void my_setup(){
	crt_render_mode = GL_TRIANGLE_FAN;
	make_shape(&theShape);
	read_spec("/Users/onorinbejasus/Desktop/spec");
	lighting_setup();
	
	return;
}

/*******************************************************
 FUNCTION: my_reshape
 ARGS: new window width and height 
 RETURN:
 DOES: remaps viewport to the Window Manager's window
 ********************************************************/
void my_reshape(int w, int h) {
	// ensure a square view port
	glViewport(0,0,min(w,h),min(w,h));
	return ;
	
}
void reset()
{ // resets scene back to original settings
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity() ;
}
void draw_quads(GLfloat vertices[50][50][4], int iv1a, int iv1b, int iv2a, int iv2b, int iv3a, int iv3b, int iv4a, int iv4b, int ic)
{
	float U[2][3];
	float V[2][3];
	
	float N[2][3];
	GLfloat finalN[3];
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shine);
	
	glBegin(crt_render_mode); 
	{
		glColor3fv(colors[RED]);
		
		
		U[0][0] = vertices[iv2a][iv2b][0]-vertices[iv1a][iv1b][0];
		U[0][1] = vertices[iv2a][iv2b][1]-vertices[iv1a][iv1b][1];
		U[0][2] = vertices[iv2a][iv2b][2]-vertices[iv1a][iv1b][2];
		
		V[0][0] = vertices[iv3a][iv3b][0]-vertices[iv1a][iv1b][0];
		V[0][1] = vertices[iv3a][iv3b][1]-vertices[iv1a][iv1b][1];
		V[0][2] = vertices[iv3a][iv3b][2]-vertices[iv1a][iv1b][2];
		
		N[0][0] = (U[0][1] * V[0][2]) - (U[0][2]*V[0][1]);
		N[0][1] = (U[0][2] * V[0][0]) - (U[0][0]*V[0][2]);
		N[0][2] = (U[0][0] * V[0][1]) - (U[0][1]*V[0][0]);
		
		U[1][0] = vertices[iv2a][iv2b][0]-vertices[iv4a][iv1b][0];
		U[1][1] = vertices[iv2a][iv2b][1]-vertices[iv4a][iv1b][1];
		U[1][2] = vertices[iv2a][iv2b][2]-vertices[iv4a][iv1b][2];
		
		V[1][0] = vertices[iv3a][iv3b][0]-vertices[iv4a][iv1b][0];
		V[1][1] = vertices[iv3a][iv3b][1]-vertices[iv4a][iv1b][1];
		V[1][2] = vertices[iv3a][iv3b][2]-vertices[iv4a][iv1b][2];
		
		N[1][0] = (U[1][1] * V[1][2]) - (U[1][2]*V[1][1]);
		N[1][1] = (U[1][2] * V[1][0]) - (U[1][0]*V[1][2]);
		N[1][2] = (U[1][0] * V[1][1]) - (U[1][1]*V[1][0]);
		
		finalN[0] = (N[0][0]+ N[1][0])/2.0;
		finalN[1] = (N[0][1]+ N[1][1])/2.0;
		finalN[2] = (N[0][2]+ N[1][2])/2.0;

	//	normalize(&finalN);
		glNormal3fv(finalN);
		
		/*note the explicit use of homogeneous coords below: glVertex4f*/
		glVertex4fv(vertices[iv1a][iv1b]);
		glVertex4fv(vertices[iv2a][iv2b]);
		glVertex4fv(vertices[iv3a][iv3b]);
		glVertex4fv(vertices[iv4a][iv4b]);
	}
	glEnd();
	
}
/***********************************
 FUNCTION: draw_axes
 ARGS: none
 RETURN: none
 DOES: draws main X, Y, Z axes
 ************************************/
void draw_axes( void ) {
    glLineWidth( 5.0 );
    glBegin(GL_LINES); 
	{
		glColor3fv(colors[1]);
		glVertex4fv(vertices_axes[0]);
		glVertex4fv(vertices_axes[1]);
		
		glColor3fv(colors[4]);
		glVertex4fv(vertices_axes[0]);
		glVertex4fv(vertices_axes[2]);
		
		glColor3fv(colors[6]);
		glVertex4fv(vertices_axes[0]);
		glVertex4fv(vertices_axes[3]);
	}
	glEnd();
	glLineWidth(1.0);
	
}
void draw_shape(Shape *verts)
{
	// polymorphic way to draw shapes
	
	glClearColor(0,0,0,0); // else black
	
	int i, j; // loop variables 
	
	for(i = 0; i < verts->vs; i++) // handles main body of array
	{ 
		for(j = 0; j < verts->rs; j++)
		{
			draw_quads(verts->verts,i,j,i,j+1,i+1,j+1,i+1,j,verts->color); 
		}
	}
}
/***********************************
 FUNCTION: draw_object 
 ARGS: shape to create (HOUSE, CUBE, CYLINDER, etc)
 RETURN: none
 DOES: draws an object from triangles
 ************************************/
/*TODO: expand according to assignment 3 specs*/
void draw_object(int shape) {
			draw_shape(&theShape);
}

/***********************************
 FUNCTION: my_theShape
 ARGS: none
 RETURN: none
 DOES: main drawing function
 ************************************/
/*TODO add on*/
void my_display() {
		
//	// clear all pixels, reset depth 
//	glEnable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
//	glClearStencil(0);
//    glColorMask(0,0,0,0);
//    glDepthMask(0);
//    glStencilFunc(GL_ALWAYS, 1, 1);
//    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    
	// init to identity 
	
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity() ;
	
	gluLookAt(x_camera, y_camera, z_camera,  // x,y,z coord of the camera 
			  lookAtX, lookAtY, lookAtZ,
			  0,1,0); // the direction of Up (default is y-axis)
	
    draw_axes();
    
    
    make_shape(&theShape);
    
    glRotatef(theta_y,0,1,0);
	glRotatef(theta_x,1,0,0);
    
	draw_object(theShape.type);
    
	// this buffer is ready
	
	glutSwapBuffers();
	
}
/***********************************
 FUNCTION: my_TimeOut
 ARGS: timer id
 RETURN: none
 DOES: handles "timer" events
 ************************************/
void my_TimeOut(int id) {
		

	if(spin_val == SPIN_ENABLED)
		T.deg = (10*M_PI/180);
	else
		T.deg = 0;
	
	glutPostRedisplay();
	
	glutTimerFunc(100, my_TimeOut, 0);
	
	return;
}
/***********************************
 FUNCTION: my_idle
 ARGS: none
 RETURN: none
 DOES: handles the "idle" event
 ************************************/
void my_idle(void) {
}

void multiply(GLfloat mat1[4][4], GLfloat vect[3])
{
	GLfloat result[3];
	result[0] = (mat1[0][0] * vect[0]) + (mat1[0][1] * vect[1]) + (mat1[0][2] * vect[2]) + (mat1[0][3] * vect[3]);
	result[1] = (mat1[1][0] * vect[0]) + (mat1[1][1] * vect[1]) + (mat1[1][2] * vect[2]) + (mat1[1][3] * vect[3]);
	result[2] = (mat1[2][0] * vect[0]) + (mat1[2][1] * vect[1]) + (mat1[2][2] * vect[2]) + (mat1[2][3] * vect[3]);
	
	vect[0] = result[0];
	vect[1] = result[1];
	vect[2] = result[2];
}
/*******************************************************
 FUNCTION: my_keyboard
 ARGS: key id, x, y
 RETURN:
 DOES: handles keyboard events
 ********************************************************/

void my_keyboard( unsigned char key, int x, int y) {
	switch( key ) {
		
        case 'l':
            cL -= 0.1;
            break;
        case 'L':
            cL += 0.1;
        case 'p':
            cP -= 0.1;
            break;
        case 'P':
            cP += 0.1;
        case 's':
            cS -= 0.1;
            break;
        case 'S':
            cS += 0.1;
            break;
		case 'q':
        case 'Q':
            exit(0);
        case 'y': // rotate around the y
		case 'Y': {
			theta_y = (theta_y+2) %360;
			glutPostRedisplay(); 
		}; break;
		case 'x': // rotate about the x
		case 'X': {
			theta_x = (theta_x+2) %360;
			glutPostRedisplay(); 
		}; break; 
            
		default: break;
            
	}
	
    my_display();
    
	return ;
}

/*******************************************************
 FUNCTION: my_keyboard_up
 ARGS: key id, x, y
 RETURN:
 DOES: handles keyboard up events
 ********************************************************/
void my_keyboard_up( unsigned char key, int x, int y ) {}

/*******************************************************
 FUNCTION: my_mouse
 ARGS: button, state, x, y
 RETURN:
 DOES: handles mouse events
 ********************************************************/

void my_mouse(int button, int state, int mousex, int mousey) {}