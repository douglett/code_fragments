#include <iostream>
#include <string>
#include "pipelib.h"

using namespace std;


void sdl2_test1();
void sdl2_test2();
void bmp_test();


int main() {
	printf("pipelib test\n");
	// bmp_test();
	sdl2_test2();
}


void sdl2_test1() {
	pipelib::load("sdl2test");
	pipelib::send("sdl2test", "mainloop");
	cout << pipelib::response << endl;
	pipelib::unload("sdl2test");
}


void sdl2_test2() {
	pipelib::load("sdl2test");
	pipelib::send("sdl2test", "init");

	int w = 100, h = 100;
	auto& data = pipelib::data = { };
	data.emplace_back(2 * 4);
	data.emplace_back((2 + w * h) * 4);
	auto* d1 = (uint32_t*) &data[0][0];
	d1[0] = d1[1] = 20;
	auto* d2 = (uint32_t*) &data[1][0];
	d2[0] = w,  d2[1] = h;
	fill_n(d2+2, w*h, 0xff00ffff);
	pipelib::send("sdl2test", "blit");

	while (true) {
		if (pipelib::send("sdl2test", "paint"))  break;
	}

	pipelib::send("sdl2test", "quit");
	pipelib::unload("sdl2test");
}


void bmp_test() {
	pipelib::load("bmpload");
	// test basic functions
	pipelib::send("bmpload", "info");
	cout << pipelib::response << endl;
	pipelib::send("bmpload", "cmdlist");
	cout << pipelib::response << endl;
	// test bmp functions
	cout << "---" << endl;
	pipelib::send("bmpload", "load test.bmp");
	pipelib::unload("bmpload");
}