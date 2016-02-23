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
GLuint compile_shader(GLuint type, char *source, char *name){
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
    		printf("SHADER COMPILATION_FAILED '%s'\n%s\n", name, info_log);
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


GLuint load_program(char **filenames, GLint *types, size_t len){
	GLuint program, *shaders;
	size_t i;

	shaders = malloc(sizeof(GLuint) * len); // allocate space for shaders

	for(i=0; i<len; i++){
		char *src = readfile(filenames[i]);
		
		if(src==NULL) 
		printf("[!] Could not read shader file '%s'\n", filenames[i]);
		
		shaders[i] = compile_shader(types[i], src, filenames[i]);

		free(src);
	}

	program = link_program(shaders, len);

	free(shaders);

	return program;
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
	GLuint VBO, EBO;
	GLuint VAO;
	GLuint shader_program;
	
	win            = init_window();
	shader_program = init_program();
	
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
