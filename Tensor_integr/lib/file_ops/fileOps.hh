#include "../open_gl.hh"

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

// tensor files
#include "Parameters.hh"
#include <time.h>
#include <fstream>
#include "../std/FatalError.hh"
#include "../streamlines/Streamline.hh"
#include "../std/AllocArray.hh"
#include "../glyphs/VectorFieldRenderer.hh"
#include <iostream>

void myabort(void);
void normalize(GLfloat *p);
void parse_floats(char *buffer, GLfloat nums[]);
void parse_light(char *buffer);
void parse_settings(char *buffer);
void read_spec(char *fname);
void readVelocityField(std::string path);
void readEigenvectorField(std::string path);
void readEigenValues(std::string path);
void *loadRawFile(const char *filename, size_t size);
void readVolRenderField(int argc, char ** argv);
void readData(int argc, char ** argv);