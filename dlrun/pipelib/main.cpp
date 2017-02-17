#include <iostream>
#include <string>
#include "pipelib.h"

using namespace std;


void sdl2_test();
void bmp_test();


int main() {
	printf("pipelib test\n");
	// bmp_test();
	sdl2_test();
}


void sdl2_test() {
	pipelib::dblock_t data;

	pipelib::loadall({ "bmpload", "sdl2test" });

	pipelib::send("sdl2test", "init");
	pipelib::send("bmpload", "load asd.bmp", &data);  // load into slot 1
	pipelib::push32(data, { 20, 40 });  // blit data in slot 1, to pos in slot 2
	pipelib::send("sdl2test", "blit", &data);  // do blit

	while (true) 
		if (pipelib::send("sdl2test", "paint"))  break;

	pipelib::unloadall();
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