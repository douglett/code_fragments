#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <dlfcn.h>

using namespace std;


typedef  void (*voidfn_t) ();
typedef  void (*pipefn_t) (void*, void*, int*, int*);

void* handle       = NULL;
pipefn_t initpipe  = NULL;
voidfn_t dothread  = NULL;
voidfn_t step      = NULL;


int load(string path) {
	handle = dlopen(path.c_str(), RTLD_LAZY);
	if (handle == NULL) {
		// err(soname, dlerror());
		cerr << dlerror() << endl;
		return 1;
	}
	// attempt load
	map<string, void*> funclist = {
		{ "initpipe",  NULL },
		{ "dothread",  NULL },
		{ "step",      NULL }
	};
	for (auto& fn : funclist) {
		fn.second = dlsym(handle, fn.first.c_str());
		if (fn.second == NULL) {
			// err(soname, dlerror());
			cerr << dlerror() << endl;
			dlclose(handle);
			return 1;
		}
	}
	// save
	initpipe  = (pipefn_t) funclist["initpipe"];
	dothread  = (voidfn_t) funclist["dothread"];
	step      = (voidfn_t) funclist["step"];
	return 0;
}

int unload() {
	dlclose(handle);
	return 0;
}


int main() {
	cout << "init" << endl;
	load("../sub/bin/main.out");

	stringstream in, out;
	int infl = 0, outfl = 0;
	initpipe(&in, &out, &infl, &outfl);

	// test 1
	string s = "hello world";
	cout << "writing: " << s << endl;
	in << s;
	infl = 1;
	step();

	// thread test
	thread t(dothread);
	while (cin >> s) {
		if (infl == 0)  in.str(""), in.clear();
		in << s << " ";
		infl = 1;
	}
	infl = -1;
	t.join();

	// loop t
	// while (cin >> s) {
	// 	cout << s << " ";
	// }

	unload();
}