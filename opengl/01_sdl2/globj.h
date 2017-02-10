#pragma once

#include <string>
#include <vector>

namespace x3 {
	struct GLtri {
		float  vec[9]; // = {0};
		float  col[4]; // = {0};
	};

	struct GLsquare {
		uint32_t  x=0, y=0, w=10, h=10;
		float     col[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
		uint32_t  texID  = 0;
	};

	class GLobj {
	public:
		std::string id;
		float  x=0,   y=0,   z=0;
		float  pitch=0,  yaw=0;
		uint32_t texID = 0;
		std::vector<GLtri>  tris;
		void translate (float x, float y, float z);
		void scale     (float s);
	};

	namespace glbuild {
		void    make();
		GLobj*  finalize();
		GLobj*  clone(const GLobj* obj);
		void    col  (float r, float g, float b);
		void    col  (float r, float g, float b, float a);
		void    tex  (const std::string name);
		void    tri  (const std::vector<float>& v);
		void    quad (const std::vector<float>& v);
		void    tris (const std::vector<float>& v);
		void    quads(const std::vector<float>& v);
		std::string  serialize(const GLobj* obj);
		GLobj*       deserialize(const std::string& s);
	} // end glbuild
} // end x3