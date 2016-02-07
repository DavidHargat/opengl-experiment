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

// define a callback for key events
void keypress(GLFWwindow* window, int key, int scancode, int action, int mode){
	printf("keypress %d %d %d %d\n\n", key, scancode, action, mode);
}

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
		printf("Failed to create GLFW window.\n\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}
	
	// Bind the window to opengl
	glfwMakeContextCurrent(win);

	glewExperimental = GL_TRUE;	
	// Check that glew initialized.
	if(glewInit() != GLEW_OK){
		printf("Failed to init GLEW.\n\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetKeyCallback(win, &keypress);

	// Specify opengl viewport.
	glViewport(0, 0, 800, 600);


	/* 
 	 * We want to draw a shape, so we need to create 
 	 * some vertex data for OpenGL.
 	 *
 	 * VBO is an ID to refer to our Vertex Buffer Object
	 * which is instantiated behind the scenes in OpenGL
	 *
	 * glGenBuffer  : Generates [n] [buffer] objects in OpenGL.
	 * glBindBuffer : Binds the [buffer] to the current type target
	 *                all commands after glBindBuffer will configure
	 *                said buffer.
	 * 
	 * glBufferData : Populates our object with our [vertex_data].
	 */

	/*
 	* Now that we have our vertex data, we need to compile
 	* a 'vertex shader' and a 'fragment shader' so that we
 	* can render.
 	*    glCreateShader  : Create the shader in OpenGL.
 	*    glShaderSource  : Bind our source code to said shader.
 	*    glCompileShader : Compile the shader.
 	*/
	
	const GLchar *vertex_shader_source =
	"                                                                    \n\
	#version 330 core                                                    \n\
	layout (location = 0) in vec3 position;                              \n\
	                                                                     \n\
	void main(){                                                         \n\
	 	gl_Position = vec4(position.x, position.y, position.z, 1.0); \n\
	}                                                                    \n\
	                                                                     \n\
	";

	const GLchar *fragment_shader_source =
	"                                         \n\
	#version 330 core                         \n\
                                                  \n\
	out vec4 color;                           \n\
                                                  \n\
	void main()                               \n\
	{                                         \n\
	    color = vec4(1.0f, 0.5f, 0.2f, 1.0f); \n\
	}                                         \n\
                                                  \n\
	";

	// Construct our vertex shader
	GLuint vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	// For storing result of compilation (if failure)
	GLint  success;
	GLchar info_log[512];
	
	// Check that the shader compiled properly.

	// Construct our fragment shader
	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	
 	// But wait, it gets better, now we have to link our compiled shaders into a 'vertex program'.

	GLuint shader_program;
	shader_program = glCreateProgram();
	  glAttachShader(shader_program, vertex_shader);
	  glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	
	// Check that our shaders compiled properly.
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s\n", info_log);
		return EXIT_FAILURE;
	}
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", info_log);
		return EXIT_FAILURE;
	}
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n %s\n", info_log);
		return EXIT_FAILURE;
	}
	
	// Don't need out individual shaders anymore
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	// Vertex Data
	GLfloat vertex_data[] = {
		- 0.5f, - 0.5f, + 0.0f,
		+ 0.5f, - 0.5f, + 0.0f,
		+ 0.0f, + 0.5f, + 0.0f
	};
	
	GLuint VBO;
	glGenBuffers(1, &VBO);	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Vertex Arrays
	GLuint VAO;
	glGenVertexArrays(1, &VAO);  

	glBindVertexArray(VAO);
	  glBindBuffer(GL_ARRAY_BUFFER, VBO);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	  glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	

	// Run until glfw decides to close
	while(!glfwWindowShouldClose(win)){
		glfwPollEvents();
		
		// Clear screen.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		glUseProgram(shader_program);
		glBindVertexArray(VAO);
		  glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(win);
	}

	// Free our glfw resources.
	glfwTerminate();
	return EXIT_SUCCESS;
}
