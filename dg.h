#ifndef DG_H
#define DG_H

#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL.h>
#include "readfile.h"

#ifndef DG_SHADER_MAX
#define DG_SHADER_MAX 256
#endif

static GLuint DG_PROGRAM;
static GLuint DG_SHADER_STACK[DG_SHADER_MAX];
static GLuint DG_SHADER_COUNT = 0;

void dg_compile_shader(GLuint *dest, GLuint type, char *source, char *name);
void dg_attach(char *filename, GLuint type);
void dg_program(GLuint *dest);
void dg_delete_shaders();
void dg_compile();
GLFWwindow *dg_window(int width, int height, char *title);
GLuint dg_texture(char *filename);

void dg_program(GLuint *dest){
	DG_PROGRAM = glCreateProgram();
	*dest      = DG_PROGRAM;
}

void dg_delete_shaders(){
	size_t i;

	for(i=0; i<DG_SHADER_COUNT; i++)
		glDeleteShader(DG_SHADER_STACK[i]);

	DG_SHADER_COUNT = 0;
}

void dg_attach(char *filename, GLuint type){
	char *src;
	GLuint shader;

	if(DG_SHADER_COUNT >= DG_SHADER_MAX){
		printf("[dg] dg_attach :: reached max number of shaders: %d\n", DG_SHADER_MAX);
		return;
	}

	src = readfile(filename);
	
	if( src == NULL ){
		printf("[dg] dg_attach :: error reading file '%s'\n", filename); 
		return;
	}
	
	dg_compile_shader(&shader, type, src, filename);
	
	// save shader for later deletion 
	DG_SHADER_STACK[DG_SHADER_COUNT] = shader;
	DG_SHADER_COUNT++;

	glAttachShader(DG_PROGRAM, shader);
}

void dg_compile(){
	GLint success;
	GLchar info_log[512];

	glLinkProgram(DG_PROGRAM);
	glGetProgramiv(DG_PROGRAM, GL_LINK_STATUS, &success);
	
	if(!success){
		glGetProgramInfoLog(DG_PROGRAM, 512, NULL, info_log);
		printf("[dg] dg_compile :: link failed \n%s\n", info_log);
		return;
	}

	dg_delete_shaders();
}

// compile/check a shader
void dg_compile_shader(GLuint *dest, GLuint type, char *source, char *name){
	// pack source into an array (opengl likes source files as a list of strings)
	const GLchar *sources[1];
	GLuint shader;
	GLint success;
	GLchar info_log[512];

	sources[0] = source;
	shader     = glCreateShader(type);

	glShaderSource(shader, 1, sources, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	
	if(!success){
		glGetShaderInfoLog(shader, 512, NULL, info_log);
    	printf("SHADER COMPILATION_FAILED '%s'\n%s\n", name, info_log);
	}

	*dest = shader;
}


GLFWwindow *dg_window(int width, int height, char *title){
	GLFWwindow *window;
	
	// Init the window manager
	glfwInit();
	
	// Optional?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	
	// Create a glfw window
	window = glfwCreateWindow(width, height, title, NULL, NULL);

	// Check that the window initialized
	if(window==NULL){
		printf("Failed to create GLFW window.\n\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	glfwMakeContextCurrent(window);
	
	glewExperimental = GL_TRUE;	
	
	if(glewInit() != GLEW_OK){
		printf("Failed to init GLEW.\n\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, width, height);

	return window;
}

GLuint dg_texture(char *filename){	
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
