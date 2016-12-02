#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <dlfcn.h>

using namespace std;


typedef  void (*voidfn_t) ();
typedef  void (*pipefn_t) (void*, void*, int*, int*);

void* handle         = NULL;
pipefn_t pipeinit    = NULL;
voidfn_t pipethread  = NULL;
voidfn_t pipestep    = NULL;


int load(string path) {
	handle = dlopen(path.c_str(), RTLD_LAZY);
	if (handle == NULL) {
		// err(soname, dlerror());
		cerr << dlerror() << endl;
		return 1;
	}
	// attempt load
	map<string, void*> funclist = {
		{ "pipeinit",    NULL },
		{ "pipethread",  NULL },
		{ "pipestep",    NULL }
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
	pipeinit    = (pipefn_t) funclist["pipeinit"];
	pipethread  = (voidfn_t) funclist["pipethread"];
	pipestep    = (voidfn_t) funclist["pipestep"];
	return 0;
}

int unload() {
	dlclose(handle);
	handle      = NULL;
	pipeinit    = NULL; 
	pipethread  = NULL;
	pipestep    = NULL;
	return 0;
}



stringstream in, out;
int infl = 0, outfl = 0;

void test1() {
	cout << "sub: single test" << endl;
	load("../sub/bin/main.out");
	pipeinit(&in, &out, &infl, &outfl);
	// test 1
	string s = "hello world";
	cout << "writing: " << s << endl;
	in << s;
	infl = 1;
	pipestep();
	// end
	unload();
}
void test2() {
	cout << "sub: thread test" << endl;
	load("../sub/bin/main.out");
	pipeinit(&in, &out, &infl, &outfl);
	// thread test
	thread t(pipethread);
	string s;
	while (cin >> s) {
		if (infl == 0)  in.str(""), in.clear();
		in << s << " ";
		infl = 1;
	}
	infl = -1;
	t.join();
	// end
	unload();
}
void test3() {
	cout << "multiply: step test" << endl;
	load("../mult/bin/main.out");
	pipeinit(&in, &out, &infl, &outfl);
	// single test
	string s;
	in << 1234;
	infl = 1;
	pipestep();
	out >> s;
	cout << "result: " << s << endl;
	// end
	unload();
}

void test4() {
	cout << "xd load" << endl;
	load("../../../xd/xdso/libxd.so");
	pipeinit(&in, &out, &infl, &outfl);
	
	infl = 1;
	pipestep();

	unload();
}


int main() {
	cout << "init hello" << endl;
	// test1();
	// test2();
	// test3();
	test4();
}