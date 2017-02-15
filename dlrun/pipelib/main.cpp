#include <iostream>
#include <string>
#include "pipelib.h"

using namespace std;


int sdl2_test();
int bmp_test();


int main() {
	printf("pipelib test\n");
	// bmp_test();
	sdl2_test();
}


int sdl2_test() {
	pipelib::load("sdl2test");
	// pipelib::send("sdl2test", "mainloop");
	// cout << pipelib::response << endl;
	pipelib::send("sdl2test", "init");
	while (true) {
		if (pipelib::send("sdl2test", "paint"))  break;
	}
	pipelib::send("sdl2test", "quit");
	pipelib::unload("sdl2test");
	return 0;
}


int bmp_test() {
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
	return 0;
}