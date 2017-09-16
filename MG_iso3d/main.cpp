#include "graphics/graphics.h"
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

using namespace std;


// predef
void setview(string viewid);


// transform
inline void rotatepoint(double& ax1, double& ax2, double rot) {
	static double xt, yt, th;
	th = M_PI/180 * rot;
	xt = ax1*cos(th) - ax2*sin(th);
	yt = ax1*sin(th) + ax2*cos(th);
	ax1=xt, ax2=yt;
}
void transform(char axis, double delta, double* p) {
	switch(axis) {
	case 'r':  rotatepoint(p[0], p[1], delta);  break;
	case 'p':  rotatepoint(p[1], p[2], delta);  break;
	case 'Y':  rotatepoint(p[0], p[2], delta);  break;
	case 'x':  p[0] += delta;  break;
	case 'y':  p[1] += delta;  break;
	case 'z':  p[2] += delta;  break;
	case 's':  p[0]*=delta, p[1]*=delta, p[2]*=delta;  break;
	}
}
inline void transformln(char axis, double delta, double* p) {
	transform(axis, delta, p);
	transform(axis, delta, p+3);
}
vector<pair<char,double>> tglobal;


// models
struct Model {
	string id;
	double x=0, y=0, z=0;
	double roll=0, pitch=0, yaw=0;
	double scale=1;
	array<int,3> col={{255,255,255}};
	vector<array<double,6>> lines;

	void draw() const {
		gfx::drawc(col[0], col[1], col[2]);
		for (auto l : lines) {
			// rotateline(l, roll, pitch, yaw);
			// transform: scale, rotate, move (most predictable)
			transformln('s', scale, &l[0]);
			transformln('r', roll, &l[0]);
			transformln('p', pitch, &l[0]);
			transformln('Y', yaw, &l[0]);
			transformln('x', x, &l[0]);
			transformln('y', y, &l[0]);
			transformln('z', z, &l[0]);
			// apply global transforms
			for (const auto& t : tglobal)
				transformln(t.first, t.second, &l[0]);
			gfx::drawline(SDL_GetVideoSurface(), 160+l[0],120-l[1], 160+l[3],120-l[4]);
			// gfx::drawline(SDL_GetVideoSurface(), 
			// 	x + l[0] * scale,
			// 	y + l[1] * scale,
			// 	x + l[3] * scale,
			// 	y + l[4] * scale );
		}
	}
};
vector<Model> models;
Model& getmodel(const string& id) {
	static Model tmp;
	for (auto& m : models)
		if (m.id==id)  return m;
	return tmp;
}
bool models_sort_z(const Model& l, const Model& r) {
	return (l.y < r.y);  // sort y index lowest first
}


// events
int do_rot=0;
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
				default:  printf("key: %d\n", e.key.keysym.sym);
			}
	}
	return 0;
}


void makemodel(const string& id) {
	Model m;
	m.id=id;
	if (id=="cube") {
		m.y=10;
		m.scale=10;
		m.lines={
			{{0,-1.5,0, 0,1.5,0}},

			{{-1,-1,1, 1,-1,1}},
			{{1,-1,1,  1,1,1}},
			{{1,1,1,  -1,1,1}},
			{{-1,1,1, -1,-1,1}},

			{{-1,-1,-1, 1,-1,-1}},
			{{1,-1,-1,  1,1,-1}},
			{{1,1,-1,  -1,1,-1}},
			{{-1,1,-1, -1,-1,-1}},
		};
		models.push_back(m);
	}
	else if (id=="matrix") {
		m.col={{50,50,50}};
		m.scale=20;
		m.x=-100, m.z=-100;
		for (double i=0; i<=10; i+=1)
			m.lines.push_back({{ i,0,0, i,0,10 }}),
			m.lines.push_back({{ 0,0,i, 10,0,i }});
		models.push_back(m);
	}
	else if (id=="king") {
		m.scale=10;
		m.lines={
			{{-1,-1,0,  1,-1,0}},
			{{-1,-1,0,  -0.70,1,0}},
			{{-0.70,1,0,  -0.35,0,0}},
			{{-0.35,0,0,  0,1,0}},
			{{0,1,0,  0.35,0,0}},
			{{0.35,0,0,  0.70,1,0}},
			{{0.70,1,0,  1,-1,0}}
		};
		models.push_back(m);
	}
}

void setview(string viewid) {
	if (viewid=="" || viewid=="default")  viewid="x";
	if (viewid=="x") {
		tglobal={
			{'Y',0}
			// {'x',160},{'y',120}
		};
	}
	else if (viewid=="z") {
		tglobal={
			{'Y',0},
			{'p',90}
		};
	}
	else if (viewid=="iso1") {
		tglobal={
			{'Y',0},
			{'p',20}
		};
	}
}


int main(int argc, char** argv) {
	gfx::init(640, 480, "iso3d");
	SDL_Surface* scr = SDL_GetVideoSurface();

	setview("default");
	setview("iso1");
	// makemodel("cube");
	makemodel("matrix");
	makemodel("king");

	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		// gfx::drawline(scr, 10, 10, 100, 100);

		// draw all models
		sort(models.begin(), models.end(), models_sort_z);  // force z-index order
		for (const auto& m : models)
			m.draw();
		getmodel("cube").yaw += 2;
		if (do_rot)
			tglobal[0].second += 0.5;

		gfx::scale2x(scr, scr);
		gfx::flip();
		if (pollevents()) break;
	}

	SDL_Quit();
	return 0;
}
