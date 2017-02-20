#include <iostream>
#include <string>
#include <sstream>
#include "pipelib/pipelib.h"

using namespace std;


int  sdl2_test();
void bmp_test();


int main() {
	printf("pipelib test\n");
	// bmp_test();
	return  sdl2_test();
}


template<typename ... Args>
string strfmt(const string& fmt, Args ... args) {
	int size = snprintf(NULL, 0, fmt.c_str(), args ...) + 1; // Extra space for '\0'
	char cs[size];
	snprintf(cs, size, fmt.c_str(), args ...);
	return cs;
}


int sdl2_test() {
	pipelib::dblock_t data;

	pipelib::dlpath = "./libs/";
	if (pipelib::loadall({ "bmpload", "sdl2test" }))  return 1;

	// cout << strfmt("cls %02x%02x%02x%02x", 0x88, 0x00, 0x00, 0xff) << endl;
	// cout << strfmt("cls %08x", 0x880000ff) << endl;

	pipelib::send("sdl2test", "init");
	pipelib::send("sdl2test", "cls 0x880000ff");
	if (pipelib::send("bmpload", "load asd.bmp", &data))  return 1;  // load into slot 1
	pipelib::push32(data, { 20, 40 });  // blit data in slot 1, to pos in slot 2
	pipelib::send("sdl2test", "blit", &data);  // do blit

	while (true) 
		if (pipelib::send("sdl2test", "paint"))  break;

	pipelib::unloadall();
	return 0;
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