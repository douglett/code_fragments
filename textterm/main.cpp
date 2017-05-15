#include "globals.h"
#include <iostream>

using namespace std;


int main(int argc, char** argv) {
	printf("hello world\n");
	vid::init();
	// vram::init(),  vram::testscene(),  vid::video_mode=3;
	vidf::init("vid0"),  vid::vmode(5);
	// main loop
	int loop=1;
	while (loop) {
		if (vid::flipvid())  loop=0;
	}
	// quit
	vid::quit();
	return 0;
}