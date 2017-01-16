/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

#include <string>
#include <GL/glew.h>

GLuint LoadShaders (const std::string& VertexFilePath, const std::string& FragmentFilePath);
GLuint BuildShaders(const std::string& VertexShaderCode, const std::string& FragmentShaderCode);