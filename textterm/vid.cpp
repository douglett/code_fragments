#include "globals.h"
#include <iostream>

using namespace std;


static void loadqbfont();
static void makeqbfont();


namespace vid {
	// public
	SDL_Surface  *screen=NULL,  *qbfont=NULL;
	uint32_t     tcolor=0,  fgcolor=0,  errcolor=0;
	vector<uint32_t> keylist;
	// private
	static int video_mode=0;

	void init() {
		logfmt("initialising...");
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
		tcolor   = SDL_MapRGB(screen->format, 255, 0, 255);  // set transparent color
		fgcolor  = SDL_MapRGB(screen->format, 255, 255, 0);  // set text foreground color
		errcolor = SDL_MapRGB(screen->format, 255, 0, 0);  // set text foreground color
		( true ? makeqbfont() : loadqbfont() );  // load font
		logfmt("ready.");
	}

	void quit() {
		vidf::quit();
		vram::quit();
		SDL_FreeSurface(qbfont);
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

	SDL_Surface* textc(char c) {
		return textc(c, fgcolor);
	}
	SDL_Surface* textc(char c, uint32_t col) {
		static SDL_Rect      rs={0,0, 8,8};
		static SDL_Surface*  sf=NULL;
		if (sf==NULL)  sf = vid::makesurface(8, 8);
		rs.x=(c%16)*8,  rs.y=(c/16)*8;  // source char
		SDL_FillRect(sf, NULL, col);
		SDL_BlitSurface(vid::qbfont, &rs, sf, NULL);
		return sf;
	}

	void scaleto(SDL_Surface* src, SDL_Surface* dst) {
		SDL_Rect r={0,0, 2,2};
		uint32_t* spx = (uint32_t*)src->pixels;
		for (int h=min(src->h,dst->h/2)-1; h>=0; h--)  // go in reverse, in case src=dst
		for (int w=min(src->w,dst->w/2)-1; w>=0; w--) {
			r.x=w*2,  r.y=h*2;
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
				case SDLK_F1:  vmode(1);  break;
				case SDLK_F2:  vmode(2);  break;
				case SDLK_F3:  vmode(3);  break;
				case SDLK_F4:  vmode(4);  break;
				case SDLK_F5:  vmode(5);  break;
				default:  keylist.push_back(k);  printf("%x\n", k);  break;
				}
			}
		}
		return 0;
	}

	int vmode(int mode) {
		// if (mode > -1)  video_mode = mode;
		switch (mode) {
			case 0:  break;  // just return
			case 1:  video_mode=1;  break;  // tty
			// case 2:  video_mode=2;  break;  // tty curses
			case 3:  if (vram::is_init())  video_mode=3;  break;  // vram sprite flip
			case 4:  if (vram::is_init())  video_mode=4;  break;  // vram debug
			case 5:  if (vidf::is_init())  video_mode=5;  break;  // video file mode
			default:  errfmt("error: unknown video mode: %d", mode);
		}
		return video_mode;
	}

	int flipvid() {
		switch (video_mode) {
		case 1:  default:  term::flip();  break;  // tty
		case 2:  break;  // curses mode
		case 3:  vram::flip();  break;
		case 4:  vram::dbgflip();  break;
		case 5:  vidf::flip();  break;  // display file vram
		}
		// flip
		SDL_Flip(screen);
		SDL_Delay(16);  // delay
		return getkeys();
	}
} // end vid



static void loadqbfont() {
	// load font from file
	vid::qbfont = vid::loadsurface("qbfont.bmp");
	// some setup work
	int len = vid::qbfont->w * vid::qbfont->h;
	printf("%d %d  [%d]\n", vid::qbfont->w, vid::qbfont->h, len);
	auto* px = (uint32_t*)vid::qbfont->pixels;
	// display loop
	switch (0) {
	case 0:  break;  // don't show
	case 1:  // raw data
		for (int i=0; i<len; i++) {
			printf("%04x ", px[i]);
			if (i>0 && i%8==0)      printf("\n");
		}
		printf("\n");
		break;
	case 2:  // raw binary
		for (int i=0; i<len; i++) {
			if      (px[i]==0xff00ff00)  printf("0");
			else if (px[i]==0xffffff00)  printf("1");
			if (i>0 && i%8==0)      printf(" ");
			if (i>0 && i%(8*8)==0)  printf("\n");
		}
		printf("\n");
		break;
	case 3:  // binary as hex
		for (int i=0; i<len; i+=32) {
			uint32_t v=0;
			for (int j=0; j<32; j++)  v = (v<<1) | (px[i+j]==0xffffff00 ? 1 : 0);
			if (i>0 && i%(32*8)==0)  printf("\n");
			printf("0x%08x, ", v);
		}
		printf("\n");
		break;
	}
}


