#include <iostream>
#include <SDL.h>

using namespace std;


extern "C" {
	int buffercmd(const char* in, const char** out, void* data);
}


namespace sdl {
	extern int  running;
	extern int  screen_width, screen_height, screen_scale;
	extern SDL_Window*    win;
	extern SDL_Renderer*  ren;
	int init(int width, int height, int scale);
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
		*out = "...";
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

	int mainloop() {
		printf("screen test start\n");
		sdl::init(300, 200, 2);

		while(sdl::running) {
			SDL_SetRenderDrawColor(sdl::ren, 255, 0, 0, 255);
			SDL_RenderClear(sdl::ren);
			SDL_RenderPresent(sdl::ren);
			SDL_Event e;
			while (SDL_PollEvent(&e))
				switch (e.type) {
				case SDL_QUIT:  sdl::running = 0;  break;
			}
		}

		SDL_Quit();
		return 0;
	}
} // end sdl