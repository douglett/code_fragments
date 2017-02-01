#include <vector>
#include <list>
#include <random>
#include <OpenGL/gl.h>
#include "gltex.h"

using namespace std;


namespace x3 {
namespace gltex {

	const int TEX_W = 128;

	class Tex {
	public:
		string name;
		uint32_t texID = 0;
		vector<uint32_t> data;
		// Tex()  {  }
		~Tex() { erase(); }
		// send to gpu
		uint32_t send() {
			glGenTextures( 1, &texID );
			glBindTexture( GL_TEXTURE_2D, texID );
				glTexImage2D   ( GL_TEXTURE_2D, 0, GL_RGBA, TEX_W, TEX_W, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &data[0] );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );  // simple pixel-perfect scaling
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );  // needed?
			glBindTexture( GL_TEXTURE_2D, 0 );  // unbind
			return texID;
		}
		// erase from gpu
		void erase() {
			glDeleteTextures( 1, &texID );  // delete
			texID = 0;  // set to zero
		}
	};

	static list<Tex> texlist;

	uint32_t generate(const std::string& name, const std::string& type) {
		texlist.push_back(Tex());
		Tex& t = texlist.back();
		t.name = name;
		t.data.resize(TEX_W * TEX_W, 0x000000ff);
		// make patterns
		if (type == "greyscale_static") {
			srand(time(NULL));
			int x, y, c;
			for (int i = 0; i < TEX_W * TEX_W * 5; i++) {
				x = rand() % TEX_W;
				y = rand() % TEX_W;
				c = rand() % 256;
				t.data[y * TEX_W + x] = c<<24 | c<<16 | c<<8 | 0xff;
			}
		}
		t.send();
		return t.texID;
	}

	uint32_t gettexID(const std::string& name) {
		for (const auto& t : texlist )
			if (t.name == name)
				return t.texID;
		return 0;
	}

} // end gltex
} // end x3