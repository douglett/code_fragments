#include <iostream>
// local
#include "core.h"
#include "shader.h"
// GL extensions
#include <glm/gtc/matrix_transform.hpp>

using namespace std;



namespace common {

	// vars
	static GLFWwindow*  window;
	static GLuint       VertexArrayID, programID, MatrixID;
	static glm::mat4    Projection, View, Model, MVP;
	// public vars
	int running = 1;
	string winName = "opengl common window";
	int screenWidth = 1024, screenHeight = 768;
	vector<GLobj*> objlist;


	
	int init() {
		// Initialise GLFW
		if( !glfwInit() )
		{
			fprintf( stderr, "Failed to initialize GLFW\n" );
			return -1;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Open a window and create its OpenGL context
		window = glfwCreateWindow( screenWidth, screenHeight, winName.c_str(), NULL, NULL );
		if( window == NULL ){
			fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);

		// Initialize GLEW
		glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			glfwTerminate();
			return -1;
		}

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		// Dark blue background
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS); 

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// Create and compile our GLSL program from the shaders
		// programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
		programID = SelectShaders(SHADER_DEFAULT);

		// Get a handle for our "MVP" uniform
		MatrixID = glGetUniformLocation(programID, "MVP");

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		View       = glm::lookAt(
									glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
									glm::vec3(0,0,0), // and looks at the origin
									glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
							   );
		// Model matrix : an identity matrix (model will be at the origin)
		Model      = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

		return 0;
	}



	int quit() {
		// delete objects
		// ...
		// delete program
		glDeleteProgram(programID);
		glDeleteVertexArrays(1, &VertexArrayID);
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
		return 0;
	}



	int paint() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the screen
		glUseProgram(programID);  // Use our shader
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		// draw objects here
		if (vbufferID > 0 && cbufferID > 0) {
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbufferID);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cbufferID);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, vbufferSize()); // 12*3 indices starting at 0 -> 12 triangles
			// unset vertex attribs
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		// Check if the ESC key was pressed or the window was closed
		if      (glfwWindowShouldClose(window) == 1)  running = 0;
		else if (glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS)  running = 0;
		return running;
	}

	int paint2() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the screen
		glUseProgram(programID);  // Use our shader
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		// draw objects here
		for (auto obj : objlist)
			if (obj->bufferID('v') > 0 && obj->bufferID('c')) {
				// 1st attribute buffer : vertices
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, obj->bufferID('v'));
				glVertexAttribPointer(
					0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

				// 2nd attribute buffer : colors
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, obj->bufferID('c'));
				glVertexAttribPointer(
					1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
				// Draw the triangle !
				glDrawArrays(GL_TRIANGLES, 0, obj->vbufferSize()); // 12*3 indices starting at 0 -> 12 triangles
				// unset vertex attribs
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
			}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		// Check if the ESC key was pressed or the window was closed
		if      (glfwWindowShouldClose(window) == 1)  running = 0;
		else if (glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS)  running = 0;
		return running;
	}

} // end common