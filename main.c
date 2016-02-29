#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "readfile.h"
#include "matrix.h"
#include "dlib.h"

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

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

int main(int argc, char *argv[]){
	
	GLFWwindow *win;
	GLuint VBO, EBO;
	GLuint VAO;
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
	
	// define some vertices
	GLfloat vertex_data[] = {
		// triangles share faces now
		// Vx Position            Color               Tex Coords
		0.0f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		0.5f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
		0.5f, 0.0f, 0.0f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f
	}; // this makes a neat little trapazoid thingy

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

	// VAO's and VBO's are a bit tricky to understand, so this part is heavily commented.
	glBindVertexArray(VAO);
		// bind a vertex buffer
		// put data in said buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

		// position
		glVertexAttribPointer(
			0,                   // index      : attribute index (first is zero)
			3,                   // size       : how many 'components' to this attribute (xyz)
			GL_FLOAT,            // type       : data type (float)
			GL_FALSE,            // normalized : should the data be normalized? (nope, already is)
			8 * sizeof(GLfloat), // stride     : generally the total size of a vertex
			(GLvoid*)(0)
		);
		
		// color
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)(3 * sizeof(GLfloat))
		);
		
		// tex coords
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)(6 * sizeof(GLfloat))
		);
		
		// Vertex Attributes are disabled by default. (enable the 'zeroth' attribute we just bound)
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);

	
	// [wireframe mode]
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	matrix *transform, *translate, *rotate;

	translate = matrix_create(4,4);

	matrix_copy(translate, (float[]){
		1.0f, 0.0f, 0.0f,-0.25f,	
		0.0f, 1.0f, 0.0f,-0.25f,	
		0.0f, 0.0f, 1.0f, 0.0f,	
		0.0f, 0.0f, 0.0f, 1.0f,	
	});


	// Run until glfw decides to close
	while(!glfwWindowShouldClose(win)){

		float time = (float)(glfwGetTime() * 100);
		
		rotate    = matrix_rotate_z(time);
		transform = matrix_multiply(rotate, translate);

		glfwPollEvents();
		
		// Clear buffer.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		
		// Use our linked shaders.	
		glUseProgram(shader_program);
		
		// Set vx_color variable.
		GLint var_transform;
		var_transform = glGetUniformLocation(shader_program, "transform");
		glUniformMatrix4fv(var_transform, 1, GL_TRUE, transform->data);
		
		glBindTexture(GL_TEXTURE_2D, texture);
		
		// Call our VAO bindings (get ready to draw), then draw!
		glBindVertexArray(VAO);
		
		// mode, count, type, indices (offset)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Load buffer to screen
		glfwSwapBuffers(win);
	}
	
	// Free our glfw resources.
	glfwTerminate();

	return EXIT_SUCCESS;
}
