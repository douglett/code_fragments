#include "globals.h"
#include <iostream>

using namespace std;


namespace vid {
	// public
	SDL_Surface *screen=NULL, *vmem=NULL;
	vector<array<int,6>> sprlist;
	vector<uint32_t> keylist;
	uint32_t tcolor=0;
	// private
	static int video_mode=0;
	static SDL_Surface *qbfont=NULL;

	void init() {
		// init library
		if (SDL_Init(SDL_INIT_VIDEO)) {
			fprintf(stderr, "error initialising SDL\n");
			exit(1);
		}
		// create screen
		screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
		if (screen==NULL) {
			fprintf(stderr, "error making screen surface\n");
			exit(1);
		}
		// first fill
		int err = SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
		if (err) {
			fprintf(stderr, "error making screen surface: first fill failed (%d)\n", err);
			exit(1);
		}
		// set screen properties
		SDL_WM_SetCaption("mywin1", "mywin2");  // window title
		tcolor = SDL_MapRGB(screen->format, 255, 0, 255);  // set transparent color
		vmem = makesurface(640, 480);  // setup background video memory
		// load font
		qbfont = loadsurface("qbfont.bmp");
		SDL_Rect r = { 0, int16_t(480-qbfont->h) };
		SDL_BlitSurface(qbfont, NULL, vid::vmem, &r);  // blit to bottom left corner
		// printf("%x\n", *((uint32_t*)qbfont->pixels) );  // test transparent color
	}

	void quit() {
		SDL_FreeSurface(qbfont);
		SDL_FreeSurface(vmem);
		SDL_Quit();
	}

	SDL_Surface* makesurface(int w, int h) {
		// check bounds
		if (w<0 || w>SURFACE_MAX || h<0 || h>SURFACE_MAX) {
			fprintf(stderr, "error: new surface dimensions of bounds: %d %d\n", w, h);
			exit(1);
		}
		// make surface
		SDL_Surface* box = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
			screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
		if (box==NULL) {
			fprintf(stderr, "error making surface: could not allocate\n");
			exit(1);
		}
		// first fill
		int err = SDL_FillRect(box, NULL, SDL_MapRGB(box->format, 0,0,0));
		if (err) {
			fprintf(stderr, "error making surface: first fill failed (%d)\n", err);
			exit(1);
		}
		// set surface properties
		SDL_SetColorKey(box, SDL_SRCCOLORKEY, tcolor);  // transparency
		return box;  // return
	}

	SDL_Surface* loadsurface(const std::string& name) {
		SDL_Surface *sf  = SDL_LoadBMP(name.c_str());
		if (sf==NULL) {
			fprintf(stderr, "error loading image: %s\n", name.c_str());
			exit(1);
		}
		// SDL_Surface *sfo = sf;
		SDL_Surface *sfo = SDL_DisplayFormat(sf);  // optimize
		SDL_FreeSurface(sf);
		SDL_SetColorKey(sfo, SDL_SRCCOLORKEY, tcolor);  // transparency
		return sfo;
	}

	void scaleto(SDL_Surface* src, SDL_Surface* dst) {
		SDL_Rect r;
		uint32_t* spx = (uint32_t*)src->pixels;
		for (int h=min(src->h,dst->h/2)-1; h>=0; h--)  // go in reverse, in case src=dst
		for (int w=min(src->w,dst->w/2)-1; w>=0; w--) {
			r={ int16_t(w*2), int16_t(h*2), 2,2 };
			SDL_FillRect(dst, &r, spx[h*src->w + w]);
		}
	}

	static int getkeys() {
		keylist={ };
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type==SDL_QUIT)  return -1;
			else if (e.type==SDL_KEYDOWN) {
				uint32_t k = e.key.keysym.sym;
				switch (k) {
				case SDLK_ESCAPE:  return -1;
				case SDLK_F1:  video_mode=1;  break;
				// case SDLK_F2:  video_mode=2;  break;
				case SDLK_F3:  video_mode=3;  break;
				case SDLK_F4:  video_mode=4;  break;
				case SDLK_F5:  video_mode=5;  break;
				default:  keylist.push_back(k);  printf("%x\n", k);  break;
				}
			}
		}
		return 0;
	}

	int flipvid() {
		SDL_Rect r, r2;
		char c;
		switch (video_mode) {
		case 1:  default:  // tty
			SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
			for (int i=0; i<term::texthist.size(); i++)
				for (int j=0; j<term::texthist[i].length(); j++) {
					c  = term::texthist[i][j];
					r  = { int16_t(c%16*8), int16_t(c/16*8), 8,8 };  // source char
					r2 = { int16_t((j+1)*8), int16_t((i+1)*8) };  // dest on screen
					SDL_BlitSurface(qbfont, &r, screen, &r2);
				}
			// vid::scaleto(screen, screen);  // scale up in place before flipping
			break;
		case 2:  // curses mode
			break;
		case 3:  // blit screen
			r={0,0,320,240};  // flip background
			SDL_BlitSurface(vmem, &r, screen, &r);
			for (auto& spr : sprlist) {
				r.x=spr[0],   r.y=spr[1],  r.w=spr[2],  r.h=spr[3];
				r2.x=spr[4],  r2.y=spr[5];
				SDL_BlitSurface(vmem, &r, screen, &r2);
			}
			vid::scaleto(screen, screen);  // scale up in place before flipping
			break;
		case 4:  // display vram
			SDL_BlitSurface(vmem, NULL, screen, NULL);
			break;
		case 5:  // display file vram
			vidf::update();
			SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
			r={0,0};
			for (auto& f : vidf::flist)  SDL_BlitSurface(f.sf, NULL, screen, &r);
			break;
		}
		// flip
		SDL_Flip(screen);
		SDL_Delay(16);  // delay
		return getkeys();
	}
} // end vid



namespace term {
	vector<string> texthist={
		"initialising...",
		"ready."
	};
	// string textpg;
	// std::string& gettextpg();
} // end term



void initvmem() {
	SDL_Rect r, r2;
	// draw main background
	r={0,0,320,240};
	SDL_FillRect(vid::vmem, &r, SDL_MapRGB(vid::vmem->format, 255,0,0));
	// draw sprite in vmem
	r={320,0,40,40};
	SDL_FillRect(vid::vmem, &r, SDL_MapRGB(vid::vmem->format, 255,255,0));
	r={320,0,1,1};
	for (int i=0; i<40; i++) {
		SDL_FillRect(vid::vmem, &r, SDL_MapRGB(vid::vmem->format, 0,0,255));
		r.x++;  r.y++;
	}
	r={330,10,20,20};
	SDL_FillRect(vid::vmem, &r, SDL_MapRGB(vid::vmem->format, 255,0,255));
	// r={10,400,20,20};
	// SDL_FillRect(vid::vmem, &r, SDL_MapRGB(vid::vmem->format, 255,0,255));
	
	r ={0,400,40,40};
	r2={10,250,40,40};
	SDL_BlitSurface(vid::vmem, &r, vid::vmem, &r2);

	// create sprite
	vid::sprlist.push_back({{ 320,0,40,40, 10,10 }});
	vid::sprlist.push_back({{ 10,250,40,40, 100,40 }});

	vid::video_mode=3;
}


int main(int argc, char** argv) {
	printf("hello world\n");
	// system("stat -f \"%N \%m\" *.cpp");  // get file list and timestamp
	vid::init();
	// initvmem();
	vidf::init("vid0");
	vid::video_mode=5;
	// main loop
	int loop=1;
	while (loop) {
		if (vid::flipvid())  loop=0;
	}
	// quit
	vid::quit();
	return 0;
}