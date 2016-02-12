#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "readfile.h"

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

// define a callback for key events
void keypress(GLFWwindow* window, int key, int scancode, int action, int mode){
	printf("keypress %d %d %d %d\n\n", key, scancode, action, mode);
}

// compile/check a shader
GLuint load_shader(GLuint type, char *source){
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
    		printf("SHADER COMPILATION_FAILED\n %s\n", info_log);
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

GLuint load_shaders(){
	// shaders
	GLuint shader_program, shaders[2],
	       vertex_shader,  fragment_shader;
	char   *vertex_source, *fragment_source;
	
	// read source files
	vertex_source   = readfile("vertex.glsl");
	fragment_source = readfile("fragment.glsl");
	
	if(vertex_source==NULL)   printf("[!] could not read 'vertex.glsl'\n");
	if(fragment_source==NULL) printf("[!] could not read 'fragment.glsl'\n");

	// compile shaders
	vertex_shader   = load_shader(GL_VERTEX_SHADER, vertex_source);
	fragment_shader = load_shader(GL_FRAGMENT_SHADER, fragment_source);
	
	// pack resulting shaders into an array
	shaders[0] = vertex_shader;
	shaders[1] = fragment_shader;
	
	// link shaders into program
	shader_program = link_program(shaders, 2);

	return shader_program;
}

GLFWwindow *init_window(){
	// Init the window manager
	glfwInit();
	
	// Optional?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	// Create a glfw window
	GLFWwindow* win;
	win = glfwCreateWindow(800, 600, "LearnOpeNGL", NULL, NULL);

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
	
	glfwSetKeyCallback(win, &keypress);

	// Specify opengl viewport.
	glViewport(0, 0, 800, 600);

	return win;
}

int main(int argc, char *argv[]){
	
	GLFWwindow *win;
	GLuint VBO;
	GLuint VAO;
	GLuint shader_program;
	
	win = init_window();
	shader_program = load_shaders();
	
	// Vertex Data
	GLfloat vertex_data[] = {
		- 0.5f, - 0.5f, + 0.0f,
		+ 0.5f, - 0.5f, + 0.0f,
		+ 0.0f, + 0.5f, + 0.0f
	};
	
	// Vertex Buffer Object for our triangle vertices
	glGenBuffers(1, &VBO);	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Vertex Array Object for our Vertex Buffer bindings
	glGenVertexArrays(1, &VAO);  

	glBindVertexArray(VAO);
	  glBindBuffer(GL_ARRAY_BUFFER, VBO);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	  glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	

	glUseProgram(shader_program);
	// Run until glfw decides to close
	while(!glfwWindowShouldClose(win)){
		glfwPollEvents();
		
		// Clear screen.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glBindVertexArray(VAO);
		  glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(win);
	}
	// Free our glfw resources.
	glfwTerminate();


	return EXIT_SUCCESS;
}
