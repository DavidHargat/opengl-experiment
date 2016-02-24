#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "readfile.h"
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
	
	// define some vertices
	GLfloat vertex_data[] = {
		// triangles share faces now
		+ 0.5f, + 0.5f, + 0.0f,
		+ 0.5f, - 0.5f, + 0.0f,
		  0.0f, + 0.5f, + 0.0f,
		- 0.5f, - 0.5f, + 0.0f 
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
	
		glVertexAttribPointer(
			0,                    // index      : attribute index (first is zero)
			3,                    // size       : how many 'components' to this attribute (xyz)
			GL_FLOAT,             // type       : data type (float)
			GL_FALSE,             // normalized : should the data be normalized? (nope, already is)
			3 * sizeof(GLfloat),  // stride     : size of this attribute (3 floats)
			(GLvoid*)(0) // pointer    : offset (starts at zero)
		);

		// Vertex Attributes are disabled by default. (enable the 'zeroth' attribute we just bound)
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	
	glBindVertexArray(0);

	
	// [wireframe mode]
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Run until glfw decides to close
	while(!glfwWindowShouldClose(win)){
		glfwPollEvents();
		
		// Clear buffer.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		// Use our linked shaders.	
		glUseProgram(shader_program);
		
		// Set vx_color variable.
		GLint var_color;
		var_color = glGetUniformLocation(shader_program, "vx_color");
		glUniform4f(var_color, 0.0f, 0.0f, 1.0f, 1.0f);
	
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
