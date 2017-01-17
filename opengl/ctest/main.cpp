#include <iostream>
#include <vector>
#include <common/core.h>

using namespace std;


int main() {
	printf("hello\n");

	// GLfloat a[20] = { 0 };
	// printf("%d  %d  %d \n", sizeof(GLfloat), sizeof(int), sizeof(a));
	// return 0;

	common::init();

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
		common::addVertex( vl[i], vl[i+1], vl[i+2] );
	// simple tri
	common::addTri({ 1.5f,1.5f,2.0f,    2.0f,2.0f,2.0f,    2.0f,1.5f,2.0f });
	// send
	common::sendVertexes();

	while (common::running) {
		common::paint();
	}
	common::quit();
}