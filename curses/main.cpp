#include <iostream>
#include "game.h"

using namespace std;


int main() {
	cout << "hello world" << endl;
	scr::init();

	while (true) {
		if (scr::get_input())  break;
	}

	scr::quit();
}