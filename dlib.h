#ifndef DLIB_H
#define DLIB_H

// DLIB - Dave's (OpenGL) Library

#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "readfile.h"

GLuint compile_shader(GLuint type, char *source, char *name);
GLuint link_program(GLuint *shaders, GLuint length);
GLuint load_program(char **filenames, GLint *types, size_t len);
GLFWwindow *init_simple_window(int width, int height, char *title);

// compile/check a shader
GLuint compile_shader(GLuint type, char *source, char *name){
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
GLuint link_program(GLuint *shaders, GLuint length){
	GLuint i, program;
	
	GLint  success;	
	GLchar info_log[512];

	program = glCreateProgram();

	for( i=0; i<length; i++ )
      		glAttachShader(program, shaders[i]);
	
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	
	if(!success){
		glGetProgramInfoLog(program, 512, NULL, info_log);
		printf("PROGRAM LINK_FAILED\n %s\n", info_log);
		return EXIT_FAILURE;
	}

	// the individual shaders are no longer needed.
	for( i=0; i<length; i++ )
		glDeleteShader(shaders[i]);
	
	return program;	
}


GLuint load_program(char **filenames, GLint *types, size_t len){
	GLuint program, *shaders;
	size_t i;

	shaders = malloc(sizeof(GLuint) * len); // allocate space for shaders

	for(i=0; i<len; i++){
		char *src = readfile(filenames[i]);
		
		if(src==NULL) 
		printf("[!] Could not read shader file '%s'\n", filenames[i]);
		
		shaders[i] = compile_shader(types[i], src, filenames[i]);

		free(src);
	}

	program = link_program(shaders, len);

	free(shaders);

	return program;
}

GLFWwindow *init_simple_window(int width, int height, char *title){
	// Init the window manager
	glfwInit();
	
	// Optional?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	// Create a glfw window
	GLFWwindow* win;
	win = glfwCreateWindow(width, height, title, NULL, NULL);

	// Check that the window initialized
	if(win==NULL){
		printf("Failed to create GLFW window.\n\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	// Bind the window to opengl
	glfwMakeContextCurrent(win);

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

	return win;
}

#endif
