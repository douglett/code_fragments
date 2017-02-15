#include <iostream>
#include <vector>
#include <SDL.h>

using namespace std;


extern "C" {
	int buffercmd(const char* in, const char** out, void* data);
}


namespace sdl {
	struct Sprite {
		int           x, y, w, h;
		uint32_t*     data;
		SDL_Texture*  tex;
	};
	extern int  running;
	extern int  screen_width, screen_height, screen_scale;
	extern SDL_Window*    win;
	extern SDL_Renderer*  ren;
	extern std::vector<Sprite> sprlist;
	int init(int width, int height, int scale);
	int quit();
	int paint();
	int makesprite(int width, int height);
	int updatesprite(Sprite& spr);
	int mainloop();
} // end sdl


int main() {
	sdl::mainloop();
}


int buffercmd(const char* in, const char** out, void* data) {
	string cmd = in;
	if (cmd == "info") {
		*out = "sdl2test...";
		return 0;
	} else if (cmd == "version") {
		*out = "1";
		return 0;
	} else if (cmd == "cmdlist") {
		*out = 
			"info :: get lib info\n"
			"version :: get version number\n"
			"mainloop :: test sdl2 mainloop";
		return 0;
	} else if (cmd == "mainloop") {
		*out = "mainloop running...";
		return sdl::mainloop();
	}
	return 1;
}


namespace sdl {
	int  running=1;
	int  screen_width=0,  screen_height=0,  screen_scale=1;
	SDL_Window*    win = NULL;
	SDL_Renderer*  ren = NULL;
	vector<Sprite> sprlist;

	int init(int width, int height, int scale) {
		screen_width  = max(1, width);
		screen_height = max(1, height);
		screen_scale  = max(1, scale);
		// setup SDL
		int init = SDL_Init(SDL_INIT_VIDEO);
		win = SDL_CreateWindow( "xd",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			screen_width * screen_scale, screen_height * screen_scale, 
			0 );
		ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
		// error checking
		if (init != 0 || win == NULL || ren == NULL) {
			cerr << SDL_GetError() << endl;
			return 1;
		}
		// initial state
		SDL_RenderSetLogicalSize(ren, screen_width, screen_height);
		SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
		return 0;
	}

	int quit() {
		SDL_Quit();
		return 0;
	}

	int paint() {
		SDL_SetRenderDrawColor(sdl::ren, 180, 0, 0, 255);
		SDL_RenderClear(sdl::ren);
		for (const auto& spr : sprlist) {
			SDL_Rect dst = { spr.x, spr.y, spr.w, spr.h };
			SDL_RenderCopy(ren, spr.tex, NULL, &dst);
		}
		SDL_RenderPresent(sdl::ren);
		// clear events
		SDL_Event e;
		while (SDL_PollEvent(&e))
			switch (e.type) {
			case SDL_QUIT:  sdl::running = 0;  break;
		}
		return 0;
	}

	int makesprite(int width, int height) {
		width  = max(1, width);
		height = max(1, height);
		// make pixel data
		uint32_t* data = new uint32_t[2 + width * height];
		data[0] = width,  data[1] = height;
		for (int i = 0; i < width * height; i++)
			data[2 + i] = 0xffff00ff;
		// make texture
		SDL_Texture* tex = SDL_CreateTexture(ren,  // make texture
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 
			width, height);
		// save sprite and update
		sprlist.push_back({ 0, 0, width, height, data, tex });
		updatesprite(sprlist.back());
		return 0;
	}

	int updatesprite(Sprite& spr) {
		uint32_t* pix; 	// holds pixel data
		int pitch;	    // holds width*bytelen (unused)
		SDL_LockTexture(spr.tex, NULL, (void**)&pix, &pitch);  // prepare texture data
		memcpy(pix, spr.data+2, sizeof(uint32_t)*spr.w*spr.h);  // copy pixels to GPU
		SDL_UnlockTexture(spr.tex);  // done copying
		return 0;
	}

	int mainloop() {
		printf("screen test start\n");
		sdl::init(300, 200, 2);
		makesprite(100, 100);

		while(sdl::running) {
			paint();
		}

		quit();
		return 0;
	}
} // end sdl