#include <vector>
#include <list>
#include <random>
#include <cmath>
#include <OpenGL/gl.h>
#include "gltex.h"

using namespace std;


namespace x3 {
namespace gltex {

	const int TEX_W = 128;

	class Tex {
	public:
		uint32_t texID = 0;
		string name;
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

	uint32_t gettexID(const std::string& name) {
		for (const auto& t : texlist )
			if (t.name == name)
				return t.texID;
		return 0;
	}


	// function pointer
	typedef  uint32_t (*genfn_t)(uint32_t x, uint32_t y);
	// helper consts
	static const double  
		PI  = 3.14159265,
		//DEG_RAD = PI / 180.0,  // degrees->radians (n * RAD_REG)
		WID_RAD = PI / TEX_W;  // width->radians
	// random noise
	static uint32_t gen_random(uint32_t x, uint32_t y) {
		int c = rand() % 256;
		return ( c<<24 | c<<16 | c<<8 | 0xff );
	}
	// 10px stripes
	static uint32_t gen_stripes(uint32_t x, uint32_t y) {
		if (int(y / 10) % 2 == 0)  return 0xffffffff;
		return 0x000000ff;
	}
	// sine wave
	static uint32_t gen_sinewave(uint32_t x, uint32_t y) {
		int c = sin(x * WID_RAD) * 255;
		return ( c<<24 | c<<16 | c<<8 | 0xff );
	}
	// double sinewave
	static uint32_t gen_sinewave_2(uint32_t x, uint32_t y) {
		return gen_sinewave((x * 2) % TEX_W, 0);
	}
	static uint32_t gen_stars_1(uint32_t x, uint32_t y) {
		return ( rand() % 256 == 0 ? 0xffffffff : 0x000000ff );
	}


	uint32_t generate(const std::string& name, const std::string& type) {
		texlist.push_back(Tex());
		Tex& t = texlist.back();
		t.name = name;
		t.data.resize(TEX_W * TEX_W, 0x000000ff);
		// make patterns
		srand(time(NULL));
		genfn_t genfn = NULL;
		if (type == "greyscale_static")
			 genfn = gen_random;
		else if (type == "greyscale_stripes")
			genfn = gen_stripes;
		else if (type == "greyscale_sinewave")
			genfn = gen_sinewave;
		else if (type == "greyscale_sinewave_2")
			genfn = gen_sinewave_2;
		else if (type == "greyscale_stars_1")
			genfn = gen_stars_1;
		// perform function
		if (genfn != NULL)
			for (int y = 0; y < TEX_W; y++)
				for (int x = 0; x < TEX_W; x++)
					t.data[y * TEX_W + x] = genfn(x, y);
		// finish
		t.send();
		return t.texID;
	}

} // end gltex
} // end x3