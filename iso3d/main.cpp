#include "graphics/graphics.h"
#include <iostream>
#include <vector>
#include <array>
#include <cmath>

using namespace std;


inline void rotatepoint(double& ax1, double& ax2, double rot) {
	static double xt, yt, th;
	th = M_PI/180 * rot;
	xt = ax1*cos(th) - ax2*sin(th);
	yt = ax1*sin(th) + ax2*cos(th);
	ax1=xt, ax2=yt;
}
inline void rotateline(array<double,6>& l, double roll, double pitch, double yaw) {
	rotatepoint(l[0], l[1], roll);
	rotatepoint(l[3], l[4], roll);
	rotatepoint(l[1], l[2], pitch);
	rotatepoint(l[4], l[5], pitch);
	rotatepoint(l[0], l[2], yaw);
	rotatepoint(l[3], l[5], yaw);
}


struct Model {
	double x=0, y=0, scale=1;
	double roll=0, pitch=0, yaw=0;
	vector<array<double,6>> lines;

	void draw() {
		for (auto l : lines) {
			rotateline(l, roll, pitch, yaw);
			gfx::drawline(SDL_GetVideoSurface(), 
				x + l[0] * scale,
				y + l[1] * scale,
				x + l[3] * scale,
				y + l[4] * scale );
		}
	}
	
	void draw_old() {
		double thr = M_PI/180 * roll;   // theta roll
		double thp = M_PI/180 * pitch;  // theta pitch
		double thy = M_PI/180 * yaw;    // theta yaw
		for (auto l : lines) {
			double xt, yt;
			// translate yaw
			xt = l[0]*cos(thy) - l[2]*sin(thy);
			yt = l[0]*sin(thy) + l[2]*cos(thy);
			l[0]=xt, l[2]=yt;
			xt = l[3]*cos(thy) - l[5]*sin(thy);
			yt = l[3]*sin(thy) + l[5]*cos(thy);
			l[3]=xt, l[5]=yt;
			// translate pitch
			xt = l[1]*cos(thp) - l[2]*sin(thp);
			yt = l[1]*sin(thp) + l[2]*cos(thp);
			l[1]=xt, l[2]=yt;
			xt = l[4]*cos(thp) - l[5]*sin(thp);
			yt = l[4]*sin(thp) + l[5]*cos(thp);
			l[4]=xt, l[5]=yt;
			// translate roll
			xt = l[0]*cos(thr) - l[1]*sin(thr);
			yt = l[0]*sin(thr) + l[1]*cos(thr);
			l[0]=xt, l[1]=yt;
			xt = l[3]*cos(thr) - l[4]*sin(thr);
			yt = l[3]*sin(thr) + l[4]*cos(thr);
			l[3]=xt, l[4]=yt;
			// draw
			gfx::drawline(SDL_GetVideoSurface(), 
				x + l[0] * scale,
				y + l[1] * scale,
				x + l[3] * scale,
				y + l[4] * scale );
		}
	}
};
vector<Model> models;


int main(int argc, char** argv) {
	gfx::init(640, 480, "iso3d");
	SDL_Surface* scr = SDL_GetVideoSurface();

	Model m;
	m.x=160, m.y=120;
	m.scale=30;
	m.roll=30;
	m.pitch=30;
	// m.yaw=30;
	m.lines={
		{{-1,-1,1, 1,-1,1}},
		{{1,-1,1,  1,1,1}},
		{{1,1,1,  -1,1,1}},
		{{-1,1,1, -1,-1,1}},

		{{-1,-1,-1, 1,-1,-1}},
		{{1,-1,-1,  1,1,-1}},
		{{1,1,-1,  -1,1,-1}},
		{{-1,1,-1, -1,-1,-1}},
	};

	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		// gfx::drawline(scr, 10, 10, 100, 100);

		m.draw();
		m.yaw += 2;

		gfx::scale2x(scr, scr);
		gfx::flip();
	}

	SDL_Quit();
	return 0;
}