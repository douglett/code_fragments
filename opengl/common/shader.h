/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

#include <string>
#include <GL/glew.h>

namespace common {
	enum SHADERS {
		SHADER_DEFAULT,
		SHADER_COLOR
	};
	GLuint SelectShaders(SHADERS shader);
	GLuint LoadShaders  (const std::string& VertexFilePath, const std::string& FragmentFilePath);
	GLuint BuildShaders (const std::string& VertexShaderCode, const std::string& FragmentShaderCode);
} // end common