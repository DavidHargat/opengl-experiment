#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "smatrix.h"
#include "dlib.h"
#include "model.h"

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

// define a callback for key events
void keypress(GLFWwindow* window, int key, int scancode, int action, int mode){
	printf("keypress %d %d %d %d\n\n", key, scancode, action, mode);
}

GLuint init_shape(){
	GLuint VAO, VBO;
	
	// define some vertices
	GLfloat vertex_data[] = {
		// Position             Tex Coords
		-0.25f,  0.25f, 0.0f,   0.0f, 1.0f,
		 0.25f,  0.25f, 0.0f,   1.0f, 1.0f,
		-0.25f, -0.25f, 0.0f,   0.0f, 0.0f,
		 0.25f, -0.25f, 0.0f,   1.0f, 0.0f
	};

	// Vertex Buffer Object for our triangle vertices
	glGenBuffers(1, &VBO);
	
	// Vertex Array Object for our Vertex Buffer bindings
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
			glEnable(GL_DEPTH_TEST);
			
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(model_cube), model_cube, GL_STATIC_DRAW);

			// position, tex
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
			
			// Vertex Attributes are disabled by default. (enable the 'zeroth' attribute we just bound)
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			
	glBindVertexArray(0);

	return VAO;
}

void transform(GLuint program, 
float tx, float ty, float tz,
float rx, float ry, float rz
){
	matrix rotate, translate, scale, perspective, temp;
	GLuint var;

	// allocate stack memory for matrices
	matrix_salloc(temp, 4, 4);
	matrix_salloc(scale, 4, 4);
	matrix_salloc(rotate, 4, 4);
	matrix_salloc(translate, 4, 4);
	matrix_salloc(perspective, 4, 4);

	// calculate
	matrix_identity(&rotate);
	
	matrix_rotate_x(&temp, rx); 
	matrix_multiply(&rotate, &temp, &rotate);
	
	matrix_rotate_y(&temp, ry);
	matrix_multiply(&rotate, &temp, &rotate);
	
	matrix_rotate_z(&temp, rz); 
	matrix_multiply(&rotate, &temp, &rotate);

	matrix_scale(&scale, 1.0f, 1.0f, 1.0f);
	matrix_translate(&translate, tx, ty, tz);
	//matrix_orthographic(&perspective, 8.0f, 8.0f, 0.1f, 4.0f); // width, height, near far
	matrix_perspective(&perspective, 90.0f, 90.0f, 0.1f, 4.0f); // fovx, fovy, near far

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
	GLuint texture = dlib_load_texture("crate.jpg");

	// [wireframe mode]
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VAO_RECT = init_shape();

	
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

		glBindVertexArray(VAO_RECT);
		
		transform(program,
			-1.0f, 0.0f, -3.0f + cosf(time/100),
			time/2.0f, time, 0.0f
		);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		transform(program,
			1.0f, 0.0f, -1.0f,
			time/10.0f, time, 0.0f
		);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		/*
		transform(program,
			0.0f, -0.25f, 0.0f,
			0.0f, time, 0.0f
		);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();

	return EXIT_SUCCESS;
}
