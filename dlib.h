#ifndef DLIB_H
#define DLIB_H

// DLIB - Dave's (OpenGL) Library

#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL.h>

#include "readfile.h"


// interface
void dlib_shader_vertex(char *filename);
void dlib_shader_fragment(char *filename);
void dlib_link_program();
void dlib_compile();
GLFWwindow *dlib_window(int width, int height, char *title);
GLuint dlib_load_texture(char *filename);

// helper functions
GLuint dlib_compile_shader(GLuint type, char *source, char *name);

// static memory
GLuint DLIB_VERTEX_SHADER   = 0;
GLuint DLIB_FRAGMENT_SHADER = 0;
GLuint DLIB_PROGRAM         = 0;
GLFWwindow *DLIB_WINDOW     = 0;
unsigned char DLIB_RUNNING  = 0;

void dlib_shader_vertex(char *filename){
	char *src = readfile(filename);
	if(src == NULL){
		printf("DLIB : ERROR READING FILE : '%s'\n", filename);
		exit(EXIT_FAILURE);
	}
	DLIB_VERTEX_SHADER = dlib_compile_shader(GL_VERTEX_SHADER, src, filename);
	free(src);
}

void dlib_shader_fragment(char *filename){
	char *src = readfile(filename);
	if(src == NULL){
		printf("DLIB : ERROR READING FILE : '%s'\n", filename);
		exit(EXIT_FAILURE);
	}
	DLIB_FRAGMENT_SHADER = dlib_compile_shader(GL_FRAGMENT_SHADER, src, filename);
	free(src);
}

void dlib_compile(){
	if(DLIB_VERTEX_SHADER == 0) printf("%s\n", "DLIB : MISSING SHADER : VERTEX");
	if(DLIB_FRAGMENT_SHADER == 0) printf("%s\n", "DLIB : MISSING SHADER : FRAGMENT");
	dlib_link_program();
}

// compile/check a shader
GLuint dlib_compile_shader(GLuint type, char *source, char *name){
	GLuint shader;
	GLint success;
	GLchar info_log[512];

	// pack source into an array
	// (opengl likes source files as a list of strings)
	const GLchar *sources[1];
	sources[0] = source;

	// init a shader object
	shader = glCreateShader(type);
	glShaderSource(shader, 1, sources, NULL);
	glCompileShader(shader);

	// check that it compiled properly
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(shader, 512, NULL, info_log);
    	printf("SHADER COMPILATION_FAILED '%s'\n%s\n", name, info_log);
		exit(EXIT_FAILURE);
	}
	
	return shader;
}

// link/check a (shader) program
void dlib_link_program(){
	GLuint program;
	GLint  success;	
	GLchar info_log[512];

	program = glCreateProgram();

    glAttachShader(program, DLIB_VERTEX_SHADER);
    glAttachShader(program, DLIB_FRAGMENT_SHADER);
	
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	
	if(!success){
		glGetProgramInfoLog(program, 512, NULL, info_log);
		printf("PROGRAM LINK_FAILED\n %s\n", info_log);
		exit(EXIT_FAILURE);
	}

	// the individual shaders are no longer needed.
	glDeleteShader(DLIB_VERTEX_SHADER);
	glDeleteShader(DLIB_FRAGMENT_SHADER);
	
	DLIB_PROGRAM = program;
}

GLFWwindow *dlib_window(int width, int height, char *title){
	// Init the window manager
	glfwInit();
	
	// Optional?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a glfw window
	DLIB_WINDOW = glfwCreateWindow(width, height, title, NULL, NULL);

	// Check that the window initialized
	if(DLIB_WINDOW==NULL){
		printf("Failed to create GLFW window.\n\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	// Bind the window to opengl
	glfwMakeContextCurrent(DLIB_WINDOW);

	// enable "modern features" of opengl (won't work without this)
	glewExperimental = GL_TRUE;	
	
	// Check that glew initialized.
	if(glewInit() != GLEW_OK){
		printf("Failed to init GLEW.\n\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Specify opengl viewport.
	glViewport(0, 0, width, height);

	return DLIB_WINDOW;
}

GLuint dlib_load_texture(char *filename){	
	// Generate a texture
	GLuint texture;
	int width=0, height=0;
	unsigned char *image;
	image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return texture;
}

#endif
