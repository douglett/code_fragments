#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <dlfcn.h>

using namespace std;


typedef  void (*voidfn_t) ();
typedef  void (*pipefn_t) (void*, void*, int*, int*);

void* handle            = NULL;
pipefn_t PIPE_init      = NULL;
voidfn_t PIPE_dothread  = NULL;
voidfn_t PIPE_step      = NULL;


int load(string path) {
	handle = dlopen(path.c_str(), RTLD_LAZY);
	if (handle == NULL) {
		// err(soname, dlerror());
		cerr << dlerror() << endl;
		return 1;
	}
	// attempt load
	map<string, void*> funclist = {
		{ "PIPE_init",      NULL },
		{ "PIPE_dothread",  NULL },
		{ "PIPE_step",      NULL }
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
	PIPE_init      = (pipefn_t) funclist["PIPE_init"];
	PIPE_dothread  = (voidfn_t) funclist["PIPE_dothread"];
	PIPE_step      = (voidfn_t) funclist["PIPE_step"];
	return 0;
}

int unload() {
	dlclose(handle);
	handle         = NULL;
	PIPE_init      = NULL; 
	PIPE_dothread  = NULL;
	PIPE_step      = NULL;
	return 0;
}



stringstream in, out;
int infl = 0, outfl = 0;

void test1() {
	cout << "sub: single test" << endl;
	load("../sub/bin/main.out");
	PIPE_init(&in, &out, &infl, &outfl);
	// test 1
	string s = "hello world";
	cout << "writing: " << s << endl;
	in << s;
	infl = 1;
	PIPE_step();
	// end
	unload();
}
void test2() {
	cout << "sub: thread test" << endl;
	load("../sub/bin/main.out");
	PIPE_init(&in, &out, &infl, &outfl);
	// thread test
	thread t(PIPE_dothread);
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
	PIPE_init(&in, &out, &infl, &outfl);
	// single test
	string s;
	in << 1234;
	infl = 1;
	PIPE_step();
	out >> s;
	cout << "result: " << s << endl;
	// end
	unload();
}


int main() {
	cout << "init hello" << endl;
	// test1();
	// test2();
	test3();
}