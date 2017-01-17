#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
// #include <map>

#include <GL/glew.h>
#include "shader.h"

using namespace std;


extern const vector<string> VertexShaders, FragmentShaders;


namespace common {

	GLuint SelectShaders(SHADERS shader) {
		switch (shader) {
		case SHADER_DEFAULT:  return BuildShaders(VertexShaders[0], FragmentShaders[0]);
		case SHADER_COLOR:    return BuildShaders(VertexShaders[1], FragmentShaders[1]);
		}
		return 0;
	}

	GLuint LoadShaders(const std::string& VertexFilePath, const std::string& FragmentFilePath) {
		// Read the Vertex Shader code from the file
		string VertexShaderCode, line;
		ifstream VertexShaderStream(VertexFilePath, ios::in);
		if (VertexShaderStream.is_open()) {
			while (getline(VertexShaderStream, line))
				VertexShaderCode += line + "\n";
			VertexShaderStream.close();
		} else {
			printf( "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", 
					VertexFilePath.c_str() );
			return 0;
		}

		// Read the Fragment Shader code from the file
		string FragmentShaderCode;
		ifstream FragmentShaderStream(FragmentFilePath, ios::in);
		if (FragmentShaderStream.is_open()) {
			while (getline(FragmentShaderStream, line))
				FragmentShaderCode += line + "\n";
			FragmentShaderStream.close();
		} else {
			printf( "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", 
					VertexFilePath.c_str() );
			return 0;
		}

		// build shaders
		return BuildShaders(VertexShaderCode, FragmentShaderCode);
	}


	GLuint BuildShaders(const std::string& VertexShaderCode, const std::string& FragmentShaderCode) {
		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		GLint Result = GL_FALSE;
		int InfoLogLength = 0;

		// Compile Vertex Shader
		printf("Compiling vertex shader : %s\n", VertexShaderCode.substr(0, 12).c_str());
		const char* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
		// glShaderSource(VertexShaderID, 1, VertexShaderCode.c_str() , NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}

		// Compile Fragment Shader
		printf("Compiling fragment shader : %s\n", FragmentShaderCode.substr(0, 12).c_str());
		char const * FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
		// glShaderSource(FragmentShaderID, 1, &FragmentShaderCode.c_str(), NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}

		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		// cleanup
		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}

} // end common




const vector<string> 

VertexShaders = {    // vertex shaders begin
R"(#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
})",


R"(#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

// Output data ; will be interpolated for each fragment.
out vec3 fragmentColor;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main() {
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
	fragmentColor = vertexColor;
})"
},  // vertex shaders end



FragmentShaders = {    // fragment shaders begin
R"(#version 330 core

// Ouput data
out vec3 color;

void main() {
	// Output color = gray 
	color = vec3(0.8,0.8,0.8);
})",


R"(#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Ouput data
out vec3 color;

void main(){
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	color = fragmentColor;
})"
};  // fragment shaders end