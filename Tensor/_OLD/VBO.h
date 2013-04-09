#pragma once

#include <GL/glew.h>
#include <GLUT.h>

#include<iostream>
#include <cmath>

#include "Shaders.h"

// compile the shader

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

// create a vbo

void create_VBO(GLuint vbo, int size, GLfloat *verts){

	//create buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//initialize buffer object
	unsigned int m_size = size * 4 * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, m_size, verts, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void draw_VBO(GLuint vbo, GLuint shader, int size){

	// render from VBO	
	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	// load shader program
	glUseProgram(shader);
	glUniform1f(glGetUniformLocation(shader, "pointScale"), 512/tan(150*0.5*(float)3.1415/180.0) );
	glUniform1f( glGetUniformLocation(shader, "pointRadius"), 0.05 );

	glColor3f(0.0, 1.0, 0.0);

	// bind vbo
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	// draw vbo
	glDrawArrays(GL_POINTS, 0, size);

	// clean up
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glUseProgram(0);
	glDisable(GL_POINT_SPRITE_ARB);


}