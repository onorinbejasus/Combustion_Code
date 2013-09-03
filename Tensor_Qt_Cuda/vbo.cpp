#include "open_gl.hh"
#include "Shaders.h"
#include <iostream>
#include <cmath>

/*----------------------------------------------------------
					Create VBO
----------------------------------------------------------*/

GLuint createVBO(GLfloat *points, int size)
{
	GLuint vbo = 0;
	
    // create buffer object
    glGenBuffers( 1, &vbo);
    glBindBuffer( GL_ARRAY_BUFFER, vbo);

    // initialize buffer object
    unsigned int m_size = size * sizeof(float);

    glBufferData( GL_ARRAY_BUFFER, m_size, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_size, points);
	
	return vbo;
}

/*----------------------------------------------------------
					Delete VBO
----------------------------------------------------------*/
void deleteVBO(GLuint vbo, int index)
{
    glBindBuffer( 1, vbo);
    glDeleteBuffers( 1, &vbo);
}

// ============
// = Draw VBO =
// ============

void draw_streamlines (GLuint vbo, GLuint shader, int size/*, GLfloat *value*/ )
{
	// render from the vbo
	glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
  //  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shader);

	glClearColor(0.0, 0.0, 0.0, 0.0);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    glVertexPointer(4, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_POINTS, 0, size);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glDisableClientState(GL_VERTEX_ARRAY); 
    glDisableClientState(GL_COLOR_ARRAY);

    glUseProgram(0);
    glDisable(GL_POINT_SPRITE_ARB);
}

// =============================
// = Compile Shader Program(s) =
// =============================

GLuint compileShader(const char *vsource, const char *fsource){
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vsource, 0);
    glShaderSource(fragmentShader, 1, &fsource, 0);
    
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char temp[256];
        glGetProgramInfoLog(program, 256, 0, temp);
        printf("Failed to link program:\n%s\n", temp);
        glDeleteProgram(program);
        program = 0;
    }

    return program;	
}
