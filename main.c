#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//a-frame.io

/*
 * glfw - window/input manager
 * glew - 'extention wrangler' (loads opengl function pointers)
 */

int main(int argc, char *argv[]){
	
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
		printf("Failed to create GLFW window.\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}
	
	// Bind the window to opengl
	glfwMakeContextCurrent(win);
	
	// Check that glew initialized.
	if(glewInit() != GLEW_OK){
		printf("Failed to init GLEW.\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Specify opengl viewport.
	glViewport(0, 0, 800, 600);

	// Clear the screen.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Run until glfw decides to close
	while(!glfwWindowShouldClose(win)){
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	// Free our glfw resources.
	glfwTerminate();
	return EXIT_SUCCESS;
}