static const vector<uint32_t> QBFONT_DATA={
0x007e7e6c, 0x10381000, 0xff00ff0f, 0x3c3f7f99, 0x0081fffe, 0x387c1000, 0xff3cc307, 0x6633635a,
0x00a5dbfe, 0x7c383818, 0xe766990f, 0x663f7f3c, 0x0081fffe, 0xfefe7c3c, 0xc342bd7d, 0x663063e7,
0x00bdc37c, 0x7cfefe3c, 0xc342bdcc, 0x3c3063e7, 0x0099e738, 0x38d67c18, 0xe76699cc, 0x1870673c,
0x0081ff10, 0x10101000, 0xff3cc3cc, 0x7ef0e65a, 0x007e7e00, 0x00383800, 0xff00ff78, 0x18e0c099,
0x80021866, 0x7f7e0018, 0x18180000, 0x00000000, 0xe00e3c66, 0xdbc3003c, 0x3c181830, 0x002418ff,
0xf83e7e66, 0xdb78007e, 0x7e180c60, 0xc0663cff, 0xfefe1866, 0x7bcc0018, 0x1818fefe, 0xc0ff7e7e,
0xf83e1866, 0x1bcc7e7e, 0x187e0c60, 0xc066ff3c, 0xe00e7e00, 0x1b787e3c, 0x183c1830, 0xfe24ff18,
0x80023c66, 0x1b8c7e18, 0x18180000, 0x00000000, 0x00001800, 0x00f800ff, 0x00000000, 0x00000000,
0x00306c6c, 0x30003860, 0x18600000, 0x00000006, 0x00786c6c, 0x7cc66c60, 0x30306630, 0x0000000c,
0x00786cfe, 0xc0cc38c0, 0x60183c30, 0x00000018, 0x0030006c, 0x78187600, 0x6018fffc, 0x00fc0030,
0x003000fe, 0x0c30dc00, 0x60183c30, 0x00000060, 0x0000006c, 0xf866cc00, 0x30306630, 0x700030c0,
0x0030006c, 0x30c67600, 0x18600000, 0x30003080, 0x00000000, 0x00000000, 0x00000000, 0x60000000,
0x78307878, 0x1cfc38fc, 0x78780000, 0x18006078, 0xccf0cccc, 0x3cc060cc, 0xcccc0000, 0x300030cc,
0xdc300c0c, 0x6cf8c00c, 0xcccc3030, 0x60fc180c, 0xfc303838, 0xcc0cf818, 0x787c3030, 0xc0000c18,
0xec30600c, 0xfe0ccc30, 0xcc0c0000, 0x60fc1830, 0xcc30cccc, 0x0ccccc60, 0xcc183070, 0x30003000,
0x78fcfc78, 0x0c787860, 0x78703030, 0x18006030, 0x00000000, 0x00000000, 0x00000060, 0x00000000,
0x7c30fc3c, 0xfcfefe3c, 0xcc781ee6, 0xf0c6c638, 0xc6786666, 0x6c626266, 0xcc300c66, 0x60eee66c,
0xdecc66c0, 0x666868c0, 0xcc300c6c, 0x60fef6c6, 0xdecc7cc0, 0x667878c0, 0xfc300c78, 0x60d6dec6,
0xdefc66c0, 0x666868ce, 0xcc30cc6c, 0x62c6cec6, 0xc0cc6666, 0x6c626066, 0xcc30cc66, 0x66c6c66c,
0x78ccfc3c, 0xfcfef03e, 0xcc7878e6, 0xfec6c638, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xfc78fc78, 0xfcccccc6, 0xc6ccfe78, 0xc0781000, 0x66cc66cc, 0xb4ccccc6, 0xc6cccc60, 0x60183800,
0x66cc66e0, 0x30ccccc6, 0x6ccc9860, 0x30186c00, 0x7ccc7c38, 0x30ccccd6, 0x38783060, 0x1818c600,
0x60dc781c, 0x30ccccfe, 0x6c306260, 0x0c180000, 0x60786ccc, 0x30cc78ee, 0xc630c660, 0x06180000,
0xf01ce678, 0x78fc30c6, 0xc678fe78, 0x02780000, 0x00000000, 0x00000000, 0x00000000, 0x000000ff,
0x3000e000, 0x1c003800, 0xe03018e0, 0x70000000, 0x30006000, 0x0c006c00, 0x60000060, 0x30000000,
0x18787c78, 0x0c786076, 0x6c707866, 0x30ecf878, 0x000c66cc, 0x7cccf0cc, 0x7630186c, 0x30fecccc,
0x007c66c0, 0xccfc60cc, 0x66301878, 0x30d6cccc, 0x00cc66cc, 0xccc0607c, 0x6630186c, 0x30c6cccc,
0x0076bc78, 0x7678f00c, 0xe678d8e6, 0x78c6cc78, 0x00000000, 0x000000f8, 0x00007000, 0x00000000,
0x00000000, 0x10000000, 0x0000001c, 0x18e07610, 0x00000000, 0x30000000, 0x00000030, 0x1830dc38,
0xdc76d87c, 0x7cccccc6, 0xc6ccfc30, 0x1830006c, 0x66cc6cc0, 0x30ccccc6, 0x6ccc98e0, 0x001c00c6,
0x66cc6c78, 0x30ccccd6, 0x38cc3030, 0x183000c6, 0x7c7c600c, 0x34cc78fe, 0x6c7c6430, 0x183000c6,
0x600cf0f8, 0x1876306c, 0xc60cfc1c, 0x18e000fe, 0xf01e0000, 0x00000000, 0x00f80000, 0x00000000,
0x78001c7e, 0xcce03000, 0x7ecce0cc, 0x7ce0cc30, 0xcccc00c3, 0x00003000, 0xc3000000, 0xc6003030,
0xc000783c, 0x7878787c, 0x3c787870, 0x38707800, 0xcccccc06, 0x0c0c0cc0, 0x66cccc30, 0x1830cc78,
0x78ccfc3e, 0x7c7c7cc0, 0x7efcfc30, 0x1830cccc, 0x18ccc066, 0xcccccc7c, 0x60c0c030, 0x1830fcfc,
0x0c7e783f, 0x7e7e7e06, 0x3c787878, 0x3c78cccc, 0x78000000, 0x0000003c, 0x00000000, 0x00000000,
0x1c003e78, 0x00007800, 0x00c6cc18, 0x38ccf00e, 0x00006ccc, 0xcce0cce0, 0xcc380018, 0x6cccd81b,
0xfc7fcc00, 0x00000000, 0x007ccc7e, 0x6478d818, 0x600cfe78, 0x7878cccc, 0xccc6ccc0, 0xf0fcf47e,
0x787fcccc, 0xcccccccc, 0xccc6ccc0, 0x6030cc18, 0x60cccccc, 0xcccccccc, 0xfc7ccc7e, 0xe6fcde18,
0xfc7fce78, 0x78787e7e, 0x0c387818, 0xfc30ccd8, 0x00000000, 0x00000000, 0xf8000018, 0x00000e70,
0x1c380000, 0x00fc3c3c, 0x300000c6, 0xc6000000, 0x00001c1c, 0xf8006c66, 0x000000cc, 0xcc1833cc,
0x78700000, 0x00cc6c66, 0x300000d8, 0xd8006666, 0x0c3078cc, 0xf8ec3e3c, 0x60fcfc3e, 0xf318cc33,
0x7c30cccc, 0xccfc0000, 0xc0c00c63, 0x67186666, 0xcc30cccc, 0xccdc7e7e, 0xccc00cce, 0xcf3c33cc,
0x7e78787e, 0xcccc0000, 0x78000098, 0x9f3c0000, 0x00000000, 0x00000000, 0x0000001f, 0x03180000,
0x2255dd18, 0x18183600, 0x00363600, 0x36361800, 0x88aa7718, 0x18183600, 0x00363600, 0x36361800,
0x2255dd18, 0x18f83600, 0xf8f636fe, 0xf636f800, 0x88aa7718, 0x18183600, 0x18063606, 0x06361800,
0x2255dd18, 0xf8f8f6fe, 0xf8f636f6, 0xfefef8f8, 0x88aa7718, 0x18183636, 0x18363636, 0x00000018,
0x2255dd18, 0x18183636, 0x18363636, 0x00000018, 0x88aa7718, 0x18183636, 0x18363636, 0x00000018,
0x18180018, 0x00181836, 0x36003600, 0x36003618, 0x18180018, 0x00181836, 0x36003600, 0x36003618,
0x18180018, 0x00181f36, 0x373ff7ff, 0x37fff7ff, 0x18180018, 0x00181836, 0x30300000, 0x30000000,
0x1fffff1f, 0xffff1f37, 0x3f37fff7, 0x37fff7ff, 0x00001818, 0x00181836, 0x00360036, 0x36003600,
0x00001818, 0x00181836, 0x00360036, 0x36003600, 0x00001818, 0x00181836, 0x00360036, 0x36003600,
0x36000036, 0x18000036, 0x181800ff, 0x00f00fff, 0x36000036, 0x18000036, 0x181800ff, 0x00f00fff,
0x36ff0036, 0x1f1f0036, 0xff1800ff, 0x00f00fff, 0x36000036, 0x18180036, 0x001800ff, 0x00f00fff,
0xffffff3f, 0x1f1f3ff7, 0xfff81fff, 0xfff00f00, 0x00183600, 0x00183636, 0x180018ff, 0xfff00f00,
0x00183600, 0x00183636, 0x180018ff, 0xfff00f00, 0x00183600, 0x00183636, 0x180018ff, 0xfff00f00,
0x00000000, 0xfe000000, 0xfc38381c, 0x00063c78, 0x0078fefe, 0x66006676, 0x306c6c30, 0x000c60cc,
0x76ccc66c, 0x307e66dc, 0x78c6c618, 0x7e7ec0cc, 0xdcf8c06c, 0x18cc6618, 0xccfec67c, 0xdbdbfccc,
0xc8ccc06c, 0x30cc6618, 0xccc66ccc, 0xdbdbc0cc, 0xdcf8c06c, 0x66cc7c18, 0x786c6ccc, 0x7e7e60cc,
0x76c0c06c, 0xfe786018, 0x3038ee78, 0x00603ccc, 0x00c00000, 0x0000c000, 0xfc000000, 0x00c00000,
0x00306018, 0x0e183000, 0x3800000f, 0x78780000, 0xfc303030, 0x1b183072, 0x6c00000c, 0x6c0c0000,
0x00fc1860, 0x1b18009c, 0x6c00000c, 0x6c383c00, 0xfc303030, 0x1818fc00, 0x3818000c, 0x6c603c00,
0x00306018, 0x18180072, 0x001818ec, 0x6c7c3c00, 0xfc000000, 0x18d8309c, 0x0000006c, 0x00003c00,
0x00fcfcfc, 0x18d83000, 0x0000003c, 0x00000000, 0x00000000, 0x18700000, 0x0000001c, 0x00000000,
};

static void makeqbfont() {
	// load font from DATA (above)
	if (vid::qbfont==NULL)  vid::qbfont = vid::makesurface(128, 128);
	auto* px = (uint32_t*)vid::qbfont->pixels;
	uint32_t  i=0,  white=SDL_MapRGB(vid::qbfont->format, 255,255,255),  bit;
	for (uint32_t d : QBFONT_DATA) {
		for (int j=0; j<32; j++) {
			bit = (d >> (31-j)) & 1;
			px[i+j] = ( bit ? white : vid::tcolor );
		}
		i+=32;
	}
	SDL_SetColorKey(vid::qbfont, SDL_SRCCOLORKEY, white);  // transparency
}