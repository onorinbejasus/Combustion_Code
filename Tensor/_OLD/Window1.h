 #ifndef WINDOW1_H
 #define WINDOW1_H

#pragma once

#include <QtOpenGL/qgl.h>
#include "Headers.h"
#include "mainwindow.h"
#include "Parameters.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include "FatalError.h"
#include "StreamlineRenderer.h"
//#include "VolumeRenderer.h"
#include "AllocArray.h"
#include "VectorFieldRenderer.h"
#include <GLUT.h> 
#include "Projector.h"
#include "TensorData.h"
#include "CommonParameters.h"

#define DEFUALT_PIXEL_SIZE 0.125

enum SelectedObjectCatagory1{
		NO_OBJECT1 = 0,
		STREAMLINE1,
		SEEDPOINT1
	};
enum vectorFieldSource1{
		velocityFieldSource1 = 0 ,
		eigenvectorFieldSource1 
	};
enum InteractionMode1{
		ExploreMode1 = 0,
		SelectMode1
	};
 class Tensor1 : public QGLWidget
 {
     Q_OBJECT

 public:
     Tensor1(int w, int h, int t, QWidget *parent = 0);
     ~Tensor1();		 
	 int ImageWidth, ImageHeight;
	 int type;											// whether tensor or sm
	 bool parallel_rotate;
	 Tensor1 *neighbor ;
	 CommonParameters commonViewingParams;

	 Projector *projector;
	 TensorData *tensorDataSet;

	 void ReconfigureProjector(Parameters *param);
	 void ReconfigureProjector(Projector *param);
	 void ConfigureVectorField();
	 void ConfigureStreamline();
	 void ConfigureVolRenderer();
	 void UpdateRenderers();
	 void UpdateRenderers(Tensor1 *tensor) ;

 public slots:
 //    void setXRotation1(int angle);
 //    void setYRotation1(int angle);
 //    void setZRotation1(int angle);

 signals:
 //    void xRotationChanged1(int angle);
 //    void yRotationChanged1(int angle);
 //    void zRotationChanged1(int angle);

	 void setLeftWindow(Tensor1* tensor);
	 void setRightWindow(Tensor1* tensor);

 protected:
	 void resizeGL(int width, int height);
	 void initializeGL();
     void paintGL();
	 void mousePressEvent(QMouseEvent *event);
	 void mouseReleaseEvent(QMouseEvent *event);    
	 void mouseMoveEvent(QMouseEvent *event);
	 void keyPressEvent(QKeyEvent *event);
	 void MouseMoveToRotate(int diffX, int diffY);
	 void DrawAxisAndBox();
	 void SelectObjectAtXY(int x, int y);
	 void MouseMoveToZoom(int diffX, int diffY);
	 void processHits (GLint hits, GLuint buffer[]);
	 void readVelocityField();
	 void readEigenvectorField();

 private:
     void normalizeAngle(int *angle);
     
	 QPoint lastPos;
	 InteractionMode1 CurrIM;
	 
	 std::string param;
	 int buttonDownX, buttonDownY;
	 int LeftButtonDown, RightButtonDown ;
	
	StreamlineRenderer		*streamlines ;					//STREAMLINE1s object.
	//VolumeRenderer			*volRenderer ;					//volume renderer
	VectorFieldRenderer		*vectorField ;					//vector field renderer


	SelectedObjectCatagory1 ObjectCatagory ;
	int iObjectName ;

	int vfSource ;

 }; // end Tensor1 class
 
	static Tensor1 *glWidget1, *glWidget2;

	/* small multiple widgets */

	 static Tensor1 *smWidget1, *smWidget2, *smWidget3, *smWidget4, *smWidget5;
#endif