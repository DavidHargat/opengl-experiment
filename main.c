#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "matrix.h"
#include "dlib.h"
#include "model.h"
#include "vec3.h"
#include "maths.h"

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
	
	// Vertex Buffer Object for our triangle vertices
	glGenBuffers(1, &VBO);
	
	// Vertex Array Object for our Vertex Buffer bindings
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	glEnable(GL_DEPTH_TEST);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MODEL_CUBE), MODEL_CUBE, GL_STATIC_DRAW);

	// position, tex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	
	// Vertex Attributes are disabled by default. (enable the 'zeroth' attribute we just bound)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
			
	glBindVertexArray(0);

	return VAO;
}

void load_uniform(GLuint program, char *name, GLfloat *data){
	GLuint var;
	var = glGetUniformLocation(program, name);
	glUniformMatrix4fv(var, 1, GL_TRUE, data);
}

void transform_rotate(GLuint program, float x, float y, float z, float angle){
	matrix temp;
	matrix_rotate(&temp, x, y, z, angle);
	load_uniform(program, "rotate", temp.data);
}

void transform_translate(GLuint program, float x, float y, float z){
	matrix temp;
	matrix_translate(&temp, x, y, z);
	load_uniform(program, "translate", temp.data);
}

void transform_scale(GLuint program, float x, float y, float z){
	matrix temp;
	matrix_scale(&temp, x, y, z);
	load_uniform(program, "scale", temp.data);
}

void transform_perspective(GLuint program, float fovx, float fovy, float near, float far){
	matrix temp;
	matrix_perspective(&temp, fovx, fovy, near, far);
	load_uniform(program, "perspective", temp.data);
}

void transform_camera(GLuint program, 
float px, float py, float pz, 
float rx, float ry, float rz
){
	matrix temp;
	matrix_camera(&temp, px, py, pz, rx, ry, rz);
	load_uniform(program, "camera", temp.data);
}
int main(int argc, char *argv[]){
	//glfwSetKeyCallback(window, &keypress);
	// dlib initialization
	GLFWwindow *window;
	GLuint VAO_RECT;
	GLuint program;
	GLuint crate, wall, floor;

	window = dlib_window(1920-192, 1080-108, "OpenGL Experiment");
	
	dlib_shader_vertex("vertex.glsl");
	dlib_shader_fragment("fragment.glsl");

	// load textures
	crate = dlib_load_texture("crate.jpg");
	wall  = dlib_load_texture("wall.jpg");
	floor = dlib_load_texture("floor.png");

	VAO_RECT = init_shape();
	program  = dlib_program();
	
	// [wireframe mode]
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*
	float pos = 0;
	GLfloat cameraSpeed = 0.05f;
	if(key == GLFW_KEY_W)
		cameraPos += cameraSpeed * cameraFront;
	if(key == GLFW_KEY_S)
		cameraPos -= cameraSpeed * cameraFront;
	if(key == GLFW_KEY_A)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if(key == GLFW_KEY_D)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;  
	*/

	matrix look;
	float time, ratio;
	GLuint var;
	while(!glfwWindowShouldClose(window)){
		time = (float)(glfwGetTime() * 100);
		
		var = glGetUniformLocation(program, "time");
		glUniform1f(var, time);
	
		glfwPollEvents();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glUseProgram(program);

		glBindVertexArray(VAO_RECT);

		ratio = 1920.0f/1080.0f; 
		transform_perspective(program, 90.0f, 90.0f * ratio, 0.1f, 100.0f); // fovx, fovy, near far
			
		transform_scale(program, 1.0f, 1.0f, 1.0f);
		
		/*
		transform_camera(program,
			0.0f + cosf(time/100.0f), 1.0f, 0.5f + sinf(time/100.0f),
			20.0f, 0.0f, 0.0f
		);
		*/

		//eye target up
		matrix_lookat(&look, 
			0.1f, 1.0f, 1.0f,
			1.5f*cosf(time/100.0f), 1.0f + sinf(time/100.0f), -2.0f,
			//-1.5f, 1.0f, -2.0f,
			0.0f, 1.0f, 0.0f
		); load_uniform(program, "camera", look.data);

		transform_rotate(program, 0.0f, 0.0f, 0.0f, 0.0f);
		transform_scale(program, 1.0f, 1.0f, 1.0f);
		
		// first cube	
		transform_translate(program, -1.5f, 0.6f, -2.0f);
		glBindTexture(GL_TEXTURE_2D, crate);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// second cube
		transform_translate(program,  1.5f, 0.6f, -2.0f);
		glBindTexture(GL_TEXTURE_2D, crate);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		// floor
		transform_scale(program, 10.0f, 0.1f, 10.0f);
		transform_translate(program, 0.0f, 0.0f,  0.0f);
		glBindTexture(GL_TEXTURE_2D, floor);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// walls
		transform_scale(program, 9.5f, 10.0f, 9.5f);
		glBindTexture(GL_TEXTURE_2D, wall);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();

	return EXIT_SUCCESS;
}
