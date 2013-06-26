#ifndef CALLBACKS_HH
#define CALLBACKS_HH

// OpenGL Graphics includes
#include <GL/glew.h>
#if defined (__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// CUDA utilities and system includes
#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h
#include <cutil_gl_inline.h> // includes cuda_gl_interop.h// includes cuda_gl_interop.h
#include <rendercheck_gl.h>

// CUDA Includes
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

// Shared Library Test Functions
#include <shrUtils.h>
#include <shrQATest.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>

//#include "lib/volRender/volumeRender_kernel.cuh"
#include "enums.hh"
#include "lib/std/FatalError.hh"
#include "lib/streamlines/Streamline.hh"
#include "lib/projector/Projector.hh"
//#include "lib/volume/VolumeRenderer.hh"
#include "lib/std/AllocArray.hh"
#include "lib/glyphs/VectorFieldRenderer.hh"
#include "lib/file_ops/Parameters.hh"
#include "lib/vector/Vector.hh"
#include "lib/vector/point.hh"
#include "lib/open_gl.hh"

void draw_axes(void);
void processHits (GLint hits, GLuint buffer[]);
void SelectObjectAtXY(int x, int y);
void MouseMoveToRotate(int diffX, int diffY);
void MouseMoveToSeedpointMove(float diffX, float diffY);
void MouseMoveToZoom(int diffX, int diffY);
void Magnify();

void motion(int x, int y);
void reshape(int w, int h);
void mouse(int button, int state, int x ,int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void display();
void idle();

void render();
void AutoQATest();
void computeFPS();
int iDivUp(int a, int b);
void freeCudaBuffers();
void cleanup();
void initPixelBuffer();
int chooseCudaDevice(int argc, char **argv, bool bUseOpenGL);
void initCuda(void *h_volume, cudaExtent volumeSize, float4 transfer[6], uint image_size);
void InitVolRender(int argc, char ** argv);

#endif /* end of include guard: CALLBACK_HH */