#include "globals.h"
#include "graphics/graphics.h"
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

using namespace std;


// predef
int do_rot=0;
// globals
vector<Model> models;

// model helpers
Model& getmodel(const string& id) {
	static Model tmp;
	for (auto& m : models)
		if (m.id==id)  return m;
	return tmp;
}
bool models_sort_z(const Model& l, const Model& r) {
	return (l.y < r.y);  // sort y index lowest first
}
void setview(string viewid) {
	if (viewid=="" || viewid=="default")  viewid="x";
	if      (viewid=="x")  Model::tglobal={ {'Y',0} };
	else if (viewid=="z")  Model::tglobal={ {'Y',0}, {'p',90} };
	else if (viewid=="iso1")  Model::tglobal={ {'Y',0}, {'p',20} };
	else if (viewid=="iso2")  Model::tglobal={ {'Y',-20}, {'p',35} };
}


// events
int pollevents() {
	static int last_key=0;
	SDL_Event e;
	int keydir;
	while (SDL_PollEvent(&e))
	switch (e.type) {
		case SDL_QUIT:  return 1;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			last_key=0;
			keydir=(e.type==SDL_KEYDOWN);
			switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  return 1;
				case 'r':  if (keydir==1) do_rot=1;  break;
				case 'x':  do_rot=0;  setview("x");  break;
				case 'z':  do_rot=0;  setview("z");  break;
				case 'i':  do_rot=0;  setview("iso1");  break;
				case 'o':  do_rot=0;  setview("iso2");  break;
				case SDLK_DOWN:  if (keydir) getmodel("pyramid-1").z+=20;  break;
				case SDLK_UP:  if (keydir) getmodel("pyramid-1").z-=20;  break;
				case SDLK_LEFT:  if (keydir) getmodel("pyramid-1").x-=20;  break;
				case SDLK_RIGHT:  if (keydir) getmodel("pyramid-1").x+=20;  break;
				default:  printf("key: %d\n", e.key.keysym.sym);
			}
	}
	return 0;
}


int main(int argc, char** argv) {
	gfx::init(640, 480, "iso3d");
	SDL_Surface* scr = SDL_GetVideoSurface();
	gfx::handleEvents = 0;

	setview("default");
	setview("iso1");

	int scene=2;
	models.push_back( makemodel("matrix", "1") );
	switch (scene) {
	case 1:
		// makemodel("cube");
		// makemodel("king", "1");
		for (int i=0; i<10; i++) {
			models.push_back( makemodel("pyramid", to_string(i+1)) );
			string id="pyramid-"+to_string(i+1);
			getmodel(id).z = 10 - 100;
			getmodel(id).x = 10 + i*20 - 100;
		}
		break;
	case 2:
		models.push_back( makemodel("heart", "1") );
		models.push_back( makemodel("spade", "1") );
		models.push_back( makemodel("diamond", "1") );
		models.push_back( makemodel("club", "1") );
		getmodel("heart-1").z = 10-100;
		getmodel("heart-1").x = 10-100 + 0*20;
		// getmodel("heart-1").scale=30;
		getmodel("spade-1").z = 10-100;
		getmodel("spade-1").x = 10-100 + 1*20;
		// getmodel("spade-1").scale=30;
		getmodel("diamond-1").z = 10-100;
		getmodel("diamond-1").x = 10-100 + 2*20;
		// getmodel("diamond-1").scale=30;
		getmodel("club-1").z = 10-100;
		getmodel("club-1").x = 10-100 + 3*20;
		// getmodel("club-1").scale=30;
		models.push_back( makemodel("pyramid", "1") );
		getmodel("pyramid-1").z = 10-100 + 2*20;
		getmodel("pyramid-1").x = 10-100 + 2*20;
		break;
	}

	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		// gfx::drawline(scr, 10, 10, 100, 100);

		// draw all models
		sort(models.begin(), models.end(), models_sort_z);  // force z-index order
		for (const auto& m : models)
			m.draw();
		getmodel("pyramid-1").yaw += 2;
		if (do_rot)
			Model::tglobal[0].second += 0.5;

		gfx::scale2x(scr, scr);
		gfx::flip();
		if (pollevents()) break;
	}

	SDL_Quit();
	return 0;
}
