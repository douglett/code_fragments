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
		SDL_Texture*  tex;
		std::string   id;
		std::vector<uint32_t>  data;
	};
	extern int  running;
	extern int  screen_width, screen_height, screen_scale;
	extern SDL_Window*    win;
	extern SDL_Renderer*  ren;
	extern std::vector<Sprite> sprlist;
	int init(int width, int height, int scale);
	int quit();
	int paint();
	int blit(int x, int y, const uint32_t* data);
	int makesprite(int width, int height, const std::string& id="");
	int updatesprite(Sprite& spr);
	Sprite* getsprite(const std::string& id);
	int mainloop();
} // end sdl


int main() {
	// sdl::mainloop();
}


static string rval;


int buffercmd(const char* in, const char** out, void* data) {
	string cmd = in;
	*out = "ok";  // default value
	if (cmd == "info") {
		*out = "sdl2test...";
	}
	else if (cmd == "version") {
		*out = "1";
	}
	else if (cmd == "cmdlist") {
		*out = 
			"info :: get lib info\n"
			"version :: get version number\n"
			"mainloop :: test sdl2 mainloop";
	}
	else if (cmd == "init") {
		sdl::init(300, 200, 2);
	}
	else if (cmd == "quit") {
		sdl::quit();
	}
	else if (cmd == "paint") {
		sdl::paint();
		if (!sdl::running)  { *out = "running flag set false";  return 1; }
	}
	else if (cmd == "blit") {
		auto& datavec = *(vector<vector<char>>*) data;
		if (datavec.size() < 2) {
			*out = "wrong number of data args";
			return 1;
		}
		uint32_t*  dat = (uint32_t*) &datavec[0][0];
		int32_t*   pos =  (int32_t*) &datavec[1][0];
		return  sdl::blit(pos[0], pos[1], dat);
	}
	else if (cmd == "running") {
		*out = ( sdl::running ? "true" : "false" );
	}
	else if (cmd == "mainloop") {
		*out = "mainloop running...";
		return sdl::mainloop();
	}
	else {
		rval = "unknown command: " + cmd;
		*out = rval.c_str();
		return 1;
	}
	return 0;
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
		// backbuffer sprite
		makesprite(width, height, "backbuffer");
		return 0;
	}

	int quit() {
		while (sprlist.size()) {
			SDL_DestroyTexture(sprlist.back().tex);
			sprlist.erase(sprlist.end() - 1);
		}
		SDL_Quit();
		return 0;
	}

	int paint() {
		SDL_SetRenderDrawColor(sdl::ren, 0, 0, 0, 255);
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

	int blit(int posx, int posy, const uint32_t* data) {
		// get info
		int sw = data[0];
		int sh = data[1];
		Sprite* spr = getsprite("backbuffer");
		int dw = spr->data[0];
		int dh = spr->data[1];
		printf("(%d %d)  (%d %d %d %d)  %x  %x\n", posx, posy, sw, sh, dw, dh, data[2], data[3]);
		// blit
		for (int y = 0; y < sh; y++) {
			if (posy + y < 0 || posy + y >= dh)  continue;
			for (int x = 0; x < sw; x++) {
				if (posx + x < 0 || posx + x >= dw)  continue;
				spr->data[2 + ((posy + y) * dw) + posx + x] = data[2 + (y * sw) + x];
			}
		}
		// update
		updatesprite(*spr);
		return 0;
	}

	int makesprite(int width, int height, const string& id) {
		width  = max(1, width);
		height = max(1, height);
		// emplace sprite
		sprlist.push_back({ 0, 0, width, height, NULL });
		auto& spr = sprlist.back();
		spr.id = id;
		// make pixel data
		spr.data.resize(2 + width * height);
		spr.data[0] = width,  spr.data[1] = height;
		for (int i = 0; i < width * height; i++)
			spr.data[2 + i] = 0xffff00ff;
		// make texture
		spr.tex = SDL_CreateTexture(ren,  // make texture
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 
			width, height);
		// update
		updatesprite(sprlist.back());
		return 0;
	}

	int updatesprite(Sprite& spr) {
		uint32_t* pix; 	// holds pixel data
		int pitch;	    // holds width*bytelen (unused)
		SDL_LockTexture(spr.tex, NULL, (void**)&pix, &pitch);  // prepare texture data
		memcpy(pix, &spr.data[2], sizeof(uint32_t)*spr.w*spr.h);  // copy pixels to GPU
		SDL_UnlockTexture(spr.tex);  // done copying
		return 0;
	}

	Sprite* getsprite(const string& id) {
		for (auto& spr : sprlist)
			if (spr.id == id)
				return &spr;
		return NULL;
	}

	int mainloop() {
		printf("screen test start\n");
		sdl::init(300, 200, 2);
		// makesprite(100, 100);

		while(sdl::running) {
			paint();
		}

		quit();
		return 0;
	}
} // end sdl