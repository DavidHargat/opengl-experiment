#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "smatrix.h"
#include "dlib.h"

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

// define a callback for key events
void keypress(GLFWwindow* window, int key, int scancode, int action, int mode){
	printf("keypress %d %d %d %d\n\n", key, scancode, action, mode);
}

GLuint init_shape(){
	GLuint VAO, VBO, EBO;
	
	// define some vertices
	GLfloat vertex_data[] = {
		// Position             Color               Tex Coords
		-0.25f,  0.25f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		 0.25f,  0.25f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
		-0.25f, -0.25f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		 0.25f, -0.25f, 0.0f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f
	};

	// which vertices to draw (by index)
	GLuint index_data[] = {
		0,1,2, // first triangle
		1,2,3  // second triangle 
	};

	// Vertex Buffer Object for our triangle vertices
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	// Vertex Array Object for our Vertex Buffer bindings
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
	
	// position, color, tex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	
	// Vertex Attributes are disabled by default. (enable the 'zeroth' attribute we just bound)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);	
	
	glBindVertexArray(0);

	return VAO;
}

void transform(GLuint program, float time){
	matrix rotate, translate, scale, perspective;
	GLuint var;

	// allocate stack memory for matrices
	matrix_salloc(scale, 4, 4);
	matrix_salloc(rotate, 4, 4);
	matrix_salloc(translate, 4, 4);
	matrix_salloc(perspective, 4, 4);

	// calculate
	matrix_rotate_x(&rotate, time);
	matrix_scale(&scale, 4.0f, 4.0f, 1.0f);
	matrix_translate(&translate, 0.0f, 0.0f, -1.0f + cosf(time/100));
	//matrix_orthographic(&perspective, 4.0f, 4.0f, 0.1f, 4.0f); // width, height, near far
	matrix_perspective(&perspective, 45.0f, 90.0f, 0.1f, 4.0f); // fovx, fovy, near far

	// send matrices tp GPU
	var = glGetUniformLocation(program, "rotate");
	glUniformMatrix4fv(var, 1, GL_TRUE,  rotate.data);
	var = glGetUniformLocation(program, "translate");
	glUniformMatrix4fv(var, 1, GL_TRUE,  translate.data);
	var = glGetUniformLocation(program, "scale");
	glUniformMatrix4fv(var, 1, GL_TRUE,  scale.data);
	var = glGetUniformLocation(program, "perspective");
	glUniformMatrix4fv(var, 1, GL_TRUE,  perspective.data);
}

//glfwSetKeyCallback(window, &keypress);

int main(int argc, char *argv[]){
	
	// dlib initialization
	
	GLFWwindow *window =
	dlib_window(800, 600, "OpenGL Experiment");
	
	dlib_shader_vertex("vertex.glsl");
	dlib_shader_fragment("fragment.glsl");
	dlib_compile();

	// Generate a texture
	GLuint texture = dlib_load_texture("noise.png");

	// [wireframe mode]
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VAO_RECT = init_shape();

	glEnable(GL_DEPTH_TEST);
	
	GLuint program;
	program = DLIB_PROGRAM;

	float time;
	while(!glfwWindowShouldClose(window)){
		time = (float)(glfwGetTime() * 100);
		glfwPollEvents();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(program);

		transform(program, time);		
		
		glBindVertexArray(VAO_RECT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();

	return EXIT_SUCCESS;
}
