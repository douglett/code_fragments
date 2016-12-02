#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <dlfcn.h>
#include "pipe.h"

using namespace std;


int main() {
	cout << "init hello" << endl;

	// setup
	auto lib = make_shared<pipe>();
	if (load(lib, "../../xd/xdso/", "libxd.so"))  return 1;
	
	message(lib, "[init]");
	message(lib, "[make] test 10 10 50 50");
	string resp;
	while (true) {
		resp = message(lib, "[step]");
		if (resp == "[quit]")  break;
	}
	message(lib, "[quit]");
	
	// unload
	unload(lib);
}