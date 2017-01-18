#include <iostream>
#include <vector>
#include <common/core.h>

using namespace std;


common::GLobj obj, obj2;


void test2() {
	printf("test2\n");
	// box
	vector<GLfloat> vl = {
		-1.0f,-1.0f,-1.0f,   -1.0f,-1.0f, 1.0f,   -1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,   -1.0f,-1.0f,-1.0f,   -1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,   -1.0f,-1.0f,-1.0f,    1.0f,-1.0f,-1.0f,

		 1.0f, 1.0f,-1.0f,    1.0f,-1.0f,-1.0f,   -1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,   -1.0f, 1.0f, 1.0f,   -1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,   -1.0f,-1.0f, 1.0f,   -1.0f,-1.0f,-1.0f,
		
		-1.0f, 1.0f, 1.0f,   -1.0f,-1.0f, 1.0f,    1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,    1.0f,-1.0f,-1.0f,    1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f,    1.0f,-1.0f, 1.0f,
		
		 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,-1.0f,   -1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,   -1.0f, 1.0f,-1.0f,   -1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,   -1.0f, 1.0f, 1.0f,    1.0f,-1.0f, 1.0f
	};
	for (int i = 0; i < vl.size(); i += 3)
		obj.addVertex( vl[i], vl[i+1], vl[i+2] );
	// send
	obj.sendVertexes();
	common::objlist.push_back(&obj);

	// simple tri
	obj2.addTri({ 1.5f,1.5f,2.0f,    2.0f,2.0f,2.0f,    2.0f,1.5f,2.0f });
	obj2.sendVertexes();
	common::objlist.push_back(&obj2);
}


int main() {
	printf("start\n");
	common::init();

	// test1();
	test2();

	while (common::running) {
		common::paint();
	}
	common::quit();
}