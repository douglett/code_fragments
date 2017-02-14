#include <iostream>
#include <string>
#include "pipelib.h"

using namespace std;


int main() {
	printf("pipelib test\n");

	// pipelib::load("bmpload");
	// // test basic functions
	// pipelib::send("bmpload", "info");
	// cout << pipelib::response << endl;
	// pipelib::send("bmpload", "cmdlist");
	// cout << pipelib::response << endl;
	// // test bmp functions
	// cout << "---" << endl;
	// pipelib::send("bmpload", "load test.bmp");

	pipelib::send("sdl2test", "mainloop");
	cout << pipelib::response << endl;
}