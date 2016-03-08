#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "readfile.h"
#include "smatrix.h"
#include "dlib.h"

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

static GLuint VAO_RECT;
static GLuint PROGRAM;

// define a callback for key events
void keypress(GLFWwindow* window, int key, int scancode, int action, int mode){
	printf("keypress %d %d %d %d\n\n", key, scancode, action, mode);
}

GLuint init_program(){

	char *filenames[2] = {
		"vertex.glsl", 
		"fragment.glsl"
	};

	GLint types[2] = {
		GL_VERTEX_SHADER, 
		GL_FRAGMENT_SHADER
	};
	
	return load_program(filenames, types, 2);
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

void draw_rect(float x, float y, float angle){
	
	matrix scale, transform, translate, rotate;
	GLuint var;
	float s = 1.0f;
	
	matrix_salloc(transform, 4, 4);
	matrix_salloc(translate, 4, 4);
	matrix_salloc(rotate,    4, 4);
	matrix_salloc(scale,     4, 4);

	scale.data = (float[]){
		   s, 0.0f, 0.0f, 0.0f,	
		0.0f,    s, 0.0f, 0.0f,	
		0.0f, 0.0f,    s, 0.0f,	
		0.0f, 0.0f, 0.0f, 1.0f,	
	};

	transform.data = (float[]){
		1.0f, 0.0f, 0.0f, 0.0f,	
		0.0f, 1.0f, 0.0f, 0.0f,	
		0.0f, 0.0f, 1.0f, 0.0f,	
		0.0f, 0.0f, 1.0f, 1.0f,	
	};

	//transform = transform;
	matrix_rotate_x(&rotate, angle);
	matrix_multiply(&rotate, &transform, &rotate);

	glUseProgram(PROGRAM);

	var = glGetUniformLocation(PROGRAM, "rotate");
	glUniformMatrix4fv(var, 1, GL_TRUE, rotate.data);
	
	var = glGetUniformLocation(PROGRAM, "transform");
	glUniformMatrix4fv(var, 1, GL_TRUE, transform.data);
	
	// Call our VAO bindings (get ready to draw), then draw!
	glBindVertexArray(VAO_RECT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int main(int argc, char *argv[]){
	
	GLFWwindow *win;
	
	GLuint shader_program;
	
	win            = init_simple_window(800, 600, "OpenGL Experiment");
	shader_program = init_program();
	
	glfwSetKeyCallback(win, &keypress);

	// Load image to RAM
	int width=0, height=0;
	unsigned char *image;
	
	image = SOIL_load_image("noise.png", &width, &height, 0, SOIL_LOAD_RGB);

	// Generate a texture
	GLuint texture;
	glGenTextures(1, &texture);

	// Load image to GPU
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Free image from RAM
	SOIL_free_image_data(image);
	
	// [wireframe mode]
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	VAO_RECT = init_shape();
	PROGRAM  = shader_program;

	// Run until glfw decides to close
	while(!glfwWindowShouldClose(win)){
		glfwPollEvents();
		
		float time = (float)(glfwGetTime() * 100);
	
		// Clear buffer.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		
		glBindTexture(GL_TEXTURE_2D, texture);
		draw_rect(0, 0, -time);

		// Load buffer to screen
		glfwSwapBuffers(win);
	}
	
	// Free our glfw resources.
	glfwTerminate();

	return EXIT_SUCCESS;
}
