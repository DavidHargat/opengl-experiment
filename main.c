#include <stdlib.h>
#include <stdio.h>

// all opengl related imports happen in dg.h
#include "dg.h"

// local vectors and matrices
#include "maths.h"

// a cube model
#include "model.h"

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

// define a callback for key events
void keypress(GLFWwindow* window, int key, int scancode, int action, int mode){
	printf("keypress %d %d %d %d\n\n", key, scancode, action, mode);
}

int main(int argc, char *argv[]){
	//glfwSetKeyCallback(window, &keypress);
	GLFWwindow *window;
	GLuint VAO, VBO;
	GLuint program;
	GLuint crate;

	matrix camera, perspective, rotate, scale, translate;
	GLuint l_camera, l_rotate, l_scale, l_translate, l_time, l_perspective;
	float time;
	
	// init window
	window = dg_window(1920-192, 1080-108, "OpenGL Experiment");

	// init shaders
	dg_program(&program);
	  dg_attach("vertex.glsl", GL_VERTEX_SHADER);
	  dg_attach("fragment.glsl", GL_FRAGMENT_SHADER);
	dg_compile();
	
	// init texture
	crate = dg_texture("crate.jpg");

	// init model
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnable(GL_DEPTH_TEST);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MODEL_CUBE), MODEL_CUBE, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// locate shader variables
	l_scale = glGetUniformLocation(program, "scale");
	l_camera = glGetUniformLocation(program, "camera");
	l_rotate = glGetUniformLocation(program, "rotate");
	l_translate = glGetUniformLocation(program, "translate");
	l_perspective = glGetUniformLocation(program, "perspective");
	
	// init matrices
	matrix_perspective(&perspective, 90.0f, 90.0f, 0.1f, 100.0f);
	matrix_identity(&rotate);
	matrix_identity(&scale);
	matrix_identity(&translate);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // uncomment for wireframe

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		time = (float)(glfwGetTime() * 100);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//eye target up
		matrix_camera(&camera,
			// position 
			0.0f, 0.0f, 0.0f,
			// angle
			0.0f, 0.0f, 0.0f
		);
		
		glUseProgram(program);
		
		// send to gpu
		glUniform1f(l_time, time);
		glUniformMatrix4fv(l_camera, 1, GL_TRUE, camera.data);
		glUniformMatrix4fv(l_perspective, 1, GL_TRUE, perspective.data);
		
		// bind model
		glBindVertexArray(VAO);

			// calculate transformations
			matrix_scale(&scale, 1.0f, 1.0f, 1.0f);
			matrix_rotate_y(&rotate, time);
			matrix_translate(&translate, 0.0f, 0.0f, -1.0f);

			// send to gpu
			glUniformMatrix4fv(l_scale, 1, GL_TRUE, scale.data);
			glUniformMatrix4fv(l_rotate, 1, GL_TRUE, rotate.data);
			glUniformMatrix4fv(l_translate, 1, GL_TRUE, translate.data);
				
			// texture
			glBindTexture(GL_TEXTURE_2D, crate);

			// draw
			glDrawArrays(GL_TRIANGLES, 0, 36);

		// unbind model VAO
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();

	return EXIT_SUCCESS;
}
