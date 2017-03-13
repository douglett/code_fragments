#include <iostream>
#include <sstream>
#include <vector>
#include <SDL.h>

using namespace std;


extern "C" {
	int buffercmd(const char* in, const char** out, void* data);
}


namespace sdl {
	struct Sprite {
		int           x=0, y=0, w=0, h=0;
		int           zindex = 0;
		SDL_Texture*  tex = NULL;
		std::string   id;
		std::vector<uint32_t>  data;
	};
	extern int  running;
	extern int  screen_width, screen_height, screen_scale;
	extern SDL_Window*    win;
	extern SDL_Renderer*  ren;
	extern std::vector<Sprite>    sprlist;
	extern std::vector<uint32_t>  keylist;
	int init(int width, int height, int scale);
	int quit();
	int flip();
	int blit(int x, int y, const uint32_t* data, const std::string& id);
	int makesprite(const std::string& id, int width, int height);
	int updatesprite(Sprite& spr);
	Sprite* getsprite(const std::string& id);
	int mainloop();
} // end sdl


int main() {
	// sdl::mainloop();
}



// local vars
static string rval;
// local functions
template<typename ... Args>
static string strfmt(const string& fmt, Args ... args) {
	int size = snprintf(NULL, 0, fmt.c_str(), args ...) + 1; // Extra space for '\0'
	char cs[size];
	snprintf(cs, size, fmt.c_str(), args ...);
	return cs;
}


int buffercmd(const char* in, const char** out, void* data) {
	auto& dblock = *(vector<vector<char>>*) data;
	stringstream ss(in);
	string cmd;
	ss >> cmd;
	*out = "ok";  // default value
	if (cmd == "info") {
		*out = "basic sdl2 sprite drawing library";
	}
	else if (cmd == "version") {
		*out = "1";
	}
	else if (cmd == "cmdlist") {
		*out = 
			"info :: get lib info\n"
			"version :: get version number\n"
			"init :: make new window (300x200)\n"
			"quit :: close window\n"
			"flip :: flip to screen. clears input buffer.\n"
			"cls :: cls <color [rgba]>\n"
			"blit :: blit to backbuffer.  page-1: data[w, h, [rgba]],  page-2: {x, y}\n"
			"sprite :: set x, y and/or zindex values. format: <id> x <int> y <int> z <int>\n"
			"running :: get running (true/false)\n"
			"getkeys :: get keycode list into data page 1\n"
			"mainloop :: test sdl2 mainloop";
	}
	else if (cmd == "init") {
		sdl::init(300, 200, 2);
	}
	else if (cmd == "quit") {
		sdl::quit();
	}
	else if (cmd == "flip") {
		sdl::flip();
		if (!sdl::running)  { *out = "running flag set false";  return 1; }
	}
	else if (cmd == "cls") {
		uint32_t bgcol = 0x000000ff;  // default: black
		ss >> hex >> bgcol;
		printf("bgcol: %x\n", bgcol);
		auto& data = sdl::getsprite("backbuffer")->data;
		for (int i = 0; i < data[0]*data[1]; i++)
			data[2 + i] = bgcol;
	}
	else if (cmd == "blit") {
		string id;
		ss >> id;
		if (dblock.size() < 2) {
			*out = "wrong number of data args";
			return 1;
		}
		uint32_t*  dat = (uint32_t*) &dblock[0][0];
		int32_t*   pos =  (int32_t*) &dblock[1][0];
		if (sdl::blit( pos[0], pos[1], dat, id )) {
			*out = rval.c_str();
			return 1;
		}
	}
	else if (cmd == "makesprite") {
		string id;
		int  w = 0,  h = 0;
		ss >> id >> w >> h;
		auto* spr = sdl::getsprite(id);
		if (spr != NULL) {
			rval = "sprite width id [" + id + "] already exists";
			*out = rval.c_str();
			return 1;
		}
		if (w <= 0 || h <= 0 || w > 512 || h > 512) {
			rval = strfmt("bad size values:  w %d,  h %d  [0 <= x <= 512]", w, h);
			*out = rval.c_str();
			return 1;
		}
		return  sdl::makesprite(id, w, h);
	}
	else if (cmd == "sprite") {
		string id;
		int val;
		ss >> id;
		auto* spr = sdl::getsprite(id);
		if (!spr) {
			rval = "could not find sprite. id: " + id; 
			*out = rval.c_str();
			return 1;
		}
		// set positions
		while (ss >> id) {
			if (id == "x") {
				ss >> val;
				spr->x = val;
			} else if (id == "y") {
				ss >> val;
				spr->y = val;
			} else if (id == "z") {
				ss >> val;
				spr->zindex = val;
			}
		}
	}
	else if (cmd == "running") {
		*out = ( sdl::running ? "true" : "false" );
	}
	else if (cmd == "getkeys") {
		if (dblock.size() < 1)  dblock.emplace_back();
		dblock[0].assign( (char*) &sdl::keylist[0], (char*) &sdl::keylist[sdl::keylist.size()] );
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
	vector<Sprite>    sprlist;
	vector<uint32_t>  keylist;

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
		makesprite("backbuffer", width, height);
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

	static void keyvecadd(uint32_t key) {
		for (auto k : keylist)
			if (k == key)  return;
		keylist.push_back(key);
	}
	static void keyvecrm(uint32_t key) {
		for (int i = 0; i < keylist.size(); i++)
			if (keylist[i] == key)
				keylist.erase(keylist.begin() + i);
	}

	int flip() {
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
			case SDL_QUIT:     sdl::running = 0;  break;
			case SDL_KEYDOWN:  keyvecadd (e.key.keysym.sym);  break;
			case SDL_KEYUP:    keyvecrm  (e.key.keysym.sym);  break;
		}
		return 0;
	}

	int blit(int posx, int posy, const uint32_t* data, const std::string& id) {
		// source info
		int sw = data[0];
		int sh = data[1];
		// dest info
		Sprite* spr = getsprite(id);
		if (spr == NULL) {
			rval = "could not find sprite. id: " + id;
			return 1;
		}
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

	static bool spritecmp(const Sprite& l, const Sprite& r) {
		return l.zindex < r.zindex;
	}

	int makesprite(const string& id, int width, int height) {
		width  = max(1, width);
		height = max(1, height);
		// emplace sprite
		sprlist.emplace_back();
		auto& spr = sprlist.back();
		spr.id = id,  spr.w = width,  spr.h = height;
		// make pixel data
		spr.data.resize(2 + width * height);
		spr.data[0] = width,  spr.data[1] = height;
		for (int i = 0; i < width * height; i++)
			spr.data[2 + i] = 0;
		// make texture
		spr.tex = SDL_CreateTexture(ren,  // make texture
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 
			width, height);
		// update
		updatesprite(sprlist.back());
		sort( sprlist.begin(), sprlist.end(), spritecmp );
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

		while(sdl::running)
			flip();

		quit();
		return 0;
	}
} // end sdl