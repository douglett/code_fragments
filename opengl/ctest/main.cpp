#include <iostream>
#include <common/core.h>

using namespace std;


int main() {
	printf("hello\n");

	common::init();
	while (common::running) {
		common::paint();
	}
	common::quit();
}