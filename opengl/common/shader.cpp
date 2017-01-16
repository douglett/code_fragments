#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

#include <GL/glew.h>
#include "shader.hpp"

using namespace std;




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
	int InfoLogLength;


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

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}