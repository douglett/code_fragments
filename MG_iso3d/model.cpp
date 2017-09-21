#include "globals.h"
#include "graphics/graphics.h"

using namespace std;


// model class definitions
std::vector<std::pair<char,double>> Model::tglobal={ {'Y',0} };
void Model::draw() const {
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
		gfx::drawline(SDL_GetVideoSurface(), 160+l[0], 120-l[1], 160+l[3], 120-l[4]);
		// gfx::drawline(SDL_GetVideoSurface(), 
		// 	x + l[0] * scale,
		// 	y + l[1] * scale,
		// 	x + l[3] * scale,
		// 	y + l[4] * scale );
	}
}


// build helper
static lines_t linebuilder(vector<double> points, vector<int> join) {
	vector<array<double,6>> lines;
	for (int i=0; i<join.size()-1; i+=2) {
		double* p = &points[ join[i]   * 3 ];
		double* q = &points[ join[i+1] * 3 ];
		lines.push_back({{ p[0], p[1], p[2], q[0], q[1], q[2] }});
	}
	return lines;
}

// model line vectors
static const lines_t 
MOD_cube={
	{{0,-1.5,0, 0,1.5,0}},

	{{-1,-1,1, 1,-1,1}},
	{{1,-1,1,  1,1,1}},
	{{1,1,1,  -1,1,1}},
	{{-1,1,1, -1,-1,1}},

	{{-1,-1,-1, 1,-1,-1}},
	{{1,-1,-1,  1,1,-1}},
	{{1,1,-1,  -1,1,-1}},
	{{-1,1,-1, -1,-1,-1}},
},
MOD_pyramid=linebuilder({
		-1,-1,-1,  -1,-1,1,  1,-1,-1,  1,-1,1,  0,1,0,
		0,-1,-1,  0,1,-1
	},{
		0,1, 0,2, 2,3, 1,3, 4,0, 4,1, 4,2, 4,3,
		5,6
}),
MOD_king1=linebuilder({
	-1,-1,0,  -1,0,0,  -0.70,1,0.5,  -0.55,0,1,  -0.55,-1,1,  // crown 1
	1,-1,0,  1,0,0,  0.70,1,0.5,  0.55,0,1,  0.55,-1,1,  // crown 2
	0,1,1,  0,0,0, 0,0,0, 0,0,0, 0,0,0,  // crown 3
	-0.70,1,-0.5,  -0.55,0,-1,  -0.55,-1,-1,  0,0,0,  0,0,0,  // crown b1
	0.70,1,-0.5,  0.55,0,-1,  0.55,-1,-1,  0,0,0,  0,0,0,  // crown b2
	0,1,-1  // crown b3
},{
	0,1, 1,2, 2,3, 3,4, 4,0,  // crown 1
	5,6, 6,7, 7,8, 8,9, 9,5,  // crown 2
	8,10, 3,10, 4,9,  // crown 3
	1,15, 15,16, 16,17, 17,0,  // crown b1
	6,20, 20,21, 21,22,  22,5,  // crown b2
	25,21, 25,16, 17,22  // crown b3
}),
MOD_heart_r=linebuilder({
	0,0.6,0,  0.3,1,0,  0.7,1,0,  1,0.6,0,   1,0,0,   0,-1,0
},{
	0,1, 1,2, 2,3, 3,4, 4,5
});


// make by id
Model makemodel(const string& type, const string& id) {
	Model m;
	m.id=type+"-"+id;
	if (type=="cube") {
		m.y=10;
		m.scale=10;
		m.lines=MOD_cube;
	}
	else if (type=="pyramid") {
		m.y=10;
		m.scale=8;
		m.lines=MOD_pyramid;
	}
	else if (type=="matrix") {
		m.col={{50,50,50}};
		m.scale=20;
		m.x=-100, m.z=-100;
		for (double i=0; i<=10; i+=1)
			m.lines.push_back({{ i,0,0, i,0,10 }}),
			m.lines.push_back({{ 0,0,i, 10,0,i }});
	}
	else if (type=="king") {
		m.scale=8;
		m.y=10;
		m.lines=MOD_king1;
	}
	else if (type=="heart") {
		m.scale=8;
		m.y=10;
		m.lines=MOD_heart_r;
		// left side
		lines_t test=MOD_heart_r;
		for (auto& l : test)
			l[0]*=-1, l[3]*=-1;
		m.lines.insert(m.lines.end(), test.begin(), test.end());
		// 3d effect
		// test=m.lines;
		// for (int i=0; i<m.lines.size(); i++)
		// 	m.lines[i][2] = m.lines[i][5] = 0.5,
		// 	test[i][2 ]= test[i][5] = -0.5;
		// m.lines.insert(m.lines.end(), test.begin(), test.end());
	}
	return m;
}