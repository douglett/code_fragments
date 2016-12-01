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
struct pipe {
	void* handle         = NULL;
	pipefn_t pipeinit    = NULL;
	voidfn_t pipethread  = NULL;
	voidfn_t pipestep    = NULL;
	string name;
};


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

int load(shared_ptr<pipe> p, string path, string name) {
	int err = load(path + name);
	if (err)  return err;
	p->handle      = handle;
	p->pipeinit    = pipeinit;
	p->pipethread  = pipethread;
	p->pipestep    = pipestep;
	p->name        = name;
	return 0;
}

int unload(shared_ptr<pipe> p) {
	dlclose(p->handle);
	p->handle      = NULL;
	p->pipeinit    = NULL; 
	p->pipethread  = NULL;
	p->pipestep    = NULL;
	p->name        = "";
	return 0;
}



//*** main ***

stringstream in, out;
int infl = 0, outfl = 0;

string message(shared_ptr<pipe> p, const string& msg) {
	// clear
	in.str(""), in.clear();
	// send
	in << msg;
	infl = 1;
	p->pipestep();
	// response
	string resp;
	if (outfl > 0) {
		getline(out, resp);
		out.str(""), out.clear();
		outfl = 0;
	}
	return resp;
}


int main() {
	cout << "init hello" << endl;

	// setup
	auto lib = make_shared<pipe>();
	int err = load(lib, "../../xd/xdso/", "libxd.so");
	if (err)  return 1;
	lib->pipeinit(&in, &out, &infl, &outfl);
	
	message(lib, "[init]");
	string resp;
	while (true) {
		resp = message(lib, "[step]");
		if (resp == "[quit]")  break;
	}
	message(lib, "[quit]");
	
	// unload
	unload(lib);
}